#ifndef __TIME_H__
#define __TIME_H__

#include <stdint.h>

extern uint8_t f_key1;
extern uint8_t f_key2;

void base_1ms_init(void);
void time_control(void);
void base_10us_init(void);

#endif
