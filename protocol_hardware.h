#ifndef __PROTOCOL_HARDWARE_H
#define __PROTOCOL_HARDWARE_H
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t *buf;
    volatile size_t size;
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

////////////////////////////////////////////////////////////////////////////
#if defined(DEVI2C_HARDWARE_ENABLED)
__attribute__((unused)) static DEVCMNI_StatusTypeDef DEVI2C_Transmit_H(I2C_ModuleHandleTypeDef *modular, uint8_t *pdata, size_t size, uint8_t address, bool rw, uint32_t timeout) {
    if(rw) {
#if defined(STM32HAL)
#if defined(HAL_I2C_MODULE_ENABLED)
        HAL_I2C_Mem_Read(modular->bus, (modular->addr << 1) | 0X00, address, I2C_MEMADD_SIZE_8BIT, pdata, size, timeout);
#elif defined(HAL_FMPI2C_MODULE_ENABLED)
        HAL_FMPI2C_Mem_Read(modular->bus, (modular->addr << 1) | 0X00, address, FMPI2C_MEMADD_SIZE_8BIT, pdata, size, timeout);
#endif    // HAL_I2C_MODULE_ENABLED | HAL_FMPI2C_MODULE_ENABLED
#elif defined(STM32FWLIBF1)
    //固件库的硬件I2C驱动函数,待补充
#endif
    } else {
#if defined(STM32HAL)
#if defined(HAL_I2C_MODULE_ENABLED)
        HAL_I2C_Mem_Write(modular->bus, (modular->addr << 1) | 0X00, address, I2C_MEMADD_SIZE_8BIT, pdata, size, timeout);
#elif defined(HAL_FMPI2C_MODULE_ENABLED)
        HAL_FMPI2C_Mem_Write(modular->bus, (modular->addr << 1) | 0X00, address, FMPI2C_MEMADD_SIZE_8BIT, pdata, size, timeout);
#endif    // HAL_I2C_MODULE_ENABLED | HAL_FMPI2C_MODULE_ENABLED
#elif defined(STM32FWLIBF1)
    //固件库的硬件I2C驱动函数,待补充
#endif
    }
    return DEVCMNI_OK;
}
#endif    // DEVI2C_HARDWARE_ENABLED
////////////////////////////////////////////////////////////////////////////
#if defined(DEVSPI_HARDWARE_ENABLED)
__attribute__((unused)) static DEVCMNI_StatusTypeDef DEVSPI_Transmit_H(SPI_ModuleHandleTypeDef *modular, uint8_t *pdata, size_t size, bool rw, uint32_t timeout) {
    if(modular->duplex == DEVSPI_FULL_DUPLEX) {
        //to add
    } else if(modular->duplex == DEVSPI_HALF_DUPLEX) {
        if(rw) {
#if defined(STM32HAL)
#if defined(HAL_SPI_MODULE_ENABLED)
            HAL_SPI_Receive(modular->bus, pdata, size, timeout);
#elif defined(HAL_QSPI_MODULE_ENABLED)
            //todo: SPI_Write for QSPI
#endif    // HAL_SPI_MODULE_ENABLED | HAL_QSPI_MODULE_ENABLED
#elif defined(STM32FWLIBF1)
            //固件库的硬件SPI驱动函数,待补充
#endif
        } else {
#if defined(STM32HAL)
#if defined(HAL_SPI_MODULE_ENABLED)
            HAL_SPI_Transmit(modular->bus, pdata, size, timeout);
#elif defined(HAL_QSPI_MODULE_ENABLED)
            //todo: SPI_Write for QSPI
#endif    // HAL_SPI_MODULE_ENABLED | HAL_QSPI_MODULE_ENABLED
#elif defined(STM32FWLIBF1)
            //固件库的硬件SPI驱动函数,待补充
#endif
        }
    }
    return DEVCMNI_OK;
}
#endif    // DEVSPI_HARDWARE_ENABLED



////////////////////////////////////////////////////////////////////////////
#if defined(DEVOWRE_HARDWARE_ENABLED)
#endif    // DEVOWRE_HARDWARE_ENABLED

////////////////////////////////////////////////////////////////////////////
#if defined(DEVUART_HARDWARE_ENABLED)
static UART_ModuleHandleTypeDef *uartmodular;
static void DEVUART_Init(UART_ModuleHandleTypeDef *modular) {
    uartmodular = modular;
}
/* 开始串口接收 */
static void DEVUART_ReceiveStart(void) {
#if defined(STM32)
#if defined(STM32HAL)
#if defined(HAL_UART_MODULE_ENABLED)
    if(uartmodular->usedma) {
#if defined(HAL_DMA_MODULE_ENABLED)
        HAL_UARTEx_ReceiveToIdle_DMA(uartmodular->bus, uartmodular->receive.buf, uartmodular->receive.size);
#endif    // HAL_DMA_MODULE_ENABLED
    } else {
        HAL_UARTEx_ReceiveToIdle_IT(uartmodular->bus, uartmodular->receive.buf, uartmodular->receive.size);
    }
#endif    // HAL_UART_MODULE_ENABLED
#elif defined(STM32FWLIB)
    FWLIB_UARTEx_ReceiveToIdle_IT(uartmodular->bus, uartmodular->receive.buf, uartmodular->receive.size);
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
__attribute__((unused)) static DEVCMNI_StatusTypeDef DEVUART_Receive(UART_ModuleHandleTypeDef *modular, uint8_t *pdata, size_t size, size_t *length) {
    DEVUART_Init(modular);
    if(!uartmodular->receive.state) {
        uartmodular->receive.size = size;
        uartmodular->receive.buf = pdata;
        if(uartmodular->receive.size != 0 && uartmodular->receive.buf != NULL) {
            DEVUART_ReceiveStart();
        }
    } else {
        *length = uartmodular->receive.count;
        uartmodular->receive.count = 0;
        uartmodular->receive.state = false;
        return DEVCMNI_OK;
    }
    return DEVCMNI_BUSY;
}
__attribute__((unused)) static DEVCMNI_StatusTypeDef DEVUART_Transmit(UART_ModuleHandleTypeDef *modular, uint8_t *pdata, size_t size) {
    DEVUART_Init(modular);
    if(uartmodular->transmit.state) {
        uartmodular->transmit.size = size;
        uartmodular->transmit.buf = pdata;
        if(uartmodular->transmit.size != 0 && uartmodular->transmit.buf != NULL) {
            DEVUART_TransmitStart();
        }
        uartmodular->transmit.state = false;
        return DEVCMNI_OK;
    }
    return DEVCMNI_BUSY;
}
__attribute__((unused)) static void DEVUART_ReceiveReady(UART_ModuleHandleTypeDef *muart, size_t count) {
    muart->receive.state = true;
    muart->receive.count = count;
}
__attribute__((unused)) static void DEVUART_TransmitReady(UART_ModuleHandleTypeDef *muart) {
    muart->transmit.state = true;
}
#endif    // DEVUART_HARDWARE_ENABLED

#endif    // !__PROTOCOL_HARDWARE_H
