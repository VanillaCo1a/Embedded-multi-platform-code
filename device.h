#ifndef __DEVICE_H
#define __DEVICE_H
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

#if !defined(STM32HAL) && !defined(STM32FWLIBF1) && !defined(STM32FWLIBF4) && \
    !defined(STC89C51) && !defined(TC264) && !defined(TI) &&                  \
    !defined(ESP32) && !defined(HC32)
#define STM32
// #define STC89C
// #define TC264
// #define TI
// #define ESP32
// #define HC32
#endif

#ifdef STM32
#define STM32HAL
// #define STM32FWLIB
// #define STM32REGISTER    //是否优先使用寄存器版本
#endif
#ifdef STC89C
// #define STC89C51
#define STC89C52
#endif

// #define DEVICE_USEMACRO

#define DEVICE_I2C_SOFTWARE_ENABLED
#define DEVICE_SPI_SOFTWARE_ENABLED
#define DEVICE_ONEWIRE_SOFTWARE_ENABLED


#if defined(STM32)
#if defined(STM32HAL)
#include "main.h"
#if defined(STM32F1)
#include "stm32f1xx_hal.h"
#elif defined(STM32F4)
#include "stm32f4xx_hal.h"
#elif defined(STM32F7)
#include "stm32f7xx_hal.h"
#elif defined(STM32G0)
#include "stm32g0xx_hal.h"
#elif defined(STM32H7)
#include "stm32h7xx_hal.h"
#endif
#elif defined(STM32FWLIB)
#if defined(STM32F1)
#include "stm32f10x.h"
#elif defined(STM32F4)
#include "stm32f4xx.h"
#endif
#endif
#endif

/////////////////////////    设备控制部分    /////////////////////////
typedef enum {
    DEVUNDEF = 0,
    OLED,
    MPU6050,
    DS18B20,
    OTHER,
} DEV_TypeTypeDef;
typedef enum {
    idle = 0,
    busy = !idle,
} DEV_StateTypeDef;
#define DEVPOOL_MAXNUM 100        //设备池大小
typedef uint8_t devpool_size;     //设备池大小的数据类型
#define DEVBUSYLIST_MAXNUM 20     //忙设备列表大小
typedef uint8_t busypool_size;    //忙设备列表大小的数据类型
typedef struct {                  //设备类结构体
    DEV_TypeTypeDef type;         //设备类型
    devpool_size pool;            //设备池起始序号
    devpool_size size;            //设备数量
    devpool_size stream;          //活动设备相对序号
} DEVS_TypeDef;
typedef struct {                     //设备结构体
    busypool_size state;             //设备状态值
    void *parameter;                 //设备参数配置
    void *io;                        //设备IO配置
    struct DEVCMNI_TypeDef *cmni;    //设备通信配置
} DEV_TypeDef;
extern DEV_TypeDef *devpool[DEVPOOL_MAXNUM];
//活动设备流控制
int8_t DEV_SetActDevs(DEVS_TypeDef *self);
DEVS_TypeDef *DEV_GetActDevs(void);
int8_t DEV_SetActStream(DEVS_TypeDef *self, devpool_size stream);
DEV_TypeDef *DEV_GetActStream(void);
void DEV_CloseActStream(void);
int8_t DEV_SetStream(DEVS_TypeDef *self, devpool_size stream);
DEV_TypeDef *DEV_GetStream(DEVS_TypeDef *self);
//设备状态控制
int8_t DEV_SetActState(uint16_t us);
DEV_StateTypeDef DEV_GetActState(void);
//设备初始化
int8_t DEV_Init(DEVS_TypeDef *devs, DEV_TypeDef dev[]);
void DEV_ReCall(void (*action)(void));
void DEV_Error(void);
void DEV_Confi(DEVS_TypeDef *devs, DEV_TypeDef dev[]);


/////////////////////////    IO操作部分    /////////////////////////
//todo: 考虑能否模仿hal库, 改为使用assert_param进行有效性判断
#if defined(STM32)
#if defined(STM32REGISTER)
typedef enum {
    DEVIO_PIN_RESET = GPIO_PIN_RESET,
    DEVIO_PIN_SET = GPIO_PIN_SET
} DEVIO_PinState;
typedef struct {    //HAL库IO结构
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;
} DEVIO_TypeDef;
#if defined(DEVICE_USEMACRO)


