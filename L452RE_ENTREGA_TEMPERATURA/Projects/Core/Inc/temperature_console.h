#ifndef TEMPERATURE_CONSOLE_H
#define TEMPERATURE_CONSOLE_H
#include "stm32l4xx_hal.h"
void temperature_console_print(UART_HandleTypeDef *uart, int32_t centi);
#endif
