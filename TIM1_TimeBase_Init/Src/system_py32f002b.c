/**
  ******************************************************************************
  * @file    system_py32f002b.c
  * @author  MCU 应用团队
  * @Version V1.0.0
  * @Date    2020-10-19
  * @brief   CMSIS Cortex-M0+ 内核设备外设访问层系统时钟源文件
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; 版权所有 (c) 2023 普冉半导体有限公司.
  * 保留所有权利.</center></h2>
  *
  * 本软件组件基于 BSD 3-Clause 许可证授权,
  * 未经许可，不得使用本文件.
  * 许可证副本可在以下网址获取:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; 版权所有 (c) 2016 STMicroelectronics.
  * 保留所有权利.</center></h2>
  *
  * 本软件组件基于 BSD 3-Clause 许可证授权,
  * 未经许可，不得使用本文件.
  * 许可证副本可在以下网址获取:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

#include "py32f0xx.h"

/* 如果没有定义外部高速时钟值，则默认 24MHz */
#if !defined  (HSE_VALUE)
#define HSE_VALUE    24000000U     /*!< 外部高速晶振频率值 (Hz) */
#endif /* HSE_VALUE */

/* 如果没有定义内部高速时钟值，则默认 24MHz */
#if !defined  (HSI_VALUE)
#define HSI_VALUE    24000000U    /*!< 内部高速时钟频率值 (Hz) */
#endif /* HSI_VALUE */

/* 如果没有定义内部低速时钟值，则默认 32768Hz */
#if !defined  (LSI_VALUE)
#define LSI_VALUE     32768U      /*!< 内部低速时钟 LSI 频率值 (Hz) */
#endif /* LSI_VALUE */

/* 如果没有定义外部低速时钟值，则默认 32768Hz */
#if !defined  (LSE_VALUE)
#define LSE_VALUE     32768U     /*!< 外部低速时钟 LSE 频率值 (Hz) */
#endif /* LSE_VALUE */

/************************* 杂项配置 ************************/
/*!< 如果需要将中断向量表重定向到内部 SRAM，取消下面注释 */
/* #define FORBID_VECT_TAB_MIGRATION */
/* #define VECT_TAB_SRAM */

/* 中断向量表偏移地址
   该值必须是 0x100 的整数倍 */
#define VECT_TAB_OFFSET  0x00
/**************************************************************/

/*----------------------------------------------------------------------------
  系统时钟全局变量定义
 *----------------------------------------------------------------------------*/
/* 该变量在三种情况下会被自动更新:
    1) 调用 CMSIS 函数 SystemCoreClockUpdate()
    2) 调用 HAL 库函数 HAL_RCC_GetHCLKFreq()
    3) 每次调用 HAL_RCC_ClockConfig() 配置系统时钟时
       注意: 如果使用该函数配置系统时钟，则无需调用前两个函数，
       因为 SystemCoreClock 会自动更新
*/
uint32_t SystemCoreClock = HSI_VALUE;

/* AHB 预分频表 */
const uint32_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
/* APB 预分频表 */
const uint32_t APBPrescTable[8] =  {0, 0, 0, 0, 1, 2, 3, 4};

#if defined(RCC_HSI48M_SUPPORT)
/* HSI 频率表（支持 48MHz） */
const uint32_t HSIFreqTable[8] = {4000000U, 8000000U, 0U, 0U, 24000000U, 48000000U, 0U, 0U};
#else
/* HSI 频率表 */
const uint32_t HSIFreqTable[8] = {4000000U, 8000000U, 0U, 0U, 24000000U, 0U, 0U, 0U};
#endif

/* 私有函数声明 -----------------------------------------------*/
#ifndef SWD_DELAY
static void DelayTime(uint32_t mdelay);
#endif /* SWD_DELAY */

/**
 * @brief  获取并更新系统核心时钟频率
 * @param  无
 * @return 无
 */
void SystemCoreClockUpdate(void)
{
    uint32_t tmp;
    uint32_t hsidiv;
    uint32_t hsifs;

    /* 获取系统时钟源 -------------------------------------------------------*/
    switch (RCC->CFGR & RCC_CFGR_SWS)
    {
    case RCC_CFGR_SWS_0:  /* HSE 作为系统时钟 */
        SystemCoreClock = HSE_VALUE;
        break;

    case (RCC_CFGR_SWS_1 | RCC_CFGR_SWS_0):  /* LSI 作为系统时钟 */
        SystemCoreClock = LSI_VALUE;
        break;

#if defined(RCC_LSE_SUPPORT)
    case RCC_CFGR_SWS_2:  /* LSE 作为系统时钟 */
        SystemCoreClock = LSE_VALUE;
        break;
#endif /* RCC_LSE_SUPPORT */

    case 0x00000000U:     /* HSI 作为系统时钟 */
    default:              /* HSI 作为系统时钟 */
        hsifs = ((READ_BIT(RCC->ICSCR, RCC_ICSCR_HSI_FS)) >> RCC_ICSCR_HSI_FS_Pos);
        hsidiv = (1UL << ((READ_BIT(RCC->CR, RCC_CR_HSIDIV)) >> RCC_CR_HSIDIV_Pos));
        SystemCoreClock = (HSIFreqTable[hsifs] / hsidiv);
        break;
    }

    /* 计算 HCLK 时钟频率 --------------------------------------------*/
    /* 获取 AHB 预分频 */
    tmp = AHBPrescTable[((RCC->CFGR & RCC_CFGR_HPRE) >> RCC_CFGR_HPRE_Pos)];
    /* 计算最终 HCLK 频率 */
    SystemCoreClock >>= tmp;
}

/**
 * @brief  微控制器系统初始化
 *         初始化系统时钟、向量表
 * @param  无
 * @return 无
 */
void SystemInit(void)
{
    /* 默认将 HSI 时钟设置为 24MHz */
    RCC->ICSCR = (RCC->ICSCR & 0xFFFF0000) | ((*(uint32_t *)(0x1FFF0100)) & 0xFFFF);

    /* 默认将 LSI 时钟设置为 32.768KHz */
    RCC->ICSCR = (RCC->ICSCR & 0xFE00FFFF) | (((*(uint32_t *)(0x1FFF0144)) & 0x1FF) << RCC_ICSCR_LSI_TRIM_Pos);

    /* 配置中断向量表位置与偏移地址 ------------------*/
#ifdef VECT_TAB_SRAM
    SCB->VTOR = SRAM_BASE | VECT_TAB_OFFSET;  /* 向量表重定向到内部 SRAM */
#else
    SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET;  /* 向量表重定向到内部 FLASH */
#endif /* VECT_TAB_SRAM */

#ifndef SWD_DELAY
    /* 当 SWD 引脚被复用为其他功能时，此函数用于解决无法下载更新程序的问题 */
    DelayTime(100);
#endif /* SWD_DELAY */
}

#ifndef SWD_DELAY
/**
  * @brief  基于 CPU 循环的毫秒级延时函数
  * @param  mdelay: 延时时间（毫秒）
  * @retval 无
  */
static void DelayTime(uint32_t mdelay)
{
    __IO uint32_t Delay = mdelay * (24000000U / 8U / 1000U);
    do
    {
        __NOP();
    }
    while (Delay--);
}
#endif /* SWD_DELAY */

/************************ (C) 版权所有 普冉半导体 *****END OF FILE******************/