/**
 ******************************************************************************
 * @file    py32f002b_ll_usart.c
 * @author  MCU 应用团队
 * @brief   USART 串口底层驱动 (LL 库)
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

/* 只有定义了 USE_FULL_LL_DRIVER 才编译这份文件 */
#if defined(USE_FULL_LL_DRIVER)

/* 包含头文件 ------------------------------------------------------------------*/
#include "py32f002b_ll_usart.h"
#include "py32f002b_ll_rcc.h"
#include "py32f002b_ll_bus.h"

#ifdef  USE_FULL_ASSERT
#include "py32_assert.h"
#else
#define assert_param(expr) ((void)0U)   /* 如果没开断言，就不做参数检查 */
#endif

/** @addtogroup PY32F002B_LL_Driver
 * @{
 */

#if defined (USART1)  /* 只有芯片有 USART1 才编译 */

/** @addtogroup USART_LL
 * @{
 */

/* 私有类型 -------------------------------------------------------------*/
/* 私有变量 -------------------------------------------------------------*/
/* 私有常量 -------------------------------------------------------------*/

/* 私有宏（参数检查） ------------------------------------------------------------*/

/* 波特率最大 4.5M */
#define IS_LL_USART_BAUDRATE(__BAUDRATE__) ((__BAUDRATE__) <= 4500000U)

/* 波特率寄存器 BRR 最小值必须 ≥16 */
#define IS_LL_USART_BRR_MIN(__VALUE__) ((__VALUE__) >= 16U)

/* 波特率寄存器 BRR 最大值 ≤ 0xFFFF */
#define IS_LL_USART_BRR_MAX(__VALUE__) ((__VALUE__) <= 0x0000FFFFU)

/* 传输方向检查 */
#define IS_LL_USART_DIRECTION(__VALUE__) (((__VALUE__) == LL_USART_DIRECTION_NONE) \
|| ((__VALUE__) == LL_USART_DIRECTION_RX) \
|| ((__VALUE__) == LL_USART_DIRECTION_TX) \
|| ((__VALUE__) == LL_USART_DIRECTION_TX_RX))

/* 校验位检查 */
#define IS_LL_USART_PARITY(__VALUE__) (((__VALUE__) == LL_USART_PARITY_NONE) \
|| ((__VALUE__) == LL_USART_PARITY_EVEN) \
|| ((__VALUE__) == LL_USART_PARITY_ODD))

/* 数据位宽度检查 */
#define IS_LL_USART_DATAWIDTH(__VALUE__) (((__VALUE__) == LL_USART_DATAWIDTH_8B) \
|| ((__VALUE__) == LL_USART_DATAWIDTH_9B))

/* 过采样检查：16倍 / 8倍 */
#define IS_LL_USART_OVERSAMPLING(__VALUE__) (((__VALUE__) == LL_USART_OVERSAMPLING_16) \
|| ((__VALUE__) == LL_USART_OVERSAMPLING_8))

/* 最后一位时钟输出 */
#define IS_LL_USART_LASTBITCLKOUTPUT(__VALUE__) (((__VALUE__) == LL_USART_LASTCLKPULSE_NO_OUTPUT) \
|| ((__VALUE__) == LL_USART_LASTCLKPULSE_OUTPUT))

/* 时钟相位 */
#define IS_LL_USART_CLOCKPHASE(__VALUE__) (((__VALUE__) == LL_USART_PHASE_1EDGE) \
|| ((__VALUE__) == LL_USART_PHASE_2EDGE))

/* 时钟极性 */
#define IS_LL_USART_CLOCKPOLARITY(__VALUE__) (((__VALUE__) == LL_USART_POLARITY_LOW) \
|| ((__VALUE__) == LL_USART_POLARITY_HIGH))

/* 时钟输出使能 */
#define IS_LL_USART_CLOCKOUTPUT(__VALUE__) (((__VALUE__) == LL_USART_CLOCK_DISABLE) \
|| ((__VALUE__) == LL_USART_CLOCK_ENABLE))

/* 停止位检查 */
#define IS_LL_USART_STOPBITS(__VALUE__) (((__VALUE__) == LL_USART_STOPBITS_1) \
|| ((__VALUE__) == LL_USART_STOPBITS_2))

/* 硬件流控检查 */
#define IS_LL_USART_HWCONTROL(__VALUE__) (((__VALUE__) == LL_USART_HWCONTROL_NONE) \
|| ((__VALUE__) == LL_USART_HWCONTROL_RTS) \
|| ((__VALUE__) == LL_USART_HWCONTROL_CTS) \
|| ((__VALUE__) == LL_USART_HWCONTROL_RTS_CTS))

/* 导出函数 --------------------------------------------------------*/

/**
 * @brief  USART 去初始化（复位寄存器到默认值）
 * @param  USARTx 串口实例
 * @retval 成功/失败
 */
ErrorStatus LL_USART_DeInit(USART_TypeDef *USARTx)
{
    ErrorStatus status = SUCCESS;

    /* 检查参数 */
    assert_param(IS_UART_INSTANCE(USARTx));

    if (USARTx == USART1)
    {
        /* 强制复位 USART1 */
        LL_APB1_GRP2_ForceReset(LL_APB1_GRP2_PERIPH_USART1);
        /* 释放复位 */
        LL_APB1_GRP2_ReleaseReset(LL_APB1_GRP2_PERIPH_USART1);
    }
    else
    {
        status = ERROR;
    }

    return (status);
}

/**
 * @brief  根据结构体初始化 USART
 * @note   调用前必须先关闭串口
 * @param  USARTx 串口实例
 * @param  USART_InitStruct 初始化结构体
 * @retval 成功/失败
 */
