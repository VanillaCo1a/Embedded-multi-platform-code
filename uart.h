#ifndef __UART_H
#define __UART_H
#include "head.h"

void UART_Init(void);
void UART1_Ctrl(void);
int8_t UART1_Transmit(void);
void UART1_Receive(void);

#endif
