#ifndef __UART_H__
#define __UART_H__

#include "main.h"

extern volatile uint8_t rx_data;

void uart_init(void);

void USart1_Send_ASCII_Back(uint8_t data);

#endif
