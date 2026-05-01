#include "uart.h"
#include "main.h"

/// @brief 串口初始化 TX：PA6 RX：PA7
void uart_init()
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    LL_USART_InitTypeDef USART_InitStruct = {0};
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_USART1);

    /* TX */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_6;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Alternate = LL_GPIO_AF1_USART1;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* RX */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_7;
    GPIO_InitStruct.Alternate = LL_GPIO_AF3_USART1;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Set USART1 interrupt priority  */
    NVIC_SetPriority(USART1_IRQn, 0);
    /* Enable USART1 interrupt request */
    NVIC_EnableIRQ(USART1_IRQn);

    /* Set USART feature */
    /* Set baud rate */
    // USART_InitStruct.BaudRate = 9600;
    USART_InitStruct.BaudRate = 4800;
    /* set word length to 8 bits: Start bit, 8 data bits, n stop bits */
    USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_9B;
    /* 1 stop bit */
    USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    /* Parity control disabled  */
    USART_InitStruct.Parity = LL_USART_PARITY_EVEN;
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;

    /* Initialize USART */
    LL_USART_Init(USART1, &USART_InitStruct);

    /* Set mode as full-duplex asynchronous mode */
    LL_USART_ConfigAsyncMode(USART1);

    /*Enable USART */
    LL_USART_Enable(USART1);
    LL_USART_EnableIT_RXNE(USART1);
}

uint8_t rx_data;
void USART1_IRQHandler(void)
{
    /* receive data register not empty */
    if ((LL_USART_IsActiveFlag_RXNE(USART1) != RESET) && (LL_USART_IsEnabledIT_RXNE(USART1) != RESET))
    {
        /* Receive data */
        rx_data = (uint8_t)(USART1->DR & (uint8_t)0x00FF);
    }
}
