#ifndef TEMPERATURE_APP_H
#define TEMPERATURE_APP_H
#include "stm32l4xx_hal.h"

typedef enum
{
    TEMPERATURE_APP_RESULT_BUSY = 0,
    TEMPERATURE_APP_RESULT_OK,
    TEMPERATURE_APP_RESULT_NOT_INITIALIZED,
    TEMPERATURE_APP_RESULT_I2C_ERROR,
    TEMPERATURE_APP_RESULT_IDENTITY_ERROR,
    TEMPERATURE_APP_RESULT_UNAVAILABLE,
    TEMPERATURE_APP_RESULT_UART_ERROR
} TemperatureApp_Result;

void TemperatureApp_Init(I2C_HandleTypeDef *i2c, UART_HandleTypeDef *uart);
TemperatureApp_Result TemperatureApp_Task(void);
#endif
