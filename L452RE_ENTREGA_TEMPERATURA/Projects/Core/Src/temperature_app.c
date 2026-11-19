#include "temperature_app.h"

#define SENSOR_ADDRESS  (0x48U << 1)
#define SAMPLE_PERIOD_MS 500U
#define IO_TIMEOUT_MS    100U

typedef enum
{
    APP_IDLE,
    APP_READ_IDENTITY,
    APP_ENABLE_SENSOR,
    APP_READ_TEMPERATURE,
    APP_SEND_RESULT
} App_State;

static I2C_HandleTypeDef *i2c;
static UART_HandleTypeDef *uart;
static App_State state;
static TemperatureApp_Result final_result;
static uint32_t last_sample;
static uint32_t operation_start;
static uint8_t identity;
static uint8_t control = 1U;
static uint8_t temperature_bytes[2];
static uint8_t uart_buffer[96];
static volatile uint8_t i2c_done;
static volatile uint8_t i2c_error;
static volatile uint8_t uart_done;
static volatile uint8_t uart_error;

static void FinishCycle(uint32_t now)
{
    last_sample = now;
    state = APP_IDLE;
    i2c_done = 0U;
    i2c_error = 0U;
    uart_done = 0U;
    uart_error = 0U;
}

static TemperatureApp_Result I2cWait(uint32_t now)
{
    if (i2c_error)
        return TEMPERATURE_APP_RESULT_I2C_ERROR;

    if (!i2c_done)
    {
        if ((uint32_t)(now - operation_start) < IO_TIMEOUT_MS)
            return TEMPERATURE_APP_RESULT_BUSY;

        (void)HAL_I2C_Master_Abort_IT(i2c, SENSOR_ADDRESS);
        return TEMPERATURE_APP_RESULT_I2C_ERROR;
    }

    i2c_done = 0U;
    i2c_error = 0U;
    return TEMPERATURE_APP_RESULT_OK;
}

static TemperatureApp_Result SendText(uint32_t now, const char *text,
                                      TemperatureApp_Result result)
{
    uint16_t length = 0U;

    while (*text != '\0')
        uart_buffer[length++] = (uint8_t)*text++;

    uart_buffer[length++] = '\r';
    uart_buffer[length++] = '\n';
    final_result = result;
    uart_done = 0U;
    uart_error = 0U;
    operation_start = now;
    state = APP_SEND_RESULT;

    if (HAL_UART_Transmit_IT(uart, uart_buffer, length) != HAL_OK)
        return TEMPERATURE_APP_RESULT_UART_ERROR;

    return TEMPERATURE_APP_RESULT_BUSY;
}

static uint16_t AppendNumber(uint16_t position, uint32_t number)
{
    uint32_t divisor = 1U;

    while (number / divisor >= 10U)
        divisor *= 10U;

    while (divisor > 0U)
    {
        uart_buffer[position++] = (uint8_t)('0' + (number / divisor));
        number %= divisor;
        divisor /= 10U;
    }
    return position;
}

void TemperatureApp_Init(I2C_HandleTypeDef *i2c_handle, UART_HandleTypeDef *uart_handle)
{
    i2c = i2c_handle;
    uart = uart_handle;
    state = APP_IDLE;
    last_sample = HAL_GetTick() - SAMPLE_PERIOD_MS;
    i2c_done = 0U;
    i2c_error = 0U;
    uart_done = 0U;
    uart_error = 0U;
}

