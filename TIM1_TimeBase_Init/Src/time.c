// 包含定时器相关头文件
#include "time.h"

// 包含主函数头文件（芯片定义、宏定义、全局变量）
#include "main.h"

/**
  * @brief  定时器1初始化 —— 生成10微秒基础时基
  * @note   用于433无线解码的高精度计时
  * @retval 无
  */
void base_10us_init(void)
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
void base_1ms_init(void)
{
    // 使能 TIM14 定时器时钟
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_TIM14);

    // 定义定时器初始化结构体，并清零
    LL_TIM_InitTypeDef TIM4CountInit = {0};

    // 时钟分频：不分频
    TIM4CountInit.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    // 计数模式：向上计数
    TIM4CountInit.CounterMode = LL_TIM_COUNTERMODE_UP;
    // 预分频器：8分频
    TIM4CountInit.Prescaler = 8 - 1;
    // 自动重装载值：3000 → 1ms中断一次
    TIM4CountInit.Autoreload = 3000 - 1;
    // 重复计数器：不用
    TIM4CountInit.RepetitionCounter = 0;

    // 初始化 TIM14 定时器
    LL_TIM_Init(TIM14, &TIM4CountInit);

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

// ===================== 全局变量定义 =====================
uint8_t F_1ms = 0;                  // 1毫秒标志位（每1ms置1）
uint16_t count_1s = 0;              // 秒级计时计数器
uint16_t key1_down_count = 0;       // 按键1按下时长计数
uint16_t key1_up_count = 0;         // 按键1松开时长计数
uint8_t f_key1 = 0;                 // 按键1有效动作标志
uint8_t key1_press_count = 0;       // 按键1按下次数统计
uint8_t key2_press_count = 0;       // 按键2按下次数统计
uint16_t key2_up_count = 0;         // 按键2松开时长计数
uint16_t key2_down_count = 0;       // 按键2按下时长计数
uint8_t f_key2 = 0;                 // 按键2有效动作标志
uint16_t led_flash_count = 0;       // LED闪烁计时（单位ms）

/**
  * @brief  按键处理函数（单击/双击/长按3秒/三连击）
  * @note   K1：学习/恢复出厂；K2：模式切换/消警/切换音效
  * @retval 无
  */
void key_control(void)
{
    // 报警状态下，屏蔽大部分按键功能
    if (!f_warning)
    {
        // 按键1 被按下
        if (read_key1())
        {
            // 按下消抖有效
            if ((key1_down_count > 20) && (key1_down_count != 3000))
            {
                key1_press_count += 1;  // 记录按下次数
            }
            // 松开超时判断
            if (key1_up_count < 500)
            {
                if (++key1_up_count == 500)
                {
                    if (key1_press_count >= 2)
                    {
                        f_key2 = 1;      // 双击K1
                    }
                    else if (key1_press_count == 1)
                    {
                        f_key1 = 1;      // 单击K1
                    }
                    key1_press_count = 0;
                }
            }
            key1_down_count = 0;
        }
        // 按键1 未按下
        else
        {
            if (key1_down_count < 3000)
            {
                key1_down_count++;
                // 按下达到3秒 → 恢复出厂设置
                if (key1_down_count == 3000)
                {
                    key1_press_count = 0;
                    recovery();                  // 恢复出厂函数
                    speak(SPEAK_RECOVERY);       // 播放“恢复出厂”语音
                }
            }
            key1_up_count = 0;
        }
    }

    // 按键2 被按下
    if (read_key2())
    {
        if ((key2_down_count > 20) && (key2_down_count != 3000))
        {
            key2_press_count += 1;
        }
        if (key2_up_count < 500)
        {
            if (++key2_up_count == 500)
            {
                // 三连击 K2 → 切换报警语音
                if (key2_press_count >= 3)
                {
                    if (!f_warning)
                    {
                        // 语音模式循环切换 0→1→2→3→0
                        speak_mode = speak_mode >= 3 ? 0 : speak_mode + 1;

                        if (speak_mode == 0)
                        {
                            speak(SPEAK_FIRE_WARNING);
                            sos_speak_count_down = 0;
                        }
                        else if (speak_mode == 1)
                        {
                            speak(SPEAK_URGENCY_WARNING);
                            sos_speak_count_down = 0;
                        }
                        else if (speak_mode == 2)
                        {
                            speak(SPEAK_SECURE_WARNING);
                            sos_speak_count_down = 0;
                        }
                        else if (speak_mode == 3)
                        {
                            speak(SPEAK_WARNING);
                            sos_speak_count_down = 5;
                        }
                        sos_speak_resend = 0;
                        update_control();
                    }
                }
                // 单击 K2 → 停止报警
                else if (key2_press_count == 1)
                {
                    if (sos_speak_count_down)
                    {
                        sos_speak_count_down = 0;
                        f_warning = 0;
                        speak(SPEAK_STOP);
                        LED1_CLOSE();
                        LED2_CLOSE();
                        LED3_CLOSE();
                        LED4_CLOSE();
                        LED5_CLOSE();
                    }
                }
                key2_press_count = 0;
            }
        }
        key2_down_count = 0;
    }
    // 按键2 未按下
    else
    {
        if (key2_down_count < 3000)
        {
            key2_down_count++;
            // 长按3秒 → 切换正常/紧急模式
            if (key2_down_count == 3000)
            {
                key2_press_count = 0;
                if (!f_warning)
                {
                    if (sos_urgency == 0)
                    {
                        sos_urgency = 1;
                        speak(SPEAK_NORMAL);
                    }
                    else
                    {
                        sos_urgency = 0;
                        speak(SPEAK_URGENCY);
                    }
                    update_control();
                }
            }
        }
        key2_up_count = 0;
    }
}

