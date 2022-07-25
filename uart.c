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
