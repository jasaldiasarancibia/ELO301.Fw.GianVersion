#include "temperature_app.h"

/* I2C1 reads the course emulator on I2C3 through external wires. */
#define SENSOR_ADDRESS (0x48U << 1)
#define IO_TIMEOUT_MS 100U
#define SAMPLE_PERIOD_MS 500U

typedef enum
{
    APP_STATE_IDLE,
    APP_STATE_WAIT_IDENTITY,
    APP_STATE_WAIT_CONTROL_WRITE,
    APP_STATE_WAIT_TEMPERATURE,
    APP_STATE_WAIT_UART
} TemperatureApp_State;

static I2C_HandleTypeDef *bus;
static UART_HandleTypeDef *serial;
static uint32_t last_sample;
static uint32_t operation_start;
static TemperatureApp_State state = APP_STATE_IDLE;
static uint8_t identity;
static uint8_t temperature_bytes[2];
static uint8_t control = 1;
static uint8_t uart_buffer[96];
static uint16_t uart_length;
static volatile uint8_t i2c_operation_done;
static volatile uint8_t i2c_operation_error;
static volatile uint8_t uart_operation_done;
static TemperatureApp_Result pending_result;

static uint16_t append_text(uint16_t position, const char *text)
{
    while (*text != '\0' && position < sizeof(uart_buffer))
        uart_buffer[position++] = (uint8_t)*text++;
    return position;
}

static uint16_t append_uint32(uint16_t position, uint32_t value)
{
    uint32_t divisor = 1U;

    while (value / divisor >= 10U)
        divisor *= 10U;

    while (divisor > 0U)
    {
        uart_buffer[position++] = (uint8_t)('0' + (value / divisor));
        value %= divisor;
        divisor /= 10U;
    }
    return position;
}

static void finish_cycle(uint32_t now)
{
    last_sample = now;
    state = APP_STATE_IDLE;
    i2c_operation_done = 0U;
    i2c_operation_error = 0U;
}

static TemperatureApp_Result start_uart(const char *message)
{
    HAL_StatusTypeDef status;

    uart_length = append_text(0U, message);
    if (uart_length < sizeof(uart_buffer))
    {
        uart_buffer[uart_length++] = '\r';
        uart_buffer[uart_length++] = '\n';
    }

    uart_operation_done = 0U;
    operation_start = HAL_GetTick();
    state = APP_STATE_WAIT_UART;
    status = HAL_UART_Transmit_IT(serial, uart_buffer, uart_length);
    if (status != HAL_OK)
        return TEMPERATURE_APP_RESULT_UART_ERROR;

    return TEMPERATURE_APP_RESULT_BUSY;
}

void TemperatureApp_Init(I2C_HandleTypeDef *i2c, UART_HandleTypeDef *uart)
{
    bus = i2c;
    serial = uart;
    last_sample = HAL_GetTick() - SAMPLE_PERIOD_MS;
    state = APP_STATE_IDLE;
    i2c_operation_done = 0U;
    i2c_operation_error = 0U;
    uart_operation_done = 0U;
    pending_result = TEMPERATURE_APP_RESULT_BUSY;
}

