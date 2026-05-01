#include "main.h"

int main(void)
{
    System_Clock_Config(); // 配置系统时钟
    gpio_init();           // 初始化 GPIO
    Timer14_1ms_init();
    Timer1_10us_init();
    uart_init(); // 初始化 UART

    while (1)
    {
        app_control(); // 处理1ms和1s周期内的应用逻辑
    }
}
