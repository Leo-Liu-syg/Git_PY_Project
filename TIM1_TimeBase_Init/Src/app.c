#include "app.h"
#include "main.h"

uint32_t last_devices = 0;
uint32_t count_down_30s = 0;
uint8_t app_statue = APP_IDLE;
uint8_t app_step = 0;
uint8_t f_warning = 0;

uint8_t sos_count = 0;
uint16_t sos_mode_count_down = 0;  // ms
uint16_t sos_key_count_down = 0;   // ms
uint16_t study_count_down = 0;     // ms
uint16_t sos_speak_count_down = 0; // s
uint16_t sos_speak_resend = 0; // s

void switch_app_status(uint8_t new_status)
{
    app_statue = new_status;
    f_key1 = 0;
    f_key2 = 0;
    app_step = 0;
    count_down_30s = 0;
}

void study_cancel()
{
    switch_app_status(APP_IDLE);
    speak(SPEAK_CANCEL);
    for (uint8_t i = 0; i < 5; i++)
    {
        LED1_OPEN();
        LED2_OPEN();
        LED3_OPEN();
        LED4_OPEN();
        LED5_OPEN();
        LL_mDelay(100);
        LED1_CLOSE();
        LED2_CLOSE();
        LED3_CLOSE();
        LED4_CLOSE();
        LED5_CLOSE();
        LL_mDelay(100);
    }
}


void sos_speak()
{
    f_warning = 1;
    if (sos_speak_count_down && (last_devices == ev1527_data))
    {
        return;
    }
    sos_speak_resend = 0;
    last_devices = ev1527_data;
    // speak(SPEAK_WARNING);
    if (speak_mode == 0)
    {
        speak(SPEAK_FIRE_WARNING);
    }
    else if (speak_mode == 1)
    {
        speak(SPEAK_URGENCY_WARNING);
    }
    else if (speak_mode == 2)
    {
        speak(SPEAK_SECURE_WARNING);
    }
    else if (speak_mode == 3)
    {
        speak(SPEAK_WARNING);
    }
    switch (sos_time)
    {
    case 0:
        sos_speak_count_down = 60;
        break;
    case 1:
        sos_speak_count_down = 5 * 60;
        break;
    case 2:
        sos_speak_count_down = 15 * 60;
        break;
    case 3:
        sos_speak_count_down = 30 * 60;
        break;
    case 4:
        sos_speak_count_down = 65535;
        break;
    }
}

void change_sos_time()
{
    sos_time = sos_time >= 4 ? 0 : sos_time + 1;
    update_control();
    switch (sos_time)
    {
    case 0:
        LED1_OPEN();
        break;
    case 1:
        LED1_OPEN();
        LED2_OPEN();
        break;
    case 2:
        LED1_OPEN();
        LED2_OPEN();
        LED3_OPEN();
        break;
    case 3:
        LED1_OPEN();
        LED2_OPEN();
        LED3_OPEN();
        LED4_OPEN();
        break;
    case 4:
        LED1_OPEN();
        LED2_OPEN();
        LED3_OPEN();
        LED4_OPEN();
        LED5_OPEN();
        break;
    }
    LL_mDelay(1000);
    LED1_CLOSE();
    LED2_CLOSE();
    LED3_CLOSE();
    LED4_CLOSE();
    LED5_CLOSE();
}

uint8_t remote_check()
{
    uint32_t data_buffer = ev1527_data & REMOTE_ADDR_MASK;
    for (uint8_t i = 0; i < 6; i++)
    {
        if (data_buffer == remote[i])
        {
            return 1;
        }
    }
    return 0;
}

void save_remote()
{
    if (remote_check())
    {
        if (study_count_down == 0)
        {
            study_count_down = 1000;
            speak(SPEAK_STUDY_SUCCESS);
        }
        return;
    }
    speak(SPEAK_STUDY_SUCCESS);
    remote[remote_cur] = ev1527_data & REMOTE_ADDR_MASK;
    remote_cur = remote_cur >= REMOTE_NUM ? 0 : remote_cur + 1;
    update_control();
    update_remote();
}

void remote_work()
{
    if (!remote_check())
        return;

    switch (ev1527_data & REMOTE_DATA_MASK)
    {
    // 布防
    case 2 :
        if(f_warning && (sos_urgency == 0))
        {
            return;
        }
        if ((sos_mode != 1) || (sos_mode_count_down == 0))
        {
            sos_mode = 1;
            sos_mode_count_down = 3000;
            speak(SPEAK_PLACE);
            f_warning = 0;
            sos_speak_count_down = 1;
            LED1_CLOSE();
            LED2_CLOSE();
            LED3_CLOSE();
            LED4_CLOSE();
            LED5_CLOSE();
            LL_mDelay(100);
            LED1_OPEN();
            LED2_OPEN();
            LED3_OPEN();
            LED4_OPEN();
            LED5_OPEN();
            LL_mDelay(100);
            LED1_CLOSE();
            LED2_CLOSE();
            LED3_CLOSE();
            LED4_CLOSE();
            LED5_CLOSE();
            update_control();
        }
        break;
    // 撤防
    case 8:
        if ((sos_mode != 0) || (sos_mode_count_down == 0))
        {
            sos_mode = 0;
            sos_mode_count_down = 3000;
            speak(SPEAK_UNPLACE);
            f_warning = 0;
            sos_speak_count_down = 1;
            LED1_CLOSE();
            LED2_CLOSE();
            LED3_CLOSE();
            LED4_CLOSE();
            LED5_CLOSE();
            LL_mDelay(100);
            LED1_OPEN();
            LED2_OPEN();
            LED3_OPEN();
            LED4_OPEN();
            LED5_OPEN();
            LL_mDelay(100);
            LED1_CLOSE();
            LED2_CLOSE();
            LED3_CLOSE();
            LED4_CLOSE();
            LED5_CLOSE();
            update_control();
        }
        break;
    // 设置报警时间
    case 4:
        change_sos_time();
        break;
    // 长按报警
    case 1:
        if (++sos_count >= 10)
        {
            sos_count = 0;
            sos_speak();
        }
        sos_key_count_down = 1000;
        break;
    }
}

