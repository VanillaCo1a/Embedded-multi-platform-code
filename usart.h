#ifndef __USART_H
#define __USART_H
#include "device.h"

#if defined(STM32)
#if defined(STM32HAL)
#elif defined(STM32FWLIB)
void USART1_Confi(void);
/* 中断式串口接收函数, 移植自hal库 */
int8_t FWLIB_UART_Receive_IT(USART_TypeDef *USARTx, uint8_t *pdata, uint16_t size);
/* 中断式串口接收至总线空闲函数, 移植自hal库 */
int8_t FWLIB_UARTEx_ReceiveToIdle_IT(USART_TypeDef *USARTx, uint8_t *pdata, uint16_t size);
/* 中断式串口发送函数, 移植自hal库 */
int8_t FWLIB_UART_Transmit_IT(USART_TypeDef *USARTx, uint8_t *pdata, uint16_t size);
/* 指定空间数据接收完毕中断回调函数 */
void FWLIB_UART_RxCpltCallback(USART_TypeDef *USARTx);
/* 指定空间数据接收完毕/总线空闲时中断回调函数 */
void FWLIB_UARTEx_RxEventCallback(USART_TypeDef *USARTx, uint16_t size);
/* 数据发送完毕中断回调函数 */
void FWLIB_UART_TxCpltCallback(USART_TypeDef *USARTx);
#endif
#endif


#endif
