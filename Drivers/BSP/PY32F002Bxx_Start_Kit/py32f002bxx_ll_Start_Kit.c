/**
  ******************************************************************************
  * @file    py32f002bxx_ll_Start_Kit.c
  * @author  MCU 应用团队
  * @brief   该文件提供了板载 LED、按键 的驱动函数
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; 版权所有 (c) 2023 普冉半导体有限公司.
  * 保留所有权利.</center></h2>
  *
  * 本软件基于 BSD 3-Clause 开源协议
  * 未获得许可，不得使用
  * 协议地址：opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* 包含头文件 ------------------------------------------------------------------*/
#include "py32f002bxx_ll_Start_Kit.h"

/**
  * @brief PY32F002Bxx 开发板 BSP 驱动版本号
  */
#define __PY32F002Bxx_STK_BSP_VERSION_MAIN   (0x00U) /*!< 主版本号 */
#define __PY32F002Bxx_STK_BSP_VERSION_SUB1   (0x00U) /*!< 子版本号1 */
#define __PY32F002Bxx_STK_BSP_VERSION_SUB2   (0x01U) /*!< 子版本号2 */
#define __PY32F002Bxx_STK_BSP_VERSION_RC     (0x00U) /*!< 发布候选版本 */
#define __PY32F002Bxx_STK_BSP_VERSION        ((__PY32F002Bxx_STK_BSP_VERSION_MAIN << 24) \
                                             |(__PY32F002Bxx_STK_BSP_VERSION_SUB1 << 16) \
                                             |(__PY32F002Bxx_STK_BSP_VERSION_SUB2 << 8 ) \
                                             |(__PY32F002Bxx_STK_BSP_VERSION_RC))

/* LED 端口数组 */
GPIO_TypeDef* LED_PORT[LEDn] = {LED3_GPIO_PORT};
/* LED 引脚数组 */
const uint16_t LED_PIN[LEDn] = {LED3_PIN};

/* 按键端口数组 */
GPIO_TypeDef* BUTTON_PORT[BUTTONn] = {USER_BUTTON_GPIO_PORT };
/* 按键引脚数组 */
const uint16_t BUTTON_PIN[BUTTONn] = {USER_BUTTON_PIN };
/* 按键中断号数组 */
const uint8_t BUTTON_IRQn[BUTTONn] = {USER_BUTTON_EXTI_IRQn };
/* 按键外部中断线数组 */
const uint32_t BUTTON_EXTI_LINE[BUTTONn] = {USER_BUTTON_EXTI_LINE };

/** @addtogroup PY32F002Bxx_STK_Exported_Functions
  * @{
  */

/**
  * @brief  获取 BSP 驱动版本号
  * @retval 版本号：0xXYZR
  */
uint32_t BSP_GetVersion(void)
{
  return __PY32F002Bxx_STK_BSP_VERSION;
}

/** @addtogroup LED_Functions
  * @{
  */

/**
  * @brief  配置 LED 对应的 GPIO 引脚
  * @param  Led：要初始化的 LED（仅支持 LED3）
  * @retval 无
  */
void BSP_LED_Init(Led_TypeDef Led)
{
  /* 使能 LED 对应的 GPIO 时钟 */
  LEDx_GPIO_CLK_ENABLE(Led);

  /* 配置为推挽输出模式 */
  LL_GPIO_SetPinMode(LED_PORT[Led], LED_PIN[Led], LL_GPIO_MODE_OUTPUT);

  /* 默认输出高电平（LED 熄灭） */
  LL_GPIO_SetOutputPin(LED_PORT[Led], LED_PIN[Led]);
}

/**
  * @brief  取消 LED 引脚初始化
  * @param  Led：要取消初始化的 LED
  * @note   不会关闭 GPIO 时钟
  * @retval 无
  */
void BSP_LED_DeInit(Led_TypeDef Led)
{
  /* 熄灭 LED */
  LL_GPIO_ResetOutputPin(LED_PORT[Led], LED_PIN[Led]);
  /* 配置为模拟模式，关闭 GPIO 功能 */
  LL_GPIO_SetPinMode(LED_PORT[Led], LED_PIN[Led], LL_GPIO_MODE_ANALOG);
}