uint8_t devices_check()
{
    for (uint8_t i = 0; i < 30; i++)
    {
        if (ev1527_data == devices1[i])
        {
            return 1;
        }
    }
    for (uint8_t i = 0; i < 30; i++)
    {
        if (ev1527_data == devices2[i])
        {
            return 1;
        }
    }
    return 0;
}

void save_devices()
{
    if (devices_check())
    {
        if (study_count_down == 0)
        {
            study_count_down = 1000;
            speak(SPEAK_STUDY_SUCCESS);
        }
        return;
    }

    speak(SPEAK_STUDY_SUCCESS);
    if (devices_cur < 30)
    {
        devices1[devices_cur] = ev1527_data;
    }
    else
    {
        devices2[devices_cur - 30] = ev1527_data;
    }
    update_devices();
    devices_cur = devices_cur >= DEVICE_NUM ? 0 : devices_cur + 1;
    update_control();
}

void devices_work()
{
    // 正常模式并且撤防时不报警
    if ((sos_urgency == 1) && (sos_mode == 0))
    {
        return;
    }
    if (devices_check())
    {
        sos_speak();
    }
}

void app_control()
{
    switch (app_statue)
    {
    case APP_IDLE:
        if (f_ev1527)
        {
            remote_work();
            devices_work();
            f_ev1527 = 0;
        }

        if(f_warning)
        {
            f_key1 = 0;
            f_key2 = 0;
        }
        else
        {
            if (f_key1)
            {
                f_key1 = 0;
                switch_app_status(APP_STUDY_REMOTE);
                speak(SPEAK_STUDY_REMOTE);
                LED1_OPEN();
                LED2_OPEN();
                LED3_OPEN();
                LED4_OPEN();
                LED5_OPEN();
                count_down_30s = 30 * 1000;
            }
    
            if (f_key2)
            {
                f_key2 = 0;
                LED1_OPEN();
                switch_app_status(APP_STUDY_DETECTOR);
                speak(SPEAK_STUDY_DETECTOR);
                count_down_30s = 30 * 1000;
            }
        }
        break;

    case APP_STUDY_REMOTE:
        if (count_down_30s)
        {
            count_down_30s--;
            if (count_down_30s == 0)
            {
                study_cancel();
            }
        }

        if (f_key1)
        {
            f_key1 = 0;
            study_cancel();
        }

        if (f_ev1527)
        {
            if(remote_cur < REMOTE_NUM)
            {
                save_remote();
                count_down_30s = 10 * 1000;
            }
            else
            {
                switch_app_status(APP_IDLE);
                speak(SPEAK_CANCEL);
                for (uint8_t i = 0; i < 15; i++)
                {
                    LED1_OPEN();
                    LED2_OPEN();
                    LED3_OPEN();
                    LED4_OPEN();
                    LED5_OPEN();
                    LL_mDelay(100);
                    LED1_CLOSE();
                    LED2_CLOSE();
                    LED3_CLOSE();
                    LED4_CLOSE();
                    LED5_CLOSE();
                    LL_mDelay(100);
                }
            }
            f_ev1527 = 0;
        }
        break;
    case APP_STUDY_DETECTOR:
        if (count_down_30s)
        {
            count_down_30s--;
            if (count_down_30s == 0)
            {
                study_cancel();
            }
        }

        if (f_key2)
        {
            f_key2 = 0;
            study_cancel();
        }

        if (f_ev1527)
        {
            if(devices_cur < DEVICE_NUM)
            {
                save_devices();
                count_down_30s = 10 * 1000;
            }
            else
            {
                switch_app_status(APP_IDLE);
                speak(SPEAK_CANCEL);
                for (uint8_t i = 0; i < 15; i++)
                {
                    LED1_OPEN();
                    LED2_OPEN();
                    LED3_OPEN();
                    LED4_OPEN();
                    LED5_OPEN();
                    LL_mDelay(100);
                    LED1_CLOSE();
                    LED2_CLOSE();
                    LED3_CLOSE();
                    LED4_CLOSE();
                    LED5_CLOSE();
                    LL_mDelay(100);
                }
            }
            f_ev1527 = 0;
        }
        break;
    }
}
