#ifndef __APP_H__
#define __APP_H__

#include <stdint.h>

#define REMOTE_ADDR_MASK 0xFFFFF0
#define REMOTE_DATA_MASK 0x00000F

enum DEVICE_STATUS
{
    APP_IDLE,
    APP_STUDY_REMOTE,
    APP_STUDY_DETECTOR,
};

extern uint8_t f_warning;
extern uint8_t sos_count;
extern uint16_t sos_key_count_down;
extern uint16_t sos_speak_count_down;
extern uint16_t sos_mode_count_down;
extern uint16_t study_count_down;
extern uint16_t sos_speak_resend;

void app_control();

#endif
