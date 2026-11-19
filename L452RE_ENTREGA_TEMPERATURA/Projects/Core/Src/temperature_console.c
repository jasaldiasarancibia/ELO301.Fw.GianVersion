#include "temperature_console.h"
#include <stdio.h>

void temperature_console_print(UART_HandleTypeDef *uart, int32_t centi)
{
    char line[40];
    int32_t magnitude = centi < 0 ? -centi : centi;
    int length = snprintf(line, sizeof line, "Temperatura: %s%ld.%02ld C\r\n",
                          centi < 0 ? "-" : "", (long)(magnitude / 100),
                          (long)(magnitude % 100));

    if (length > 0 && length < (int)sizeof line)
        (void)HAL_UART_Transmit(uart, (uint8_t *)line, (uint16_t)length, 100U);
}
