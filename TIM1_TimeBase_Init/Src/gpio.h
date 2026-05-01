#ifndef __GPIO_H__
#define __GPIO_H__

#include <stdint.h>


// LED 1
#define GPIO_LED1 GPIOA
#define PIN_LED1 LL_GPIO_PIN_4
#define LED1_CLOSE() LL_GPIO_SetOutputPin(GPIO_LED1, PIN_LED1)
#define LED1_OPEN() LL_GPIO_ResetOutputPin(GPIO_LED1, PIN_LED1)
#define LED1_TOG() LL_GPIO_TogglePin(GPIO_LED1, PIN_LED1)
// LED 2
#define GPIO_LED2 GPIOA
#define PIN_LED2 LL_GPIO_PIN_3
#define LED2_CLOSE() LL_GPIO_SetOutputPin(GPIO_LED2, PIN_LED2)
#define LED2_OPEN() LL_GPIO_ResetOutputPin(GPIO_LED2, PIN_LED2)
#define LED2_TOG() LL_GPIO_TogglePin(GPIO_LED2, PIN_LED2)
// LED 3
#define GPIO_LED3 GPIOA
#define PIN_LED3 LL_GPIO_PIN_1
#define LED3_CLOSE() LL_GPIO_SetOutputPin(GPIO_LED3, PIN_LED3)
#define LED3_OPEN() LL_GPIO_ResetOutputPin(GPIO_LED3, PIN_LED3)
#define LED3_TOG() LL_GPIO_TogglePin(GPIO_LED3, PIN_LED3)
// LED 4
#define GPIO_LED4 GPIOA
#define PIN_LED4 LL_GPIO_PIN_0
#define LED4_CLOSE() LL_GPIO_SetOutputPin(GPIO_LED4, PIN_LED4)
#define LED4_OPEN() LL_GPIO_ResetOutputPin(GPIO_LED4, PIN_LED4)
#define LED4_TOG() LL_GPIO_TogglePin(GPIO_LED4, PIN_LED4)
// LED 5
#define GPIO_LED5 GPIOB
#define PIN_LED5 LL_GPIO_PIN_0
#define LED5_CLOSE() LL_GPIO_SetOutputPin(GPIO_LED5, PIN_LED5)
#define LED5_OPEN() LL_GPIO_ResetOutputPin(GPIO_LED5, PIN_LED5)
#define LED5_TOG() LL_GPIO_TogglePin(GPIO_LED5, PIN_LED5)

// SPEAK
#define GPIO_SPEAK GPIOA
#define PIN_SPEAK LL_GPIO_PIN_5
#define SPEAK_HIGH() LL_GPIO_SetOutputPin(GPIO_SPEAK, PIN_SPEAK)
#define SPEAK_LOW() LL_GPIO_ResetOutputPin(GPIO_SPEAK, PIN_SPEAK)
#define SPEAK_TOG() LL_GPIO_TogglePin(GPIO_SPEAK, PIN_SPEAK)



// 按键
#define GPIO_KEY1 GPIOB
#define PIN_KEY1 LL_GPIO_PIN_2
#define read_key1() LL_GPIO_IsInputPinSet(GPIO_KEY1, PIN_KEY1)
#define GPIO_KEY2 GPIOB
#define PIN_KEY2 LL_GPIO_PIN_3
#define read_key2() LL_GPIO_IsInputPinSet(GPIO_KEY2, PIN_KEY2)

// 数据
#define GPIO_DATA GPIOB
#define PIN_DATA LL_GPIO_PIN_4
#define read_data() LL_GPIO_IsInputPinSet(GPIO_DATA, PIN_DATA)

void gpio_init(void);

#endif
