#ifndef __FLASH_H__
#define __FLASH_H__

#include <stdint.h>

#define FLASH_PAGE_CONTROL 0x08004000
#define FLASH_PAGE_REMOTE (FLASH_PAGE_CONTROL + 128)
#define FLASH_PAGE_DEVICES1 (FLASH_PAGE_REMOTE + 128)
#define FLASH_PAGE_DEVICES2 (FLASH_PAGE_DEVICES1 + 128)

#define REMOTE_NUM 6
#define DEVICE_NUM 60

extern uint32_t remote_cur;
extern uint32_t devices_cur;
extern uint32_t sos_time;
extern uint32_t sos_mode;
extern uint32_t speak_mode;
extern uint32_t sos_urgency;
extern uint32_t remote[6];  
extern uint32_t devices1[30];
extern uint32_t devices2[30];

void recovery();
void update_control();
void update_remote();
void update_devices();
void flash_init();

#endif
