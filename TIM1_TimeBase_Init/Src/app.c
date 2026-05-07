#include "app.h"

unsigned int Wait_count = 0;
unsigned int Hold_count = 0;
unsigned int Loose_count = 0;
unsigned int Sync_count = 0;
unsigned int Remote_count = 0;
volatile unsigned int OK_2_warning_count = 0;

volatile unsigned char Warning_Flag = 1;
volatile unsigned char Flash_update_flag = 0; // 1表示需要更新Flash，0表示不需要
volatile unsigned char Place_Flag = 0;        // 1表示布防，0表示撤防
volatile unsigned char Remoted_Sync_Flag = 0; // 表示确认是配对的遥控器
volatile unsigned char Sync_Flag = 0;
volatile unsigned char OK_2_warning_Flag = 1;

Key_State key_state = KEY_IDLE;
Speak WARNING_MODE = SPEAK_WARNING;

volatile unsigned int Old_data_ev1527 = 0;
volatile unsigned char Ev1527_Last_Bit = 0;

volatile unsigned char APP_MODE = 0; // 0是正常，1是紧急

unsigned int Remoted_Device_Address[6] = {0};
unsigned int Detector_Device_Address[32] = {0};

void APP_Get_Flash_Init_Data(void)
{
    for (unsigned char i = 0; i < 6; i++)
    {
        Remoted_Device_Address[i] = DATA[i]; // 从Flash数据数组读取遥控器地址数据
    }
    for (unsigned char i = 0; i < 32; i++)
    {
        Detector_Device_Address[i] = DATA[i + 6]; // 从Flash数据数组读取探测器地址数据
    }
}

void APP_LED_ALL_OFF(void)
{
    LED1_CLOSE();
    LED2_CLOSE();
    LED3_CLOSE();
    LED4_CLOSE();
    LED5_CLOSE();
}

void APP_LED_ALL_ON(void)
{
    LED1_OPEN();
    LED2_OPEN();
    LED3_OPEN();
    LED4_OPEN();
    LED5_OPEN();
}

