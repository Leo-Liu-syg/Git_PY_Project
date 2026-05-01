#include "uart.h"

/**************************************************************************
 * 模块功能：PY32F002B UART1 串口驱动
 * 通信引脚：
 *     TX  ->  PA6 (复用功能 AF1)
 *     RX  ->  PA7 (复用功能 AF3)
 * 通信配置：4800波特率、9位数据位、偶校验、1位停止位
 * 工作模式：异步全双工 + 接收中断
 *************************************************************************/

/**
 * @brief  串口初始化函数（TX:PA6  RX:PA7）
 * @param  无
 * @retval 无
 */
void uart_init(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};   /* GPIO初始化结构体 */
    LL_USART_InitTypeDef USART_InitStruct = {0}; /* 串口初始化结构体 */

    /* 1. 开启时钟：GPIOA 和 USART1 必须先开时钟才能工作 */
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_USART1);

    /* 2. 配置 TX 引脚 PA6 */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_6;                  /* 引脚6 */
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;        /* 复用功能模式 */
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH; /* 高速 */
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL; /* 推挽输出 */
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;               /* 上拉 */
    GPIO_InitStruct.Alternate = LL_GPIO_AF1_USART1;       /* 复用为 USART1_TX */
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* 3. 配置 RX 引脚 PA7 */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_7;            /* 引脚7 */
    GPIO_InitStruct.Alternate = LL_GPIO_AF3_USART1; /* 复用为 USART1_RX */
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* 4. 配置串口接收中断优先级 */
    NVIC_SetPriority(USART1_IRQn, 0); /* 设置中断优先级为0（最高） */
    NVIC_EnableIRQ(USART1_IRQn);      /* 使能 USART1 中断通道 */

    /* 5. 配置串口通信参数 */
    // USART_InitStruct.BaudRate = 9600;     /* 原注释：波特率9600 */
    USART_InitStruct.BaudRate = 4800;                               /* 当前使用：波特率4800 */
    USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_9B;             /* 数据位：9位 */
    USART_InitStruct.StopBits = LL_USART_STOPBITS_1;                /* 停止位：1位 */
    USART_InitStruct.Parity = LL_USART_PARITY_EVEN;                 /* 校验位：偶校验 */
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;  /* 收发模式 */
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE; /* 无硬件流控 */
    USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;       /* 16倍过采样 */

    /* 6. 初始化 USART1 */
    LL_USART_Init(USART1, &USART_InitStruct);

    /* 7. 设置为异步通信模式 */
    LL_USART_ConfigAsyncMode(USART1);

    /* 8. 使能 USART1 + 使能“接收寄存器非空”中断 */
    LL_USART_Enable(USART1);
    LL_USART_EnableIT_RXNE(USART1);
}

/* 全局变量：保存串口接收到的1字节数据 */
volatile uint8_t rx_data;

/**
 * @brief  USART1 中断服务函数
 * @note   每当串口收到1个字节，就会自动进入这个函数
 * @param  无
 * @retval 无
 */
void USART1_IRQHandler(void)
{
    /* 判断：接收寄存器非空 + 接收中断已使能 */
    if ((LL_USART_IsActiveFlag_RXNE(USART1) != RESET) &&
        (LL_USART_IsEnabledIT_RXNE(USART1) != RESET))
    {
        /* 读取 DR 寄存器，获取接收到的1字节数据 */
        rx_data = (uint8_t)(USART1->DR & (uint8_t)0x00FF);
    }
}

// 回显函数：把接收到的数据发回去电脑
void USart1_Send_ASCII_Back(uint8_t data)
{
    // 等待 发送寄存器为空（TXE）
    while (LL_USART_IsActiveFlag_TXE(USART1) == 0)
        ;

    // 直接用你库里已经有的函数发送
    LL_USART_TransmitData8(USART1, data);
}