#define DEVIO_WritePin_SET(DEVIO) ({                \
    /* Check the parameters */                      \
    assert_param(IS_GPIO_PIN((DEVIO)->GPIO_Pin));   \
    assert_param(IS_GPIO_PIN_ACTION(GPIO_PIN_SET)); \
    (DEVIO)->GPIOx->BSRR = (DEVIO)->GPIO_Pin;       \
})
#define DEVIO_WritePin_RESET(DEVIO) ({                         \
    /* Check the parameters */                                 \
    assert_param(IS_GPIO_PIN((DEVIO)->GPIO_Pin));              \
    assert_param(IS_GPIO_PIN_ACTION(GPIO_PIN_RESET));          \
    (DEVIO)->GPIOx->BSRR = (uint32_t)(DEVIO)->GPIO_Pin << 16U; \
})
#define DEVIO_ReadPin(DEVIO) ({                                                 \
    GPIO_PinState bitstatus;                                                    \
    assert_param(IS_GPIO_PIN((DEVIO)->GPIO_Pin));                               \
    if(((DEVIO)->GPIOx->IDR & (DEVIO)->GPIO_Pin) != (uint32_t)GPIO_PIN_RESET) { \
        bitstatus = GPIO_PIN_SET;                                               \
    } else {                                                                    \
        bitstatus = GPIO_PIN_RESET;                                             \
    }                                                                           \
    (DEVIO_PinState) bitstatus;                                                 \
})
#define DEV_CLK_GPIO_PIN_CONFI(DEVIO, GPIO_InitStructure) ({ \
    /* todo: clkinit */                                      \
    (GPIO_InitStructure)->Pin = (DEVIO)->GPIO_Pin;           \
    HAL_GPIO_Init((DEVIO)->GPIOx, (GPIO_InitStructure));     \
})
#else
void DEVIO_WritePin_SET(DEVIO_TypeDef *DEVIO);
void DEVIO_WritePin_RESET(DEVIO_TypeDef *DEVIO);
DEVIO_PinState DEVIO_ReadPin(DEVIO_TypeDef *DEVIO);
void DEV_CLK_GPIO_PIN_CONFI(DEVIO_TypeDef *DEVIO, GPIO_InitTypeDef *GPIO_InitStructure);
#endif    // DEVICE_USEMACRO
#elif defined(STM32HAL)
typedef enum {
    DEVIO_PIN_RESET = GPIO_PIN_RESET,
    DEVIO_PIN_SET = GPIO_PIN_SET
} DEVIO_PinState;
typedef struct {    //HAL库IO结构
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;
} DEVIO_TypeDef;
#if defined(DEVICE_USEMACRO)
#define DEVIO_WritePin_SET(DEVIO) HAL_GPIO_WritePin((DEVIO)->GPIOx, (DEVIO)->GPIO_Pin, GPIO_PIN_SET)
#define DEVIO_WritePin_RESET(DEVIO) HAL_GPIO_WritePin((DEVIO)->GPIOx, (DEVIO)->GPIO_Pin, GPIO_PIN_RESET)
#define DEVIO_ReadPin(DEVIO) (DEVIO_PinState) HAL_GPIO_ReadPin((DEVIO)->GPIOx, (DEVIO)->GPIO_Pin)
#define DEV_CLK_GPIO_PIN_CONFI(DEVIO, GPIO_InitStructure) ({ \
    /* todo: clkinit */                                      \
    (GPIO_InitStructure)->Pin = (DEVIO)->GPIO_Pin;           \
    HAL_GPIO_Init((DEVIO)->GPIOx, (GPIO_InitStructure));     \
})
#else
void DEVIO_WritePin_SET(DEVIO_TypeDef *DEVIO);
void DEVIO_WritePin_RESET(DEVIO_TypeDef *DEVIO);
DEVIO_PinState DEVIO_ReadPin(DEVIO_TypeDef *DEVIO);
void DEV_CLK_GPIO_PIN_CONFI(DEVIO_TypeDef *DEVIO, GPIO_InitTypeDef *GPIO_InitStructure);
#endif    // DEVICE_USEMACRO
#elif defined(STM32FWLIB)
typedef enum {
    DEVIO_PIN_RESET = GPIO_PIN_RESET,
    DEVIO_PIN_SET = GPIO_PIN_SET
} DEVIO_PinState;
typedef struct {    //固件库IO结构
    uint32_t CLK;
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;
} DEVIO_TypeDef;
#if defined(DEVICE_USEMACRO)
#define DEVIO_WritePin_SET(DEVIO) GPIO_SetBits((DEVIO)->GPIOx, (DEVIO)->GPIO_Pin)
#define DEVIO_WritePin_RESET(DEVIO) GPIO_ResetBits((DEVIO)->GPIOx, (DEVIO)->GPIO_Pin)
#define DEVIO_ReadPin(DEVIO) (DEVIO_PinState) GPIO_ReadInputDataBit((DEVIO)->GPIOx, (DEVIO)->GPIO_Pin)
#define DEV_CLK_GPIO_PIN_CONFI(DEVIO, GPIO_InitStructure) ({ \
    RCC_APB2PeriphClockCmd(DEVIO->CLK, ENABLE);              \
    (GPIO_InitStructure)->GPIO_Pin = DEVIO->GPIO_Pin;        \
    GPIO_Init(DEVIO->GPIOx, (GPIO_InitStructure));           \
})
#else
void DEVIO_WritePin_SET(DEVIO_TypeDef *DEVIO);
void DEVIO_WritePin_RESET(DEVIO_TypeDef *DEVIO);
DEVIO_PinState DEVIO_ReadPin(DEVIO_TypeDef *DEVIO);
void DEV_CLK_GPIO_PIN_CONFI(DEVIO_TypeDef *DEVIO, GPIO_InitTypeDef *GPIO_InitStructure);
#endif    // DEVICE_USEMACRO
#endif
#endif