void APP_Key_Scan(void)
{
    if (Wait_count)
    {
        Wait_count--;
        return;
    }

    switch (key_state)
    {
    case KEY_IDLE:
        APP_LED_ALL_OFF();
        if (read_key1() == 0) // 按键1被按下（假设按键为低电平有效）
        {
            Wait_count = 20;
            key_state = KEY_1_DEBOUNCE; // 进入按键1消抖状态
        }
        else if (read_key2() == 0) // 按键2被按下
        {
            Wait_count = 20;

            key_state = KEY_2_DEBOUNCE; // 进入按键2消抖状态
        }
        break;
    case KEY_1_DEBOUNCE:
        if (read_key1() == 0)
        {
            key_state = KEY_1_PRESSING; // 进入按键1按下状态
        }
        else if (read_key1() == 1)
        {
            key_state = KEY_IDLE; // 按键1松开，回到空闲状态
        }
        break;
    case KEY_1_PRESSING:
        if (read_key1() == 0) // 按键1仍然被按下
        {
            Hold_count++;
            if (Hold_count > 3000) // 持续按下超过3秒
            {

                key_state = KEY_1_LONG_PRESSED; // 进入按键1长按状态
                Hold_count = 0;
                Wait_count = 300;
            }
        }
        else
        {
            key_state = KEY_1_LOOSE; // 按键1松开，进入松开状态
            Wait_count = 20;         // 设置等待时间消抖
        }
        break;
    case KEY_1_LOOSE:
        Loose_count++;
        if (read_key1() == 1 && Loose_count > 800) // 松开状态持续超过0.8秒
        {
            key_state = KEY_1_SINGLE_PRESSED; // 进入按键1单击状态
            Loose_count = 0;
            Sync_Flag = 1;    // 置位配对标志，开始配对流程
            Wait_count = 300; // 设置等待时间，防止连续触发
        }
        else if (read_key1() == 0 && Loose_count < 800) // 在松开状态期间按键1再次被按下
        {

            key_state = KEY_1_DOUBLE_PRESSED; // 进入按键1双击状态
            Loose_count = 0;
            Sync_Flag = 1; // 置位配对标志，开始配对流程
            Wait_count = 300;
        }
        break;
    case KEY_1_SINGLE_PRESSED:
        // 处理按键1单击事件
        if (Sync_Flag)
        {
            APP_LED_ALL_ON();
            SPEAKING(SPEAK_STUDY_REMOTE);
            Sync_Flag = 0; // 清除配对标志，进入配对流程
        }

        Sync_count++;
        if (Final_Data_ev1527 != Old_data_ev1527 && Final_Data_ev1527 != 0) // 如果接收到了新的EV1527数据
        {
            Sync_count = 0; // 重置配对计数，延长配对时间窗口
            Old_data_ev1527 = Final_Data_ev1527;
            // Final_Data_ev1527 = 0; // 清除接收的数据
            // 存入flash
            for (unsigned char i = 0; i < 6; i++)
            {
                if (Remoted_Device_Address[i] == Old_data_ev1527 / 16) // 去掉个位
                {
                    break; // 不需要再存储了，退出循环
                }
                else
                {
                    if (Remoted_Device_Address[i] == 0xFFFFFFFF)
                    {
                        Remoted_Device_Address[i] = Old_data_ev1527 / 16; // 存储新设备数据
                        SPEAKING(SPEAK_STUDY_SUCCESS);
                        Flash_update_flag = 1; // 设置Flash更新标志
                        break;
                    }
                }
            }
        }

        if (Sync_count > 5000) // 5秒配对时间
        {
            Sync_count = 0;
            key_state = KEY_IDLE; // 处理完成后回到空闲状态
            Wait_count = 2000;    // 设置等待时间，防止连续触发
        }

        break;
    case KEY_1_DOUBLE_PRESSED:
        if (Sync_Flag)
        {
            LED3_OPEN();
            SPEAKING(SPEAK_STUDY_DETECTOR);
            Sync_Flag = 0; // 清除配对标志，进入配对流程
        }
        Sync_count++;
        if (Final_Data_ev1527 != Old_data_ev1527 && Final_Data_ev1527 != 0) // 如果接收到了新的EV1527数据
        {
            Sync_count = 0; // 重置配对计数，延长配对时间窗口
            Old_data_ev1527 = Final_Data_ev1527;
            // Final_Data_ev1527 = 0; // 清除接收的数据，准备接收下一帧数据
            // 存入flash
            for (unsigned char i = 0; i < 32; i++)
            {
                if (Detector_Device_Address[i] == Old_data_ev1527 / 16) // 去掉个位
                {
                    break; // 不需要再存储了，退出循环
                }
                else
                {
                    if (Detector_Device_Address[i] == 0xFFFFFFFF)
                    {
                        Detector_Device_Address[i] = Old_data_ev1527 / 16; // 存储新设备数据
                        SPEAKING(SPEAK_STUDY_SUCCESS);
                        Flash_update_flag = 1; // 设置Flash更新标志
                        break;
                    }
                }
            }
        }

        Wait_count = 2000;
        key_state = KEY_IDLE; // 处理完成后回到空闲状态
        // 处理按键1双击事件
        break;
    case KEY_1_LONG_PRESSED:
        SPEAKING(SPEAK_RECOVERY);
        for (unsigned char i = 0; i < 32; i++)
        {
            Detector_Device_Address[i] = 0xFFFFFFFF; // 清除存储的设备数据
        }
        for (unsigned char i = 0; i < 6; i++)
        {
            Remoted_Device_Address[i] = 0xFFFFFFFF; // 清除存储的设备数据
        }
        for (unsigned char i = 0; i < 64; i++)
        {
            DATA[i] = 0xFFFFFFFF; // 清除存储的设备数据
        }
        Flash_update_flag = 1; // 设置Flash更新标志，清除存储的设备数据
        Wait_count = 2000;
        key_state = KEY_IDLE; // 处理完成后回到空闲状态
        break;

    case KEY_2_DEBOUNCE:
        if (read_key2() == 0)
        {
            key_state = KEY_2_PRESSING; // 进入按键2按下状态
        }
        else if (read_key2() == 1)
        {
            key_state = KEY_IDLE; // 按键2松开，回到空闲状态
        }
        break;
    case KEY_2_PRESSING:
        if (read_key2() == 0) // 按键2仍然被按下
        {
            Hold_count++;
            if (Hold_count > 3000) // 持续按下超过3秒
            {
                key_state = KEY_2_LONG_PRESSED; // 进入按键2长按状态
                Hold_count = 0;
                Wait_count = 300;
            }
        }
        else
        {
            key_state = KEY_2_LOOSE; // 按键2松开，进入松开状态
            Wait_count = 20;         // 设置等待时间消抖
        }
        break;
    case KEY_2_LOOSE:
        Loose_count++;
        if (read_key2() == 1 && Loose_count > 800) // 松开状态持续超过0.8秒
        {
            key_state = KEY_2_SINGLE_PRESSED; // 进入按键2单击状态
            Loose_count = 0;
            Wait_count = 300; // 设置等待时间，防止连续触发
        }
        else if (read_key2() == 0 && Loose_count < 800) // 在松开状态期间按键2再次被按下
        {
            key_state = KEY_2_DOUBLE_PRESSED; // 进入按键2双击状态
            Loose_count = 0;
            Wait_count = 300; // 设置等待时间，防止连续触发
        }
        break;
    case KEY_2_SINGLE_PRESSED:

        Warning_Flag = 0;
        Wait_count = 1000;    // 设置等待时间，防止连续触发
        key_state = KEY_IDLE; // 处理完成后回到空闲状态
        break;
    case KEY_2_DOUBLE_PRESSED:

        if (WARNING_MODE < SPEAK_URGENCY_WARNING)
        {
            SPEAKING(++WARNING_MODE);
        }
        else
        {
            WARNING_MODE = SPEAK_WARNING;
            SPEAKING(WARNING_MODE);
        }
        Wait_count = 3000;
        key_state = KEY_IDLE; // 处理完成后回到空闲状态
        // 处理按键2双击事件
        break;
    case KEY_2_LONG_PRESSED:
        if (APP_MODE)
        {
            APP_MODE = 0;
            Warning_Flag = 1; // 重启警铃
            SPEAKING(SPEAK_NORMAL);
        }
        else
        {
            APP_MODE = 1;
            Warning_Flag = 1; // 重启警铃
            SPEAKING(SPEAK_URGENCY);
        }

        Wait_count = 2000;
        key_state = KEY_IDLE; // 处理完成后回到空闲状态
        break;

    default:
        break;
    }
}