/**
  * @brief  点亮指定 LED
  * @param  Led：要点亮的 LED
  * @retval 无
  */
void BSP_LED_On(Led_TypeDef Led)
{
  LL_GPIO_ResetOutputPin(LED_PORT[Led], LED_PIN[Led]);
}

/**
  * @brief  熄灭指定 LED
  * @param  Led：要熄灭的 LED
  * @retval 无
  */
void BSP_LED_Off(Led_TypeDef Led)
{
  LL_GPIO_SetOutputPin(LED_PORT[Led], LED_PIN[Led]);
}

/**
  * @brief  翻转 LED 状态（亮 ↔ 灭）
  * @param  Led：要翻转的 LED
  * @retval 无
  */
void BSP_LED_Toggle(Led_TypeDef Led)
{
  LL_GPIO_TogglePin(LED_PORT[Led], LED_PIN[Led]);
}

/**
  * @brief  配置按键 GPIO 和外部中断
  * @param  Button：要配置的按键（仅支持 USER 按键）
  * @param  ButtonMode：按键模式
  *         @arg BUTTON_MODE_GPIO：普通输入模式
  *         @arg BUTTON_MODE_EXTI：外部中断模式
  * @retval 无
  */
void BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode)
{
  /* 使能按键 GPIO 时钟 */
  BUTTONx_GPIO_CLK_ENABLE(Button);

  /* 配置为输入模式 */
  LL_GPIO_SetPinMode(BUTTON_PORT[Button], BUTTON_PIN[Button], LL_GPIO_MODE_INPUT);
  /* 无上下拉 */
  LL_GPIO_SetPinPull(BUTTON_PORT[Button], BUTTON_PIN[Button], LL_GPIO_PULL_NO);

  if(ButtonMode == BUTTON_MODE_EXTI)
  {
    /* 使能外部中断 */
    LL_EXTI_EnableIT(BUTTON_EXTI_LINE[Button]);
    /* 下降沿触发 */
    LL_EXTI_EnableFallingTrig(BUTTON_EXTI_LINE[Button]);

    /* 配置中断优先级并使能中断 */
    NVIC_SetPriority((IRQn_Type)(BUTTON_IRQn[Button]), 0x0F);
    NVIC_EnableIRQ((IRQn_Type)(BUTTON_IRQn[Button]));
  }
}

/**
  * @brief  取消按键初始化
  * @param  Button：要取消初始化的按键
  * @note   不会关闭 GPIO 时钟
  * @retval 无
  */
void BSP_PB_DeInit(Button_TypeDef Button)
{
  /* 关闭中断 */
  NVIC_DisableIRQ((IRQn_Type)(BUTTON_IRQn[Button]));
  /* 配置为模拟模式 */
  LL_GPIO_SetPinMode(BUTTON_PORT[Button], BUTTON_PIN[Button], LL_GPIO_MODE_ANALOG);
}

/**
  * @brief  获取按键状态
  * @param  Button：要读取的按键
  * @retval 按键状态（0/1）
  */
uint32_t BSP_PB_GetState(Button_TypeDef Button)
{
  return LL_GPIO_IsInputPinSet(BUTTON_PORT[Button], BUTTON_PIN[Button]);
}

/**
  * @brief  配置调试串口 USART
  * @param  无
  * @retval 无
  */
