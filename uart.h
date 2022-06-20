#ifndef __UART_H
#define __UART_H
#include "head.h"

extern char tail2[5];
extern int8_t numTransmitBuf1, numReceiveBuf1;
extern Queue_structure *strBuf1_Receive, *strBuf1_Transmit;

void UART_Init(void);

int8_t writeBuf1_Transmit(uint8_t *);
uint8_t *readBuf1_Receive(void);
int8_t UART1_Transmit(void);
void UART1_Receive(void);

#endif
