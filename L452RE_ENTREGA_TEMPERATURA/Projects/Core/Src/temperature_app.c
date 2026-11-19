#include "temperature_app.h"
#include "temperature_console.h"

static UART_HandleTypeDef *console_uart;

bool temperature_app_init(I2C_HandleTypeDef *i2c, UART_HandleTypeDef *uart)
{
    console_uart = uart;
    return temperature_sensor_init(i2c);
}

void temperature_app_task(void)
{
    int32_t centi;
    /* Only print when a valid temperature was received. */
    if (temperature_sensor_read(&centi))
        temperature_console_print(console_uart, centi);
}
