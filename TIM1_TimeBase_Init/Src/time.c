// 包含定时器相关头文件
#include "time.h"

/**
 * @brief  定时器1初始化 —— 生成10微秒基础时基
 * @note   用于433无线解码的高精度计时
 * @retval 无
 */
void Timer1_10us_init(void)
{
    // 使能 TIM1 定时器时钟
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_TIM1);

    // 定义定时器初始化结构体，并清零
    LL_TIM_InitTypeDef TIM1CountInit = {0};

    // 时钟分频：不分频
    TIM1CountInit.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    // 计数模式：向上计数
    TIM1CountInit.CounterMode = LL_TIM_COUNTERMODE_UP;
    // 预分频器：8分频 (时钟 24MHz / 8 = 3MHz)
    TIM1CountInit.Prescaler = 8 - 1;
    // 自动重装载值：30 (3MHz / 30 = 100kHz → 10us中断一次)
    TIM1CountInit.Autoreload = 30 - 1;
    // 重复计数器：不用
    TIM1CountInit.RepetitionCounter = 0;

    // 初始化 TIM1 定时器
    LL_TIM_Init(TIM1, &TIM1CountInit);

    // 清除 TIM1 更新中断标志
    LL_TIM_ClearFlag_UPDATE(TIM1);

    // 使能 TIM1 更新中断
    LL_TIM_EnableIT_UPDATE(TIM1);

    // 启动 TIM1 计数器
    LL_TIM_EnableCounter(TIM1);

    // 使能 TIM1 中断通道
    NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);
    // 设置 TIM1 中断优先级为最高
    NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn, 0);
}

/**
 * @brief  定时器14初始化 —— 生成1毫秒基础时基
 * @note   用于按键扫描、LED闪烁、系统计时
 * @retval 无
 */
void Timer14_1ms_init(void)
{
    // 使能 TIM14 定时器时钟
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_TIM14);

    // 定义定时器初始化结构体，并清零
    LL_TIM_InitTypeDef TIM14CountInit = {0};

    // 时钟分频：不分频
    TIM14CountInit.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    // 计数模式：向上计数
    TIM14CountInit.CounterMode = LL_TIM_COUNTERMODE_UP;
    // 预分频器：8分频
    TIM14CountInit.Prescaler = 8 - 1;
    // 自动重装载值：3000 → 1ms中断一次
    TIM14CountInit.Autoreload = 3000 - 1;
    // 重复计数器：不用
    TIM14CountInit.RepetitionCounter = 0;

    // 初始化 TIM14 定时器
    LL_TIM_Init(TIM14, &TIM14CountInit);

    // 清除 TIM14 更新中断标志
    LL_TIM_ClearFlag_UPDATE(TIM14);

    // 使能 TIM14 更新中断
    LL_TIM_EnableIT_UPDATE(TIM14);

    // 启动 TIM14 计数器
    LL_TIM_EnableCounter(TIM14);

    // 使能 TIM14 中断通道
    NVIC_EnableIRQ(TIM14_IRQn);
    // 设置 TIM14 中断优先级
    NVIC_SetPriority(TIM14_IRQn, 0);
}

volatile unsigned char Flag_1ms = 0; // 1ms标志位
volatile unsigned int Count_1s = 0;  // 1秒计数器
volatile unsigned char Flag_1s = 0;  // 1秒标志位
/**
 * @brief  TIM14 中断服务函数
 * @note   每1ms进入一次 → 设置1ms标志
 * @retval 无
 */
void TIM14_IRQHandler(void)
{
    // 判断是否为更新中断
    if (LL_TIM_IsActiveFlag_UPDATE(TIM14) && LL_TIM_IsEnabledIT_UPDATE(TIM14))
    {
        LL_TIM_ClearFlag_UPDATE(TIM14); // 清除中断标志
        Flag_1ms = 1;                   // 设置1ms标志

        Count_1s++;           // 1秒计数器+1
        if (Count_1s >= 1000) // 计数满1000ms
        {
            Count_1s = 0; // 清空1秒计数器
            Flag_1s = 1;  // 设置1秒标志
        }
    }
}

/**
 * @brief  TIM1 中断服务函数
 * @note   每10us进入一次 → 处理433无线解码
 * @retval 无
 */
void TIM1_BRK_UP_TRG_COM_IRQHandler(void)
{
    // 判断是否为更新中断
    if (LL_TIM_IsActiveFlag_UPDATE(TIM1) && LL_TIM_IsEnabledIT_UPDATE(TIM1))
    {
        LL_TIM_ClearFlag_UPDATE(TIM1); // 清除中断标志
//        ev1527_proc();                 // 433无线解码处理
    }
}