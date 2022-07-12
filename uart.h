#ifndef __UART_H
#define __UART_H
#include "device.h"

#define UARTNUM 1
#define BUFSIZE 2000

void UART_Init(void);
size_t UART1_ScanArray(uint8_t arr[], size_t size);
size_t UART1_ScanString(char *str, size_t size);
bool UART1_PrintArray(uint8_t arr[], size_t size);
bool UART1_PrintString(char *str);

#endif
