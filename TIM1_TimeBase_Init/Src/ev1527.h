#ifndef __EV1527_H__
#define __EV1527_H__

#include "main.h"

enum EV1527_STATUS
{
    EV1527_WAIT,
    EV1527_DATA,
};

extern uint8_t Finish_Flag_ev1527;
extern volatile uint32_t Final_Data_ev1527;//按下遥控器可以读到值
void ev1527_proc();

#endif