/**
  * @brief  系统时间调度函数（每1ms执行一次）
  * @note   调用按键、应用、报警、LED、计时逻辑
  * @retval 无
  */
void time_control(void)
{
    // 没有1ms标志，直接退出
    if (!F_1ms)
        return;

    F_1ms = 0;  // 清除1ms标志

    key_control();    // 调用按键处理
    app_control();    // 调用应用状态机处理

    // 遥控器SOS长按倒计时
    if (sos_key_count_down)
    {
        sos_key_count_down--;
        if (sos_key_count_down == 0)
        {
            sos_count = 0;
        }
    }

    // 模式切换防抖动计时
    if (sos_mode_count_down)
        sos_mode_count_down--;

    // 学习模式超时计时
    if (study_count_down)
        study_count_down--;

    // 报警状态 → LED闪烁
    if (f_warning)
    {
        if (++led_flash_count >= 300)
        {
            led_flash_count = 0;
            LED1_TOG();
            LED2_TOG();
            LED3_TOG();
            LED4_TOG();
            LED5_TOG();
        }
    }

    // 秒级任务（每1秒执行一次）
    if (++count_1s >= 1000)
    {
        count_1s = 0;

        // 报警语音倒计时
        if (sos_speak_count_down)
        {
            sos_speak_count_down--;

            // 循环播放对应报警语音
            if (speak_mode == 0)
            {
                if (++sos_speak_resend >= 21)
                {
                    sos_speak_resend = 0;
                    speak(SPEAK_FIRE_WARNING);
                }
            }
            else if (speak_mode == 1)
            {
                if (++sos_speak_resend >= 18)
                {
                    sos_speak_resend = 0;
                    speak(SPEAK_URGENCY_WARNING);
                }
            }
            else if (speak_mode == 2)
            {
                if (++sos_speak_resend >= 21)
                {
                    sos_speak_resend = 0;
                    speak(SPEAK_SECURE_WARNING);
                }
            }

            // 无限报警模式
            if ((sos_time == 4) && (f_warning))
            {
                sos_speak_count_down = 65535;
            }

            // 报警时间到 → 停止报警
            if (sos_speak_count_down == 0)
            {
                speak(SPEAK_STOP);
                f_warning = 0;
                LED1_CLOSE();
                LED2_CLOSE();
                LED3_CLOSE();
                LED4_CLOSE();
                LED5_CLOSE();
            }
        }
    }
}

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
        LL_TIM_ClearFlag_UPDATE(TIM14);  // 清除中断标志
        F_1ms = 1;                       // 设置1ms标志
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
        LL_TIM_ClearFlag_UPDATE(TIM1);  // 清除中断标志
        ev1527_proc();                 // 433无线解码处理
    }
}