void APP_Remote_control(void) // 放在1ms flag里
{
    static unsigned int Remoted_Data_Buffer = 0;
    static unsigned char Remoted_Warning_Mode = 0; // 0是5s，1是10s，2是20s，3是一直响
    if (OK_2_warning_count)
    {
        OK_2_warning_count--;
    }
    if (Remote_count)
    {
        Remote_count--;
        return;
    }

    for (unsigned char i = 0; i < 6; i++)
    {
        if (Remoted_Device_Address[i] == Final_Data_ev1527 / 16)
        {
            Remote_count = 3000;
            Remoted_Data_Buffer = Final_Data_ev1527; // 转移数据
            Final_Data_ev1527 = 0;                   // 清除接收的数据，准备接收下一帧数据
            Remoted_Sync_Flag = 1;                   // 设置同步标志
            break;
        }
    }

    if (Remoted_Sync_Flag)
    {
        Remoted_Sync_Flag = 0;
        Ev1527_Last_Bit = (uint8_t)(Remoted_Data_Buffer % 16);
        switch (Ev1527_Last_Bit)
        {
        case 1:               // SOS报警=紧急模式没用，正常状态布防
            APP_MODE=0; // 切换到正常模式
            Warning_Flag = 1; // 启动警铃
            Place_Flag = 1; //布防
            OK_2_warning_Flag = 1; // 警铃一直响，显得比较SOS
            break;
        case 2:               // 布防
            
            Place_Flag = 1; //布防
            SPEAKING(SPEAK_PLACE);
            Ev1527_Last_Bit = 0;
            break;
        case 4: // 设置报警时长,switch 嵌套
            if (Remoted_Warning_Mode < 5)
            {
                Remoted_Warning_Mode++;
            }
            else
            {
                Remoted_Warning_Mode = 0;
            }
            switch (Remoted_Warning_Mode)
            {
            case 0:
                OK_2_warning_count = 5000;
                OK_2_warning_Flag = 0;
                break;
            case 1:
                OK_2_warning_count = 10000;
                OK_2_warning_Flag = 0;
                break;
            case 2:
                OK_2_warning_count = 15000;
                OK_2_warning_Flag = 0;
                break;
            case 3:
                OK_2_warning_count = 20000;
                OK_2_warning_Flag = 0;
                break;
            case 4:
                OK_2_warning_count = 0;
                OK_2_warning_Flag = 1;
                break;
            default:
                break;
            }
            break;
        case 8: // 撤防
            SPEAKING(SPEAK_UNPLACE);
            Place_Flag = 0;
            Ev1527_Last_Bit = 0;
            break;
        }
    }
}

