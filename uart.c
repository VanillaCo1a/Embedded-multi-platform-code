#include "uart.h"
#include "usart.h"

UART_ModuleHandleTypeDef muart1 = {.receive = {.state = 0}, .transmit = {.state = 1}, .usedma = 0};
DEVCMNI_TypeDef uart1_cmni = {.protocol = USART, .ware = HARDWARE, .modular = &muart1,
#if defined(STM32)
#if defined(STM32HAL)
                              .bus = &huart1
#elif defined(STM32FWLIB)
                              .bus = USART1
#endif
#endif
};
DEVS_TypeDef uarts = {.type = UART};
DEV_TypeDef uart[UART_NUM] = {
    {.parameter = NULL, .io = {0}, .cmni = {.num = 1, .confi = (DEVCMNI_TypeDef *)&uart1_cmni, .init = NULL}}};
/* 获取串口句柄对应通信句柄 */
UART_ModuleHandleTypeDef *UART_GetModular(void *bus) {
    for(size_t i = 0; i < uarts.size; i++) {
        if(((UART_ModuleHandleTypeDef *)uart[i].cmni.confi->modular)->bus == bus) {
            return uart[i].cmni.confi->modular;
        }
    }
    return NULL;
}

/* 中断回调函数 */
#if defined(STM32)
#if defined(STM32HAL)
/* 指定空间数据接收完毕中断回调函数 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    UART_ModuleHandleTypeDef *muart = UART_GetModular(huart);
    if(huart->RxState == HAL_UART_STATE_READY || huart->hdmarx->State == HAL_DMA_STATE_READY) {
        /* 判断句柄标志位, 是否为数据接收完毕 */
        DEVUART_ReceiveReady(muart, muart->receive.size);
    }
}
/* 指定空间数据半接收/接收完毕/总线空闲时中断回调函数 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size) {
    UART_ModuleHandleTypeDef *muart = UART_GetModular(huart);
    if(huart->RxState == HAL_UART_STATE_READY || huart->hdmarx->State == HAL_DMA_STATE_READY) {
        /* 判断句柄标志位, 是否为数据接收完毕/总线空闲 */
        DEVUART_ReceiveReady(muart, size);
    } else if(size == muart->receive.size / 2) {
        /* 其他情况则为数据半接收完毕, 稳妥起见再对接收长度进行一次判断 */
    }
}
/* 数据发送完毕中断回调函数 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    UART_ModuleHandleTypeDef *muart = UART_GetModular(huart);
    DEVUART_TransmitReady(muart);
}
#elif defined(STM32FWLIB)
/* 指定空间数据接收完毕中断回调函数 */
void FWLIB_UART_RxCpltCallback(USART_TypeDef *USARTx) {
    UART_ModuleHandleTypeDef *muart = UART_GetModular(USARTx);
    DEVUART_ReceiveReady(muart, muart->receive.size);
}
/* 指定空间数据接收完毕/总线空闲时中断回调函数 */
void FWLIB_UARTEx_RxEventCallback(USART_TypeDef *USARTx, uint16_t size) {
    UART_ModuleHandleTypeDef *muart = UART_GetModular(USARTx);
    DEVUART_ReceiveReady(muart, size);
}
/* 数据发送完毕中断回调函数 */
void FWLIB_UART_TxCpltCallback(USART_TypeDef *USARTx) {
    UART_ModuleHandleTypeDef *muart = UART_GetModular(USARTx);
    DEVUART_TransmitReady(muart);
}
#endif
#endif

/* 串口初始化函数 */
void UART_Init(void) {
    DEV_Init(&uarts, uart, sizeof(uart) / sizeof(*uart));
#if defined(STM32)
#if defined(STM32HAL)
#elif defined(STM32FWLIB)
    USART1_Confi();
#endif
#endif
    printf("UART1初始化完毕\r\n");
}

bool UART1_ScanArray(uint8_t arr[], size_t size, size_t *length) {
    DEV_setActStream(&uarts, 0);
    return DEVCMNI_Read(arr, size, length, 0xFF);
}
bool UART1_ScanString(char *str, size_t size) {
    bool res = false;
    size_t length;
    DEV_setActStream(&uarts, 0);
    res = DEVCMNI_Read((uint8_t *)str, size - 1, &length, 0xFF);
    if(res) {
        str[length] = '\0';
    }
    return res;
}
void UART1_PrintArray(uint8_t arr[], size_t size) {
    DEV_setActStream(&uarts, 0);
    while(!DEVCMNI_Write(arr, size, 0xFF)) continue;
}
void UART1_PrintString(char *str) {
    DEV_setActStream(&uarts, 0);
    while(!DEVCMNI_Write((uint8_t *)str, strlen(str), 0xFF)) continue;
}

/* printf重定向 */
int fputc(int ch, FILE *f) {
    /* 这里需要加一个volatile, 不然会被AC6编译优化掉 */
    volatile bool res = 0;
    DEVS_TypeDef *devs = DEV_getActDevs();
    poolsize dev = DEV_getActStream();
    DEV_setActStream(&uarts, 0);
    while(!(res = DEVCMNI_Write((uint8_t *)&ch, 1, 0xFF))) continue;
    DEV_setActStream(devs, dev);
    return ch;
}
int _write(int fd, char *pBuffer, int size) {
    volatile bool res = 0;
    DEVS_TypeDef *devs = DEV_getActDevs();
    poolsize dev = DEV_getActStream();
    DEV_setActStream(&uarts, 0);
    while(!(res = DEVCMNI_Write((uint8_t *)pBuffer, size, 0xFF))) continue;
    DEV_setActStream(devs, dev);
    return size;
}