TemperatureApp_Result TemperatureApp_Task(void)
{
    uint32_t now = HAL_GetTick();
    TemperatureApp_Result wait_result;

    if (i2c == NULL || uart == NULL)
        return TEMPERATURE_APP_RESULT_NOT_INITIALIZED;

    switch (state)
    {
    case APP_IDLE:
        if ((uint32_t)(now - last_sample) < SAMPLE_PERIOD_MS)
            return TEMPERATURE_APP_RESULT_BUSY;

        i2c_done = 0U;
        i2c_error = 0U;
        operation_start = now;
        state = APP_READ_IDENTITY;

        if (HAL_I2C_Mem_Read_IT(i2c, SENSOR_ADDRESS, 0x02U,
                                I2C_MEMADD_SIZE_8BIT, &identity, 1U) != HAL_OK)
        {
            FinishCycle(now);
            return TEMPERATURE_APP_RESULT_I2C_ERROR;
        }
        return TEMPERATURE_APP_RESULT_BUSY;

    case APP_READ_IDENTITY:
        wait_result = I2cWait(now);
        if (wait_result != TEMPERATURE_APP_RESULT_OK)
        {
            if (wait_result == TEMPERATURE_APP_RESULT_I2C_ERROR)
                FinishCycle(now);
            return wait_result;
        }

        if (identity != 0x26U)
        {
            wait_result = SendText(now,
                                   "Error: identidad del sensor incorrecta",
                                   TEMPERATURE_APP_RESULT_IDENTITY_ERROR);
            if (wait_result != TEMPERATURE_APP_RESULT_BUSY)
                FinishCycle(now);
            return wait_result;
        }

        i2c_done = 0U;
        i2c_error = 0U;
        operation_start = now;
        state = APP_ENABLE_SENSOR;

        if (HAL_I2C_Mem_Write_IT(i2c, SENSOR_ADDRESS, 0x00U,
                                 I2C_MEMADD_SIZE_8BIT, &control, 1U) != HAL_OK)
        {
            FinishCycle(now);
            return TEMPERATURE_APP_RESULT_I2C_ERROR;
        }
        return TEMPERATURE_APP_RESULT_BUSY;

    case APP_ENABLE_SENSOR:
        wait_result = I2cWait(now);
        if (wait_result != TEMPERATURE_APP_RESULT_OK)
        {
            if (wait_result == TEMPERATURE_APP_RESULT_I2C_ERROR)
                FinishCycle(now);
            return wait_result;
        }

        i2c_done = 0U;
        i2c_error = 0U;
        operation_start = now;
        state = APP_READ_TEMPERATURE;

        if (HAL_I2C_Mem_Read_IT(i2c, SENSOR_ADDRESS, 0x01U,
                                I2C_MEMADD_SIZE_8BIT, temperature_bytes, 2U) != HAL_OK)
        {
            FinishCycle(now);
            return TEMPERATURE_APP_RESULT_I2C_ERROR;
        }
        return TEMPERATURE_APP_RESULT_BUSY;

    case APP_READ_TEMPERATURE:
        wait_result = I2cWait(now);
        if (wait_result != TEMPERATURE_APP_RESULT_OK)
        {
            if (wait_result == TEMPERATURE_APP_RESULT_I2C_ERROR)
                FinishCycle(now);
            return wait_result;
        }

        {
            uint16_t raw = ((uint16_t)temperature_bytes[0] << 8) |
                           temperature_bytes[1];
            uint16_t length;
            int32_t centi;
            uint32_t magnitude;
            uint32_t fraction;

            if (raw == 0x8000U)
            {
                wait_result = SendText(now, "Temperatura no disponible",
                                       TEMPERATURE_APP_RESULT_UNAVAILABLE);
                if (wait_result != TEMPERATURE_APP_RESULT_BUSY)
                    FinishCycle(now);
                return wait_result;
            }

            centi = raw < 0x8000U ? (int32_t)raw : (int32_t)raw - 65536;
            magnitude = centi < 0 ? (uint32_t)-centi : (uint32_t)centi;
            fraction = magnitude % 100U;
            length = 0U;

            const char *text = "Temperatura: ";
            while (*text != '\0')
                uart_buffer[length++] = (uint8_t)*text++;

            if (centi < 0)
                uart_buffer[length++] = '-';

            length = AppendNumber(length, magnitude / 100U);
            uart_buffer[length++] = '.';
            if (fraction < 10U)
                uart_buffer[length++] = '0';
            length = AppendNumber(length, fraction);

            text = " C";
            while (*text != '\0')
                uart_buffer[length++] = (uint8_t)*text++;
            uart_buffer[length++] = '\r';
            uart_buffer[length++] = '\n';

            final_result = TEMPERATURE_APP_RESULT_OK;
            uart_done = 0U;
            uart_error = 0U;
            operation_start = now;
            state = APP_SEND_RESULT;

            if (HAL_UART_Transmit_IT(uart, uart_buffer, length) != HAL_OK)
            {
                FinishCycle(now);
                return TEMPERATURE_APP_RESULT_UART_ERROR;
            }
            return TEMPERATURE_APP_RESULT_BUSY;
        }

    case APP_SEND_RESULT:
        if (uart_error)
        {
            FinishCycle(now);
            return TEMPERATURE_APP_RESULT_UART_ERROR;
        }
        if (uart_done)
        {
            wait_result = final_result;
            FinishCycle(now);
            return wait_result;
        }
        if ((uint32_t)(now - operation_start) >= IO_TIMEOUT_MS)
        {
            (void)HAL_UART_AbortTransmit_IT(uart);
            FinishCycle(now);
            return TEMPERATURE_APP_RESULT_UART_ERROR;
        }
        return TEMPERATURE_APP_RESULT_BUSY;
    }

    FinishCycle(now);
    return TEMPERATURE_APP_RESULT_BUSY;
}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *handle)
{
    if (handle == i2c)
        i2c_done = 1U;
}

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *handle)
{
    if (handle == i2c)
        i2c_done = 1U;
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *handle)
{
    if (handle == i2c)
        i2c_error = 1U;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *handle)
{
    if (handle == uart)
        uart_done = 1U;
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *handle)
{
    if (handle == uart)
        uart_error = 1U;
}
