#include "main.h"

int main(void)
{
    System_Clock_Config(); // 配置系统时钟
    gpio_init();           // 初始化 GPIO
    Timer14_1ms_init();
    Timer1_10us_init();
    uart_init(); // 初始化 UART
                 // Flash_init(); // 初始化 Flash，恢复数据到 RAM 变量
                 // 语音模块测测试
    SPEAKING(SPEAK_STUDY_SUCCESS);
    LL_mDelay(3000);
    
    flash_read_buf(FLASH_USER_START_ADDR, DATA, 64); // 从Flash读取数据到DATA数组
    APP_Get_Flash_Init_Data(); // 将Flash数据加载到应用层变量

    Flash_update_process();
    while (1)
    {
        APP_Control(); // 处理1ms和1s周期内的应用逻辑
    }
}
