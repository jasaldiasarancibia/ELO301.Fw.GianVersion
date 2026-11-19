#include "temperature_app.h"
#include <stdio.h>

/* I2C1 reads the course emulator on I2C3 through external wires. */
#define SENSOR_ADDRESS (0x48U << 1)
#define IO_TIMEOUT_MS 100U
#define SAMPLE_PERIOD_MS 500U
static I2C_HandleTypeDef *bus;
static UART_HandleTypeDef *serial;
static uint32_t last_sample;

static HAL_StatusTypeDef read_register(uint8_t reg, uint8_t *data, uint16_t size)
{
    return HAL_I2C_Mem_Read(bus, SENSOR_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT,
                            data, size, IO_TIMEOUT_MS);
}

void TemperatureApp_Init(I2C_HandleTypeDef *i2c, UART_HandleTypeDef *uart)
{
    bus = i2c;
    serial = uart;
    last_sample = HAL_GetTick() - SAMPLE_PERIOD_MS;
}

void TemperatureApp_Task(void)
{
    uint32_t now = HAL_GetTick();
    uint8_t identity = 0, control = 1, bytes[2];
    const char *status = "OK";
    int32_t centi = 0;
    char line[100];
    int length;
    if (bus == NULL || serial == NULL || (uint32_t)(now - last_sample) < SAMPLE_PERIOD_MS)
        return;
    last_sample = now;
    if (read_register(0x02, &identity, 1) != HAL_OK) status = "I2C_ERROR";
    else if (identity != 0x26) status = "IDENTITY_ERROR";
    /* Enable each cycle, so the slave can recover independently after a reset. */
    else if (HAL_I2C_Mem_Write(bus, SENSOR_ADDRESS, 0x00, I2C_MEMADD_SIZE_8BIT,
                              &control, 1, IO_TIMEOUT_MS) != HAL_OK) status = "I2C_ERROR";
    else if (read_register(0x01, bytes, 2) != HAL_OK) status = "I2C_ERROR";
    else {
        uint16_t raw = ((uint16_t)bytes[0] << 8) | bytes[1];
        if (raw == 0x8000U) status = "UNAVAILABLE";
        else centi = raw < 0x8000U ? (int32_t)raw : (int32_t)raw - 65536;
    }
    /* Preserve the minus sign for values such as -0.25 C. No float printf. */
    if (status[0] == 'O') {
        int32_t magnitude = centi < 0 ? -centi : centi;
        length = snprintf(line, sizeof line, "Temperatura: %s%ld.%02ld C\r\n",
                          centi < 0 ? "-" : "", (long)(magnitude / 100),
                          (long)(magnitude % 100));
    } else {
        const char *message = status[0] == 'I' && status[1] == '2'
            ? "Error I2C: revisar cables y resistencias pull-up"
            : status[0] == 'U' ? "Temperatura no disponible"
            : "Error: identidad del sensor incorrecta";
        length = snprintf(line, sizeof line, "%s\r\n", message);
    }
    if (length > 0 && length < (int)sizeof line)
        (void)HAL_UART_Transmit(serial, (uint8_t *)line, (uint16_t)length, IO_TIMEOUT_MS);
}