void APP_Control(void)
{
    if (Flag_1ms) // 在这里添加1ms周期内需要执行的代码
    {
        Flag_1ms = 0;

        APP_Key_Scan();
        APP_Remote_control();
    }
    if (Flag_1s) // 在这里添加1s周期内需要执行的代码
    {
        Flag_1s = 0;

        // if (rx_data != 0) // 如果接收到了数据
        // {
        //     USart1_Send_ASCII_Back(rx_data); // 回显接收到的数据
        //     rx_data = 0;                     // 这里可以添加其他1s周期内需要执行的操作
        // }

        // if (Finish_Flag_ev1527) // 如果接收完成了EV1527数据
        // {
        //     Finish_Flag_ev1527 = 0;                                      // 清除完成标志，准备接收下一帧数据
        //     USart1_Send_ASCII_Back((uint8_t)(Final_Data_ev1527 & 0xFF)); // 回显EV1527数据的最低8位
        //     // 这里可以添加处理Final_Data_ev1527的代码，例如打印或控制LED等
        // }

        if (Flash_update_flag)
        {
            Flash_update_flag = 0;
            for (unsigned char i = 0; i < 6; i++)
            {
                DATA[i] = Remoted_Device_Address[i]; // 将遥控器地址数据写入待更新的Flash数据数组
            }
            for (unsigned char i = 0; i < 32; i++)
            {
                DATA[i + 6] = Detector_Device_Address[i]; // 将探测器地址数据写入Flash数组
            }
            Flash_update_process();
        }
    }
    if (Flag_3s) // 在这里添加3s周期内需要执行的代码
    {
        Flag_3s = 0;
        if (Warning_Flag && (OK_2_warning_count != 0 || OK_2_warning_Flag == 1)) // 警铃执行条件,要么有时间限制要么一直响
        {
            if (APP_MODE) // 紧急模式
            {
                SPEAKING(WARNING_MODE);
            }
            else // 正常模式
            {
                if (Place_Flag) // 布防状态
                {
                    SPEAKING(WARNING_MODE);
                }
                else // 撤防状态
                {
                }
            }
        }
    }
}