/////////////////////////    通信协议实现部分    /////////////////////////
//模拟通信的句柄, 指向活动设备io操作的函数段
typedef enum {
    I2C = 1,
    SPI,
    ONEWIRE,
} DEVCMNI_TypeTypeDef;
typedef enum {
    SOFTWARE = 1,
    HARDWARE,
} DEVCMNI_DevTypeDef;
typedef struct {
    DEVIO_TypeDef SCL_SCLK;
    DEVIO_TypeDef SDA_SDO_OWIO;
    DEVIO_TypeDef CS;
} DEVCMNI_IOTypeDef;
typedef struct DEVCMNI_TypeDef {
    DEVCMNI_TypeTypeDef type;
    DEVCMNI_DevTypeDef dev;
    void *handle;
} DEVCMNI_TypeDef;

#ifdef DEVICE_I2C_SOFTWARE_ENABLED
#include "i2c.h"
#define SOFTBUS_I2C 1
extern I2C_SoftHandleTypeDef ahi2c[SOFTBUS_I2C];
#endif    // DEVICE_I2C_SOFTWARE_ENABLED
#ifdef DEVICE_SPI_SOFTWARE_ENABLED
#include "spi.h"
#define SOFTBUS_SPI 1
extern SPI_SoftHandleTypeDef ahspi[SOFTBUS_SPI];
#endif    // DEVICE_SPI_SOFTWARE_ENABLED
#ifdef DEVICE_ONEWIRE_SOFTWARE_ENABLED
#include "onewire.h"
#define SOFTBUS_ONEWIRE 1
extern ONEWIRE_SoftHandleTypeDef ahowre[SOFTBUS_ONEWIRE];
#endif    // DEVICE_ONEWIRE_SOFTWARE_ENABLED


void DEVCMNI_WriteByte(uint8_t data, uint8_t address, int8_t skip);
void DEVCMNI_Write(uint8_t *pdata, uint16_t size, uint8_t address, int8_t skip);

#endif    // !__DEVICE_H
