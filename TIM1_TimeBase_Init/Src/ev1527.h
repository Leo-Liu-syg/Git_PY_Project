#ifndef __EV1527_H__
#define __EV1527_H__

#include "main.h"

enum EV1527_STATUS
{
    EV1527_WAIT,
    EV1527_DATA,
};

extern uint8_t f_ev1527;
extern uint32_t ev1527_data;
void ev1527_proc();

#endif
