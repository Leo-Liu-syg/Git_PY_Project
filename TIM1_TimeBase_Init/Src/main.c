#include "main.h"

int main(void)
{
    System_Clock_Config(); // 配置系统时钟
    gpio_init();           // 初始化 GPIO
    Timer14_1ms_init();
    Timer1_10us_init();
    uart_init(); // 初始化 UART
    // Flash_init(); // 初始化 Flash，恢复数据到 RAM 变量
    //语音模块测测试
        speak(5); 
        LL_mDelay(3000);

    // Flash_Update_Control();
    // Flash_Recovery();
    Flash_update_process();
    while (1)
    {
        app_control(); // 处理1ms和1s周期内的应用逻辑
    }
}
