#ifndef __UART_H
#define __UART_H
#include "device.h"
#define UART_NUM 1

void UART_Init(void);
bool UART1_ScanArray(uint8_t arr[], size_t size, size_t *length);
bool UART1_ScanString(char *str, size_t size);
void UART1_PrintArray(uint8_t arr[], size_t size);
void UART1_PrintString(char *str);

#endif
