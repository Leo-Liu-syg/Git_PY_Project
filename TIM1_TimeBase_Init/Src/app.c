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
        LED5_TOG();
    }
}