void BSP_USART_Config(void)
{
#if defined (__GNUC__)
  setvbuf(stdout,NULL,_IONBF,0);
#endif
  /* 使能 USART 时钟 */
  DEBUG_USART_CLK_ENABLE();

  /* 初始化串口：115200 8N1 */
  LL_USART_SetBaudRate(DEBUG_USART, SystemCoreClock, LL_USART_OVERSAMPLING_16, DEBUG_USART_BAUDRATE);
  LL_USART_SetDataWidth(DEBUG_USART, LL_USART_DATAWIDTH_8B);
  LL_USART_SetStopBitsLength(DEBUG_USART, LL_USART_STOPBITS_1);
  LL_USART_SetParity(DEBUG_USART, LL_USART_PARITY_NONE);
  LL_USART_SetHWFlowCtrl(DEBUG_USART, LL_USART_HWCONTROL_NONE);
  LL_USART_SetTransferDirection(DEBUG_USART, LL_USART_DIRECTION_TX_RX);
  LL_USART_Enable(DEBUG_USART);
  LL_USART_ClearFlag_TC(DEBUG_USART);

  /* 配置 TX/RX 引脚为复用功能 */
  DEBUG_USART_RX_GPIO_CLK_ENABLE();
  DEBUG_USART_TX_GPIO_CLK_ENABLE();

  /* TX 引脚配置 */
  LL_GPIO_SetPinMode(DEBUG_USART_TX_GPIO_PORT, DEBUG_USART_TX_PIN, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetPinSpeed(DEBUG_USART_TX_GPIO_PORT, DEBUG_USART_TX_PIN, LL_GPIO_SPEED_FREQ_VERY_HIGH);
  LL_GPIO_SetPinPull(DEBUG_USART_TX_GPIO_PORT, DEBUG_USART_TX_PIN, LL_GPIO_PULL_UP);
  LL_GPIO_SetAFPin_0_7(DEBUG_USART_TX_GPIO_PORT, DEBUG_USART_TX_PIN, DEBUG_USART_TX_AF);

  /* RX 引脚配置 */
  LL_GPIO_SetPinMode(DEBUG_USART_RX_GPIO_PORT, DEBUG_USART_RX_PIN, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetPinSpeed(DEBUG_USART_RX_GPIO_PORT, DEBUG_USART_RX_PIN, LL_GPIO_SPEED_FREQ_VERY_HIGH);
  LL_GPIO_SetPinPull(DEBUG_USART_RX_GPIO_PORT, DEBUG_USART_RX_PIN, LL_GPIO_PULL_UP);
  LL_GPIO_SetAFPin_0_7(DEBUG_USART_RX_GPIO_PORT, DEBUG_USART_RX_PIN, DEBUG_USART_RX_AF);
}

#if (defined (__CC_ARM)) || (defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
/**
  * @brief  重定向 printf 到串口
  * @param  ch：要发送的字符
  * @retval 发送的字符
  */
int fputc(int ch, FILE *f)
{
  /* 通过串口发送一个字节 */
  LL_USART_TransmitData8(DEBUG_USART, ch);
  while (!LL_USART_IsActiveFlag_TC(DEBUG_USART));
  LL_USART_ClearFlag_TC(DEBUG_USART);

  return (ch);
}

/**
  * @brief  重定向 scanf 从串口读取
  * @param  f：文件指针
  * @retval 读取到的字符
  */
int fgetc(FILE *f)
{
  int ch;
  while (!LL_USART_IsActiveFlag_RXNE(DEBUG_USART));
  ch = LL_USART_ReceiveData8(DEBUG_USART);
  return (ch);
}

#elif defined(__ICCARM__)
/**
  * @brief  IAR 下 printf 重定向
  */
int putchar(int ch)
{
  LL_USART_TransmitData8(DEBUG_USART, ch);
  while (!LL_USART_IsActiveFlag_TC(DEBUG_USART));
  LL_USART_ClearFlag_TC(DEBUG_USART);

  return (ch);
}
#elif  defined(__GNUC__)
/**
  * @brief  GCC 下 printf 重定向
  */
int __io_putchar(int ch)
{
  LL_USART_TransmitData8(DEBUG_USART, ch);
  while (!LL_USART_IsActiveFlag_TC(DEBUG_USART));
  LL_USART_ClearFlag_TC(DEBUG_USART);
  return ch;
}

int _write(int file, char *ptr, int len)
{
  int DataIdx;
  for (DataIdx=0;DataIdx<len;DataIdx++)
  {
    __io_putchar(*ptr++);
  }
  return len;
}

#endif

/**
  * @}
  */

/************************ (C) COPYRIGHT Puya *****END OF FILE******************/