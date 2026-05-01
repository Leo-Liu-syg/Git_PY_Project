// 包含系统驱动头文件
#include "sys.h"

// 包含主函数头文件（芯片寄存器、宏定义）


/**
  * @brief  系统时钟初始化配置（让芯片跑起来）
  * @param  无
  * @retval 无
  */
void System_Clock_Config(void)
{
    /* 使能 SYSCFG 系统配置控制器时钟 */
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SYSCFG);

    /* 使能内部高速时钟 HSI（芯片自带的8MHz/24MHz时钟源） */
    LL_RCC_HSI_Enable();

    /* 等待 HSI 时钟稳定 准备好 */
    while(LL_RCC_HSI_IsReady() != 1)
    {
        // 这里空循环，一直等到时钟准备好才往下走
    }

    /* 设置 AHB 预分频器 = 不分频
       HCLK = SYSCLK（系统时钟不减速）
    */
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);

    /* 选择 HSISYS 作为系统时钟 SYSCLK */
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSISYS);

    /* 等待系统时钟切换完成 */
    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSISYS)
    {
        // 空循环，等待切换成功
    }

    /* 设置 APB1 预分频器 = 不分频 */
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);

    /* 配置系统滴答定时器，1ms 中断
       这里系统时钟 = 24MHz
    */
    LL_Init1msTick(24000000);

    /* 更新系统核心时钟变量 = 24MHz
       告诉库函数当前系统主频是多少
    */
    LL_SetSystemCoreClock(24000000);
}
