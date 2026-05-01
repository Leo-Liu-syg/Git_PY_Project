#include "main.h"

int main(void)
{
    System_Clock_Config(); // 配置系统时钟
    gpio_init();           // 初始化 GPIO
    Timer14_1ms_init();
    Timer1_10us_init();
    uart_init(); // 初始化 UART
    
    for(int i = SPEAK_STOP; i <= SPEAK_CANCEL; i++)
    {
        speak(i); // 语音提示：安全报警，安全报警，请立即做好安全防护措施+警报声
        LL_mDelay(5000);
    }
    
    while (1)
    {
        app_control(); // 处理1ms和1s周期内的应用逻辑
    }
}