TemperatureApp_Result TemperatureApp_Task(void)
{
    uint32_t now = HAL_GetTick();

    if (bus == NULL || serial == NULL)
        return TEMPERATURE_APP_RESULT_NOT_INITIALIZED;

    if (state == APP_STATE_IDLE)
    {
        if ((uint32_t)(now - last_sample) < SAMPLE_PERIOD_MS)
            return TEMPERATURE_APP_RESULT_BUSY;

        i2c_operation_done = 0U;
        i2c_operation_error = 0U;
        operation_start = now;
        state = APP_STATE_WAIT_IDENTITY;
        if (HAL_I2C_Mem_Read_IT(bus, SENSOR_ADDRESS, 0x02U,
                                I2C_MEMADD_SIZE_8BIT, &identity, 1U) != HAL_OK)
        {
            finish_cycle(now);
            return TEMPERATURE_APP_RESULT_I2C_ERROR;
        }
        return TEMPERATURE_APP_RESULT_BUSY;
    }

    if (state == APP_STATE_WAIT_IDENTITY)
    {
        if (i2c_operation_error)
        {
            finish_cycle(now);
            return TEMPERATURE_APP_RESULT_I2C_ERROR;
        }
        if (!i2c_operation_done)
        {
            if ((uint32_t)(now - operation_start) >= IO_TIMEOUT_MS)
            {
                (void)HAL_I2C_Master_Abort_IT(bus, SENSOR_ADDRESS);
                finish_cycle(now);
                return TEMPERATURE_APP_RESULT_I2C_ERROR;
            }
            return TEMPERATURE_APP_RESULT_BUSY;
        }

        i2c_operation_done = 0U;
        if (identity != 0x26U)
        {
            pending_result = TEMPERATURE_APP_RESULT_IDENTITY_ERROR;
            if (start_uart("Error: identidad del sensor incorrecta") == TEMPERATURE_APP_RESULT_UART_ERROR)
            {
                finish_cycle(now);
                return TEMPERATURE_APP_RESULT_UART_ERROR;
            }
            return TEMPERATURE_APP_RESULT_BUSY;
        }

        operation_start = now;
        state = APP_STATE_WAIT_CONTROL_WRITE;
        if (HAL_I2C_Mem_Write_IT(bus, SENSOR_ADDRESS, 0x00U,
                                 I2C_MEMADD_SIZE_8BIT, &control, 1U) != HAL_OK)
        {
            finish_cycle(now);
            return TEMPERATURE_APP_RESULT_I2C_ERROR;
        }
        return TEMPERATURE_APP_RESULT_BUSY;
    }

    if (state == APP_STATE_WAIT_CONTROL_WRITE)
    {
        if (i2c_operation_error)
        {
            finish_cycle(now);
            return TEMPERATURE_APP_RESULT_I2C_ERROR;
        }
        if (!i2c_operation_done)
        {
            if ((uint32_t)(now - operation_start) >= IO_TIMEOUT_MS)
            {
                (void)HAL_I2C_Master_Abort_IT(bus, SENSOR_ADDRESS);
                finish_cycle(now);
                return TEMPERATURE_APP_RESULT_I2C_ERROR;
            }
            return TEMPERATURE_APP_RESULT_BUSY;
        }

        i2c_operation_done = 0U;
        operation_start = now;
        state = APP_STATE_WAIT_TEMPERATURE;
        if (HAL_I2C_Mem_Read_IT(bus, SENSOR_ADDRESS, 0x01U,
                                I2C_MEMADD_SIZE_8BIT, temperature_bytes, 2U) != HAL_OK)
        {
            finish_cycle(now);
            return TEMPERATURE_APP_RESULT_I2C_ERROR;
        }
        return TEMPERATURE_APP_RESULT_BUSY;
    }

    if (state == APP_STATE_WAIT_TEMPERATURE)
    {
        if (i2c_operation_error)
        {
            finish_cycle(now);
            return TEMPERATURE_APP_RESULT_I2C_ERROR;
        }
        if (!i2c_operation_done)
        {
            if ((uint32_t)(now - operation_start) >= IO_TIMEOUT_MS)
            {
                (void)HAL_I2C_Master_Abort_IT(bus, SENSOR_ADDRESS);
                finish_cycle(now);
                return TEMPERATURE_APP_RESULT_I2C_ERROR;
            }
            return TEMPERATURE_APP_RESULT_BUSY;
        }

        i2c_operation_done = 0U;
        {
            uint16_t raw = ((uint16_t)temperature_bytes[0] << 8) | temperature_bytes[1];
            if (raw == 0x8000U)
            {
                pending_result = TEMPERATURE_APP_RESULT_UNAVAILABLE;
                if (start_uart("Temperatura no disponible") == TEMPERATURE_APP_RESULT_UART_ERROR)
                {
                    finish_cycle(now);
                    return TEMPERATURE_APP_RESULT_UART_ERROR;
                }
                return TEMPERATURE_APP_RESULT_BUSY;
            }

            {
                int32_t centi = raw < 0x8000U
                    ? (int32_t)raw
                    : (int32_t)raw - 65536;
                uint32_t magnitude = centi < 0 ? (uint32_t)-centi : (uint32_t)centi;

                uart_length = append_text(0U, "Temperatura: ");
                if (centi < 0)
                    uart_buffer[uart_length++] = '-';
                uart_length = append_uint32(uart_length, magnitude / 100U);
                uart_buffer[uart_length++] = '.';
                {
                    uint32_t fraction = magnitude % 100U;
                    if (fraction < 10U)
                        uart_buffer[uart_length++] = '0';
                    uart_length = append_uint32(uart_length, fraction);
                }
                uart_length = append_text(uart_length, " C");

                if (uart_length < sizeof(uart_buffer))
                {
                    uart_buffer[uart_length++] = '\r';
                    uart_buffer[uart_length++] = '\n';
                }

                uart_operation_done = 0U;
                pending_result = TEMPERATURE_APP_RESULT_OK;
                operation_start = now;
                state = APP_STATE_WAIT_UART;
                if (HAL_UART_Transmit_IT(serial, uart_buffer, uart_length) != HAL_OK)
                {
                    finish_cycle(now);
                    return TEMPERATURE_APP_RESULT_UART_ERROR;
                }
                return TEMPERATURE_APP_RESULT_BUSY;
            }
        }
    }

    if (state == APP_STATE_WAIT_UART)
    {
        if (uart_operation_done)
        {
            uart_operation_done = 0U;
            finish_cycle(now);
            return pending_result;
        }
        if ((uint32_t)(now - operation_start) >= IO_TIMEOUT_MS)
        {
            (void)HAL_UART_AbortTransmit_IT(serial);
            finish_cycle(now);
            return TEMPERATURE_APP_RESULT_UART_ERROR;
        }
        return TEMPERATURE_APP_RESULT_BUSY;
    }

    finish_cycle(now);
    return TEMPERATURE_APP_RESULT_BUSY;
}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c == bus)
        i2c_operation_done = 1U;
}

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c == bus)
        i2c_operation_done = 1U;
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c == bus)
        i2c_operation_error = 1U;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == serial)
        uart_operation_done = 1U;
}
