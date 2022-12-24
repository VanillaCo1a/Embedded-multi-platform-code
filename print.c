#include "print.h"

/***  PRINT类, 初始化实例的样例如下:
static UART_ModuleHandleTypeDef uart_muart[PRINT_NUM] = {
    {.cmni = {
         .protocol = USART,
         .ware = HARDWARE,
#if defined(STM32)
#if defined(STM32HAL)
         .bus = &huart1,
#elif defined(STM32FWLIB)
         .bus = USART1,
#endif
#endif
     },
     .usedma = 0}};
static DEVS_TypeDef myprints = {.type = PRINT};
static DEV_TypeDef myprint[] = {{.parameter = NULL, .io = {0}, .cmni = {.num = 1, .confi = (DEVCMNI_TypeDef *)&print_muart[0], .init = NULL}}};    ***/



/*****    PRINT外部调用接口    *****/

static DEVS_TypeDef *prints = NULL;
static DEV_TypeDef *print = NULL;
static int8_t print_stdnum = 0;

/* PRINT构造函数 */
void PRINT_Init(DEVS_TypeDef *devs, DEV_TypeDef dev[], poolsize devSize, int8_t stdnum) {
    prints = devs;
    print = dev;
    print_stdnum = stdnum;

    /* 初始化设备类和设备, 将参数绑定到设备池中, 并初始化通信引脚 */
    DEV_Init(prints, print, devSize);

    /* 初始化PRINT设备 */
    PRINT_DevInit();
}

/* TODO: PRINT析构函数 */
void PRINT_Deinit(DEVS_TypeDef *devs, DEV_TypeDef dev[], poolsize devSize) {}

void PRINT_DevInit(void) {
#if defined(STM32)
#if defined(STM32HAL)
#elif defined(STM32FWLIB)
    USART1_Confi();
#endif
#endif
}

bool PRINT_ScanArray(int8_t num, uint8_t arr[], size_t size, size_t *len) {
    uint8_t *buf = arr;
    DEV_SetActStream(prints, num);
    return DEV_ScanArray(DEV_OK, buf, size, arr, len);
}
bool PRINT_ScanString(int8_t num, char str[], size_t size) {
    uint8_t *buf = (uint8_t *)str;
    DEV_SetActStream(prints, num);
    return DEV_ScanString(DEV_OK, buf, size, str);
}
bool PRINT_PrintArray(int8_t num, uint8_t arr[], size_t size) {
    uint8_t *buf = arr;
    DEV_SetActStream(prints, num);
    return DEV_PrintArray(DEV_OK, buf, size, arr);
}
bool PRINT_PrintString(int8_t num, uint8_t buf[], size_t size, const char *str) {
    DEV_SetActStream(prints, num);
    return DEV_PrintString(DEV_OK, buf, size, str);
}
bool PRINT_Printf(int8_t num, uint8_t buf[], size_t size, const char *str, ...) {
    bool res;
    va_list args;
    va_start(args, str);
    DEV_SetActStream(prints, num);
    res = DEV_Printf(DEV_OK, buf, size, str, args);
    va_end(args);
    return res;
}



/*****    stdio重定向    *****/

int fputc(int ch, FILE *f) {
    size_t length = 0;
    uint8_t cm_pa = 0x00;
    DEVS_TypeDef *devs = DEV_GetActDevs();
    poolsize dev = DEV_GetActStream();
    DEV_SetActStream(prints, print_stdnum);
    while(DEVCMNI_Write((uint8_t *)&ch, 1, &length, &cm_pa) != DEV_OK) continue;
    DEV_SetActStream(devs, dev);
    return ch;
}
int _write(int fd, char *pBuffer, int size) {
    size_t length = 0;
    uint8_t cm_pa = 0x00;
    DEVS_TypeDef *devs = DEV_GetActDevs();
    poolsize dev = DEV_GetActStream();
    DEV_SetActStream(prints, print_stdnum);
    while(DEVCMNI_Write((uint8_t *)pBuffer, size, &length, &cm_pa) != DEV_OK) continue;
    DEV_SetActStream(devs, dev);
    return size;
}
int fgetc(FILE *f) {
    int ch;
    size_t length = 0;
    uint8_t cm_pa = 0x00;
    DEVS_TypeDef *devs = DEV_GetActDevs();
    poolsize dev = DEV_GetActStream();
    DEV_SetActStream(prints, print_stdnum);
    while(DEVCMNI_Read((uint8_t *)&ch, 1, &length, &cm_pa) != DEV_OK) continue;
    DEV_SetActStream(devs, dev);
    return ch;
}
