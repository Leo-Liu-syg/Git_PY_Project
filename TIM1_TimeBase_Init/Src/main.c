#include "main.h"

int main(void)
{
    APP_SystemClockConfig();
    base_1ms_init();
    base_10us_init();
    gpio_init();
    // uart_init();
    flash_init();

    while (1)
    {
        time_control();
    }
}
