// 包含EV1527无线解码头文件
#include "ev1527.h"

// 433数据接收完成标志位（1表示接收到一帧完整数据）
uint8_t f_ev1527 = 0;

// 已接收的bit位数统计（EV1527协议共24bit）
uint8_t ev1527_time = 0;

// 接收完一帧数据后的延时（200ms），防止重复触发
uint8_t ev1527_time_count_down = 0;

// 脉冲宽度计数（用于判断高/低电平时长）
uint16_t ev1527_count = 0;

// EV1527解码状态机（等待同步码 / 接收数据）
uint8_t ev1527_status = EV1527_WAIT;

// 解码步骤（状态机子步骤）
uint8_t ev1527_step = 0;

// 最终接收到的24位有效数据（遥控器/烟感地址+按键值）
uint32_t ev1527_data = 0;

// 接收数据缓存（临时存储，拼接24bit）
uint32_t ev1527_data_buffer = 0;

/**
  * @brief  EV1527解码复位函数
  * @note   清空所有变量，回到初始等待状态
  * @retval 无
  */
void ev1527_exit(void)
{
    ev1527_count = 0;                  // 清空脉冲计数
    ev1527_status = EV1527_WAIT;       // 状态回到等待同步码
    ev1527_step = 0;                   // 清空步骤
    ev1527_data_buffer = 0;            // 清空数据缓存
    ev1527_time = 0;                   // 清空bit位计数
}

/**
  * @brief  EV1527协议无线解码处理函数
  * @note   每10us调用一次（由TIM1定时器中断调用）
  * @note   功能：解析433遥控器/烟感的24位无线数据
  * @retval 无
  */
void ev1527_proc(void)
{
    // 接收延时未结束，直接返回（防重复触发）
    if (ev1527_time_count_down)
    {
        ev1527_time_count_down--;
        return;
    }

    // 状态机：根据当前状态执行不同解码逻辑
    switch (ev1527_status)
    {
        // ===================== 状态1：等待同步码 =====================
        case EV1527_WAIT:
            // 步骤0：检测同步头 高电平（>200us）
            if (ev1527_step == 0)
            {
                // 读到高电平
                if (read_data())
                {
                    // 高电平持续时间计数
                    if (ev1527_count < 80)
                        ev1527_count++;
                }
                // 读到低电平
                else
                {
                    // 高电平持续时间满足条件
                    if (ev1527_count > 20)
                    {
                        ev1527_count = 0;        // 清空计数
                        ev1527_step++;           // 进入下一步
                    }
                    // 高电平太短，无效信号
                    else
                    {
                        ev1527_exit();           // 复位解码
                    }
                }
            }
            // 步骤1：检测同步头 低电平（7ms~13ms）
            else
            {
                // 读到低电平
                if (!read_data())
                {
                    // 低电平持续时间计数
                    if (ev1527_count < 1300)
                    {
                        ev1527_count++;
                    }
                    // 低电平太长，无效信号
                    else
                    {
                        ev1527_exit();
                    }
                }
                // 读到高电平
                else
                {
                    // 低电平持续时间满足同步码要求
                    if (ev1527_count > 700)
                    {
                        ev1527_count = 0;        // 清空计数
                        ev1527_step = 0;         // 步骤清零
                        ev1527_status = EV1527_DATA; // 切换到数据接收状态
                    }
                    // 低电平太短，无效信号
                    else
                    {
                        ev1527_exit();
                    }
                }
            }
            break;

        // ===================== 状态2：接收24位数据 =====================
        case EV1527_DATA:
            // 步骤0：检测数据位 起始高电平
            if (ev1527_step == 0)
            {
                // 读到高电平
                if (read_data())
                {
                    if (ev1527_count < 130)
                    {
                        ev1527_count++;
                    }
                    // 高电平过长，无效
                    else
                    {
                        ev1527_exit();
                    }
                }
                // 读到低电平
                else
                {
                    // 高电平宽度符合 数据1 的特征
                    if (ev1527_count > 60)
                    {
                        ev1527_count = 0;
                        ev1527_step = 1;   // 进入接收bit1流程
                    }
                    // 高电平宽度符合 数据0 的特征
                    else if (ev1527_count > 10)
                    {
                        ev1527_count = 0;
                        ev1527_step = 2;   // 进入接收bit0流程
                    }
                    // 无效电平
                    else
                    {
                        ev1527_exit();
                    }
                }
            }
            // 步骤1：接收数据 bit 1
            else if (ev1527_step == 1)
            {
                // 读到低电平
                if (!read_data())
                {
                    if (ev1527_count < 60)
                    {
                        ev1527_count++;
                    }
                    else
                    {
                        ev1527_exit();
                    }
                }
                // 读到高电平，bit1接收完成
                else
                {
                    if (ev1527_count > 10)
                    {
                        ev1527_count = 0;
                        ev1527_step = 0;

                        // 数据缓存左移一位，最低位写入 1
                        ev1527_data_buffer <<= 1;
                        ev1527_data_buffer |= 1;

                        ev1527_time++;  // 已接收bit数+1
                    }
                    else
                    {
                        ev1527_exit();
                    }

                    // 接收满24bit，一帧数据完成
                    if (ev1527_time >= 24)
                    {
                        ev1527_data = ev1527_data_buffer;        // 保存有效数据
                        ev1527_time_count_down = 200;            // 200ms内不再接收
                        f_ev1527 = 1;                            // 置位接收完成标志
                        ev1527_exit();                           // 复位解码
                    }
                }
            }
            // 步骤2：接收数据 bit 0
            else if (ev1527_step == 2)
            {
                // 读到低电平
                if (!read_data())
                {
                    if (ev1527_count < 130)
                    {
                        ev1527_count++;
                    }
                    else
                    {
                        ev1527_exit();
                    }
                }
                // 读到高电平，bit0接收完成
                else
                {
                    if (ev1527_count > 60)
                    {
                        ev1527_count = 0;
                        ev1527_step = 0;

                        // 数据缓存左移一位，最低位写入 0
                        ev1527_data_buffer <<= 1;

                        ev1527_time++;  // 已接收bit数+1
                    }
                    else
                    {
                        ev1527_exit();
                    }

                    // 接收满24bit，一帧数据完成
                    if (ev1527_time >= 24)
                    {
                        ev1527_data = ev1527_data_buffer;        // 保存有效数据
                        ev1527_time_count_down = 200;            // 200ms内不再接收
                        f_ev1527 = 1;                            // 置位接收完成标志
                        ev1527_exit();                           // 复位解码
                    }
                }
            }
            break;
    }
}