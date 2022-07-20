#ifndef __USART_H
#define __USART_H
#include "device.h"

#if defined(STM32)
#if defined(STM32HAL)
#elif defined(STM32FWLIB)
void USART1_Confi(void);
/* 中断式串口发送函数, 移植自hal库 */
bool FWLIB_UART_Transmit_IT(USART_TypeDef *USARTx, uint8_t *pdata, uint16_t size);
/* 中断式串口接收函数, 移植自hal库 */
bool FWLIB_UARTEx_ReceiveToIdle_IT(USART_TypeDef *USARTx, uint8_t *pdata, uint16_t size);
/* 中断式串口接收完毕回调函数, 移植自hal库 */
void FWLIB_UARTEx_RxEventCallback(USART_TypeDef *USARTx, uint16_t size);
/* 中断式串口发送完毕回调函数, 移植自hal库 */
void FWLIB_UART_TxCpltCallback(USART_TypeDef *USARTx);
#endif
#endif


#endif
