#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H
#include "stm32l4xx_hal.h"
#include <stdbool.h>
bool temperature_sensor_init(I2C_HandleTypeDef *i2c);
bool temperature_sensor_read(int32_t *centi);
#endif
