#ifndef __PROTOCOL_HARDWARE_H
#define __PROTOCOL_HARDWARE_H
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/* 宏函数 */
#define MIN(A, B) ((A) < (B)) ? (A) : (B)
#define MAX(A, B) ((A) > (B)) ? (A) : (B)

#define BUFFERSIZE 2000

////////////////////////////////////////////////////////////////////////////
typedef struct {
    uint8_t *buf;
    volatile size_t size;
    volatile size_t size_n;
    volatile size_t count;
    volatile bool state;
} BufferTypedef;
typedef struct {
    BufferTypedef receive;
    BufferTypedef transmit;
    bool usedma;
    bool checkidle;
    void *bus;
} UART_ModuleHandleTypeDef;

#ifdef DEVI2C_HARDWARE_ENABLED
#endif    // DEVI2C_HARDWARE_ENABLED

////////////////////////////////////////////////////////////////////////////
#ifdef DEVSPI_HARDWARE_ENABLED
#endif    // DEVSPI_HARDWARE_ENABLED

////////////////////////////////////////////////////////////////////////////
#ifdef DEVOWRE_HARDWARE_ENABLED
#endif    // DEVOWRE_HARDWARE_ENABLED

////////////////////////////////////////////////////////////////////////////
#ifdef DEVUART_HARDWARE_ENABLED
static UART_ModuleHandleTypeDef *uartmodular;
static void DEVUART_Init(UART_ModuleHandleTypeDef *modular) {
    uartmodular = modular;
}
/* 开始串口接收 */
static void DEVUART_ReceiveStart(void) {
#if defined(STM32)
#if defined(STM32HAL)
#if defined(HAL_UART_MODULE_ENABLED)
    HAL_UARTEx_ReceiveToIdle_IT(uartmodular->bus, uartmodular->receive.buf, uartmodular->receive.size_n);
#endif    // HAL_UART_MODULE_ENABLED
#elif defined(STM32FWLIB)
    FWLIB_UARTEx_ReceiveToIdle_IT(uartmodular->bus, uartmodular->receive.buf, uartmodular->receive.size_n);
#endif
#endif
}
/* 开始串口发送 */
static void DEVUART_TransmitStart(void) {
#if defined(STM32)
#if defined(STM32HAL)
#if defined(HAL_UART_MODULE_ENABLED)
    if(uartmodular->usedma) {
#if defined(HAL_DMA_MODULE_ENABLED)
        HAL_UART_Transmit_DMA(uartmodular->bus, uartmodular->transmit.buf, uartmodular->transmit.size);
#endif    // HAL_DMA_MODULE_ENABLED
    } else {
        HAL_UART_Transmit_IT(uartmodular->bus, uartmodular->transmit.buf, uartmodular->transmit.size);
    }
#endif    // HAL_UART_MODULE_ENABLED
#elif defined(STM32FWLIB)
    FWLIB_UART_Transmit_IT(uartmodular->bus, uartmodular->transmit.buf, uartmodular->transmit.size);
#endif
#endif
}
__attribute__((unused)) static bool DEVUART_Receive(UART_ModuleHandleTypeDef *modular, uint8_t *pdata, size_t size, size_t *length) {
    DEVUART_Init(modular);
    if(!uartmodular->receive.state) {
        /* 将待接收区域的空间设置为size, 但不大于BUFSIZE */
        uartmodular->receive.size_n = MIN(size, BUFFERSIZE);
        uartmodular->receive.buf = pdata;
        if(uartmodular->receive.size_n != 0 && uartmodular->receive.buf != NULL) {
            DEVUART_ReceiveStart();
        }
    } else {
        *length = uartmodular->receive.size;
        uartmodular->receive.state = false;
        return true;
    }
    return false;
}
__attribute__((unused)) static bool DEVUART_Transmit(UART_ModuleHandleTypeDef *modular, uint8_t *pdata, size_t size) {
    DEVUART_Init(modular);
    if(uartmodular->transmit.state) {
        /* 将待发送区域的空间设置为size, 超过BUFSIZE的部分将被弃置 */
        uartmodular->transmit.size = MIN(size, BUFFERSIZE);
        uartmodular->transmit.buf = pdata;
        if(uartmodular->transmit.size != 0 && uartmodular->transmit.buf != NULL) {
            DEVUART_TransmitStart();
        }
        uartmodular->transmit.state = false;
        return true;
    }
    return false;
}
__attribute__((unused)) static void DEVUART_ReceiveReady(UART_ModuleHandleTypeDef *muart, size_t size) {
    muart->receive.state = true;
    muart->receive.size = size;
}
__attribute__((unused)) static void DEVUART_TransmitReady(UART_ModuleHandleTypeDef *muart) {
    muart->transmit.state = true;
}
#endif    // DEVUART_HARDWARE_ENABLED

#undef MIN
#undef MAX
#endif    // !__PROTOCOL_HARDWARE_H
