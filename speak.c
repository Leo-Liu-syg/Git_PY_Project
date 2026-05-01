#include "speak.h"

// void delay100us()
// {
//     for (uint16_t i = 0; i < 300; i++)
//     {
//         __NOP();
//     }
// }

// 110us
void delay100us()
{
    for (uint16_t i = 0; i < 270; i++)
    {
        __NOP();
    }
}

void speak(uint8_t code)
{
    SPEAK_LOW();
    LL_mDelay(10);
    for (uint8_t i = 0; i < code; i++)
    {
        SPEAK_HIGH();
        delay100us();
        SPEAK_LOW();
        delay100us();
    }
}
