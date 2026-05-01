#include "gpio.h"
#include "main.h"

void gpio_init()
{
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC);

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    // LED
    GPIO_InitStruct.Pin = PIN_LED1;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_MEDIUM;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIO_LED1, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = PIN_LED2;
    LL_GPIO_Init(GPIO_LED2, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = PIN_LED3;
    LL_GPIO_Init(GPIO_LED3, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = PIN_LED4;
    LL_GPIO_Init(GPIO_LED4, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = PIN_LED5;
    LL_GPIO_Init(GPIO_LED5, &GPIO_InitStruct);
    LED1_CLOSE();
    LED2_CLOSE();
    LED3_CLOSE();
    LED4_CLOSE();
    LED5_CLOSE();

    GPIO_InitStruct.Pin = PIN_SPEAK;
    LL_GPIO_Init(GPIO_SPEAK, &GPIO_InitStruct);
    SPEAK_LOW();

    // 按键
    GPIO_InitStruct.Pin = PIN_KEY1;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_MEDIUM;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(GPIO_KEY1, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = PIN_KEY2;
    LL_GPIO_Init(GPIO_KEY2, &GPIO_InitStruct);

    // 数据
    GPIO_InitStruct.Pin = PIN_DATA;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIO_DATA, &GPIO_InitStruct);
}

