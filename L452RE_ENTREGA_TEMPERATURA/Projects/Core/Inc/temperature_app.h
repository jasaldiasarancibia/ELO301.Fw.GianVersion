#ifndef TEMPERATURE_APP_H
#define TEMPERATURE_APP_H
#include "temperature_sensor.h"
bool temperature_app_init(I2C_HandleTypeDef *i2c, UART_HandleTypeDef *uart);
void temperature_app_task(void);
#endif
