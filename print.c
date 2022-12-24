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

static char *va_buf = NULL;
static size_t va_size = 0;

/* PRINT构造函数 */
void PRINT_Init(DEVS_TypeDef *devs, DEV_TypeDef dev[], poolsize devSize, char *buf, size_t bufSize) {
    prints = devs;
    print = dev;
    va_buf = buf;
    va_size = bufSize;

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

bool PRINT_ScanArray(int8_t num, uint8_t arr[], size_t size, size_t *length, DEV_StatusTypeDef wait) {
    uint8_t cm_pa = 0x00;
    DEV_SetActStream(prints, num);
    return (DEVCMNI_Read((uint8_t *)arr, size, length, &cm_pa) == wait);
}
bool PRINT_ScanString(int8_t num, char *str, size_t size, DEV_StatusTypeDef wait) {
    DEV_StatusTypeDef rc;
    bool res = false;
    size_t length = 0;
    uint8_t cm_pa = 0x00;
    DEV_SetActStream(prints, num);
    if((rc = DEVCMNI_Read((uint8_t *)str, size - 1, &length, &cm_pa)) == wait) {
        res = true;
    }
    if(rc == DEV_OK) {
        str[length] = '\0';
    }
    return res;
}
bool PRINT_PrintArray(int8_t num, const uint8_t arr[], size_t size, DEV_StatusTypeDef wait) {
    size_t length = 0;
    uint8_t cm_pa = 0x00;
    DEV_SetActStream(prints, num);
    return (DEVCMNI_Write((uint8_t *)arr, size, &length, &cm_pa) == wait);
}
bool PRINT_PrintString(int8_t num, const char *str, DEV_StatusTypeDef wait) {
    size_t length = 0;
    uint8_t cm_pa = 0x00;
    DEV_SetActStream(prints, num);
    return (DEVCMNI_Write((uint8_t *)str, strlen(str), &length, &cm_pa) == wait);
}
bool PRINT_Printf(int8_t num, char *str, DEV_StatusTypeDef wait, ...) {
    va_list args;
    va_start(args, str);
    vsnprintf(va_buf, va_size, (char *)str, args);
    va_end(args);
    return PRINT_PrintString(num, va_buf, wait);
}
