#ifndef _APP_H__
#define _APP_H__
#include "main.h"


typedef enum Key_State
{
    KEY_IDLE = 0, // 按键空闲状态

    KEY_1_DEBOUNCE,       // 按键1消抖状态
    KEY_1_PRESSING,       // 按键1按下状态
    KEY_1_LOOSE,          // 按键1松开状态
    KEY_1_SINGLE_PRESSED, // 按键1单击状态
    KEY_1_DOUBLE_PRESSED, // 按键1双击状态
    KEY_1_LONG_PRESSED,   // 按键1长按状态

    KEY_2_DEBOUNCE,       // 按键2消抖状态
    KEY_2_PRESSING,       // 按键2按下状态
    KEY_2_LOOSE,          // 按键2松开状态
    KEY_2_SINGLE_PRESSED, // 按键2单击状态
    KEY_2_DOUBLE_PRESSED, // 按键2双击状态
    KEY_2_LONG_PRESSED    // 按键2长按状态
} Key_State;



extern volatile unsigned char Warning_Flag;
extern unsigned int Remoted_Device_Address[6];
extern unsigned int Detector_Device_Address[32];

extern volatile unsigned char Place_Flag;
extern volatile unsigned int Old_data_ev1527 ;

void APP_Get_Flash_Init_Data(void);

void APP_LED_ALL_OFF(void);

void APP_LED_ALL_ON(void);

void APP_Key_Scan(void);

void APP_Remote_control(void);

void APP_Control(void);



#endif