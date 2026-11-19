#ifndef TEMPERATURE_APP_H
#define TEMPERATURE_APP_H
#include "stm32l4xx_hal.h"
void TemperatureApp_Init(I2C_HandleTypeDef *i2c, UART_HandleTypeDef *uart);
void TemperatureApp_Task(void);
#endif
