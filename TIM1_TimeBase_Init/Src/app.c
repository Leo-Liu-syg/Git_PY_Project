#include "app.h"

void app_control(void)
{
    if (Flag_1ms) // 在这里添加1ms周期内需要执行的代码
    {
        Flag_1ms = 0;
    }
    if (Flag_1s) // 在这里添加1s周期内需要执行的代码
    {
        Flag_1s = 0;
        LED1_TOG();
        LED2_TOG();
        LED3_TOG();
        LED4_TOG();
        LED5_TOG();                  // 清除接收数据，准备接收下一个字节
        
        if(rx_data != 0)              // 如果接收到了数据
        {
            USart1_Send_ASCII_Back(rx_data); // 回显接收到的数据
            rx_data = 0;                 // 这里可以添加其他1s周期内需要执行的操作
        }
        
    }
}