#ifndef __TIME_H__
#define __TIME_H__
// 包含主函数头文件（芯片定义、宏定义、全局变量）
#include "main.h"

extern uint8_t f_key1;
extern uint8_t f_key2;
extern volatile unsigned char Flag_1ms;  // 1ms标志位

extern volatile unsigned char Flag_1s;  // 1秒标志位

void Timer14_1ms_init(void);
void Timer1_10us_init(void);

#endif
