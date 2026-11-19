#include "temperature_sensor.h"
#include "temperature_slave.h"

enum { i2c_timeout_ms = 100 };
static I2C_HandleTypeDef *sensor_bus;

static HAL_StatusTypeDef read_register(uint8_t reg, uint8_t *data, uint16_t size)
{
    return HAL_I2C_Mem_Read(sensor_bus, TEMPERATURE_SLAVE_I2C_ADDR << 1,
                            reg, I2C_MEMADD_SIZE_8BIT, data, size, i2c_timeout_ms);
}

static HAL_StatusTypeDef write_register(uint8_t reg, uint8_t value)
{
    return HAL_I2C_Mem_Write(sensor_bus, TEMPERATURE_SLAVE_I2C_ADDR << 1,
                             reg, I2C_MEMADD_SIZE_8BIT, &value, 1, i2c_timeout_ms);
}

bool temperature_sensor_init(I2C_HandleTypeDef *i2c)
{
    sensor_bus = i2c;
    /* Enable measurements once at startup. */
    return write_register(TEMPERATURE_SLAVE_REG_CONTROL, 1) == HAL_OK;
}

bool temperature_sensor_read(int32_t *centi)
{
    uint8_t bytes[2];
    if (read_register(TEMPERATURE_SLAVE_REG_TEMPERATURE, bytes, sizeof bytes) != HAL_OK)
        return false;

    uint16_t raw = ((uint16_t)bytes[0] << 8) | bytes[1];
    if (raw == TEMPERATURE_SLAVE_UNAVAILABLE)
        return false;

    if (raw < 0x8000U) *centi = (int32_t)raw;
    else *centi = (int32_t)raw - 65536;
    return true;
}
