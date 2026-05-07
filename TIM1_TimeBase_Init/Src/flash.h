#ifndef _FLASH_H
#define _FLASH_H

#include "main.h"

// 用户Flash操作起始地址
#define FLASH_USER_START_ADDR     0x08004000

void Flash_update_process(void);
void APP_ErrorHandler(void);

uint32_t flash_read_word(uint32_t addr);
void flash_read_buf(uint32_t addr, uint32_t *buf, uint16_t len);

extern uint32_t DATA[64];

#endif