#include "main.h"

int main(void)
{
    System_Clock_Config(); // 配置系统时钟
    gpio_init();           // 初始化 GPIO
    Timer14_1ms_init();
    Timer1_10us_init();
    uart_init(); // 初始化 UART

    //语音模块测试
    for(int i = SPEAK_STOP; i <= SPEAK_CANCEL; i++)
    {
        speak(i); 
        LL_mDelay(3000);
    }

    
    
    while (1)
    {
        app_control(); // 处理1ms和1s周期内的应用逻辑
    }
}