ErrorStatus LL_USART_Init(USART_TypeDef *USARTx, LL_USART_InitTypeDef *USART_InitStruct)
{
    ErrorStatus status = ERROR;
    uint32_t periphclk = LL_RCC_PERIPH_FREQUENCY_NO;
    LL_RCC_ClocksTypeDef rcc_clocks;

    /* 参数检查 */
    assert_param(IS_UART_INSTANCE(USARTx));
    assert_param(IS_LL_USART_BAUDRATE(USART_InitStruct->BaudRate));
    assert_param(IS_LL_USART_DATAWIDTH(USART_InitStruct->DataWidth));
    assert_param(IS_LL_USART_STOPBITS(USART_InitStruct->StopBits));
    assert_param(IS_LL_USART_PARITY(USART_InitStruct->Parity));
    assert_param(IS_LL_USART_DIRECTION(USART_InitStruct->TransferDirection));
    assert_param(IS_LL_USART_HWCONTROL(USART_InitStruct->HardwareFlowControl));

    /* 串口必须先关闭才能配置 */
    if (LL_USART_IsEnabled(USARTx) == 0U)
    {
        /* 配置 CR1：数据位、校验、收发模式 */
        MODIFY_REG(USARTx->CR1,
            (USART_CR1_M | USART_CR1_PCE | USART_CR1_PS | USART_CR1_TE | USART_CR1_RE),
            (USART_InitStruct->DataWidth | USART_InitStruct->Parity | USART_InitStruct->TransferDirection));

        /* 配置停止位 */
        LL_USART_SetStopBitsLength(USARTx, USART_InitStruct->StopBits);

        /* 配置硬件流控 */
        LL_USART_SetHWFlowCtrl(USARTx, USART_InitStruct->HardwareFlowControl);

        /* 获取外设时钟 */
        LL_RCC_GetSystemClocksFreq(&rcc_clocks);
        periphclk = rcc_clocks.PCLK1_Frequency;

        /* 配置波特率 */
        if ((periphclk != LL_RCC_PERIPH_FREQUENCY_NO) && (USART_InitStruct->BaudRate != 0U))
        {
            status = SUCCESS;
            LL_USART_SetBaudRate(USARTx, periphclk, USART_InitStruct->OverSampling, USART_InitStruct->BaudRate);

            /* 检查 BRR 范围 */
            assert_param(IS_LL_USART_BRR_MIN(USARTx->BRR));
            assert_param(IS_LL_USART_BRR_MAX(USARTx->BRR));
        }
    }

    return (status);
}

/**
 * @brief  将 USART 初始化结构体设为默认值
 * @param  USART_InitStruct 结构体指针
 * @retval 无
 */
void LL_USART_StructInit(LL_USART_InitTypeDef *USART_InitStruct)
{
    USART_InitStruct->BaudRate            = 9600U;          /* 波特率 9600 */
    USART_InitStruct->DataWidth           = LL_USART_DATAWIDTH_8B; /* 8位数据 */
    USART_InitStruct->StopBits            = LL_USART_STOPBITS_1;   /* 1位停止位 */
    USART_InitStruct->Parity              = LL_USART_PARITY_NONE;  /* 无校验 */
    USART_InitStruct->TransferDirection   = LL_USART_DIRECTION_TX_RX; /* 收发模式 */
    USART_InitStruct->HardwareFlowControl = LL_USART_HWCONTROL_NONE;   /* 无硬件流控 */
}

/**
 * @brief  初始化 USART 同步时钟（一般异步串口不用）
 * @param  USARTx 串口实例
 * @param  USART_ClockInitStruct 时钟结构体
 * @retval 成功/失败
 */
ErrorStatus LL_USART_ClockInit(USART_TypeDef *USARTx, LL_USART_ClockInitTypeDef *USART_ClockInitStruct)
{
    ErrorStatus status = SUCCESS;

    assert_param(IS_UART_INSTANCE(USARTx));
    assert_param(IS_LL_USART_CLOCKOUTPUT(USART_ClockInitStruct->ClockOutput));

    if (LL_USART_IsEnabled(USARTx) == 0U)
    {
        if (USART_ClockInitStruct->ClockOutput == LL_USART_CLOCK_DISABLE)
        {
            /* 关闭时钟输出 */
            LL_USART_DisableSCLKOutput(USARTx);
        }
        else
        {
            /* 配置同步时钟：极性、相位、最后一位 */
            MODIFY_REG(USARTx->CR2,
                USART_CR2_CLKEN | USART_CR2_CPHA | USART_CR2_CPOL | USART_CR2_LBCL,
                USART_CR2_CLKEN | USART_ClockInitStruct->ClockPolarity |
                USART_ClockInitStruct->ClockPhase | USART_ClockInitStruct->LastBitClockPulse);
        }
    }
    else
    {
        status = ERROR;
    }

    return (status);
}

/**
 * @brief  设置时钟结构体为默认值
 * @param  USART_ClockInitStruct 时钟结构体
 * @retval 无
 */
void LL_USART_ClockStructInit(LL_USART_ClockInitTypeDef *USART_ClockInitStruct)
{
    USART_ClockInitStruct->ClockOutput       = LL_USART_CLOCK_DISABLE;
    USART_ClockInitStruct->ClockPolarity     = LL_USART_POLARITY_LOW;
    USART_ClockInitStruct->ClockPhase        = LL_USART_PHASE_1EDGE;
    USART_ClockInitStruct->LastBitClockPulse = LL_USART_LASTCLKPULSE_NO_OUTPUT;
}

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

#endif /* USART1 */

/**
 * @}
 */

#endif /* USE_FULL_LL_DRIVER */

/************************ (C) 版权所有 普冉半导体 *****END OF FILE******************/