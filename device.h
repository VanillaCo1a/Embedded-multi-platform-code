#ifndef __DEVICE_H
#define __DEVICE_H
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
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
#define STM32REGISTER    //是否优先使用寄存器版本
#endif
#ifdef STC89C
// #define STC89C51
#define STC89C52
#endif

#define DEVICE_I2C_SOFTWARE_ENABLED
#define DEVICE_SPI_SOFTWARE_ENABLED
#define DEVICE_ONEWIRE_SOFTWARE_ENABLED
// #define I2CBUS_USEPOINTER
// #define SPIBUS_USEPOINTER
// #define OWREBUS_USEPOINTER
#define I2C_SOFTBUS_NUM 1
#define SPI_SOFTBUS_NUM 1
#define ONEWIRE_SOFTBUS_NUM 1
#define BUS_NUM ((I2C_SOFTBUS_NUM + SPI_SOFTBUS_NUM + ONEWIRE_SOFTBUS_NUM) << 1)

/////////////////////////    设备控制部分    /////////////////////////
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
typedef enum {
    DEVUNDEF = 0,
    BUS,
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
void DEV_ReCall(DEVS_TypeDef *devs, void (*action)(void));
void DEV_Error(void);
void DEV_Confi(DEVS_TypeDef *devs, DEV_TypeDef dev[]);


/////////////////////////    IO操作部分    /////////////////////////
//todo: 考虑能否模仿hal库, 改为使用assert_param进行有效性判断
#if defined(STM32)
#if defined(STM32HAL)
typedef enum {
    DEVIO_PIN_RESET = GPIO_PIN_RESET,
    DEVIO_PIN_SET = GPIO_PIN_SET
} DEVIO_PinState;
typedef struct {    //HAL库IO结构
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;
} DEVIO_TypeDef;
void DEVIO_WritePin_SET(DEVIO_TypeDef *DEVIO);
void DEVIO_WritePin_RESET(DEVIO_TypeDef *DEVIO);
void DEVIO_WritePin(DEVIO_TypeDef *DEVIO, DEVIO_PinState PinState);
DEVIO_PinState DEVIO_ReadPin(DEVIO_TypeDef *DEVIO);
void DEV_CLK_GPIO_PIN_CONFI(DEVIO_TypeDef *DEVIO, GPIO_InitTypeDef *GPIO_InitStructure);
bool DEVIO_NULL(DEVIO_TypeDef *DEVIO);
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
void DEVIO_WritePin_SET(DEVIO_TypeDef *DEVIO);
void DEVIO_WritePin_RESET(DEVIO_TypeDef *DEVIO);
DEVIO_PinState DEVIO_ReadPin(DEVIO_TypeDef *DEVIO);
void DEV_CLK_GPIO_PIN_CONFI(DEVIO_TypeDef *DEVIO, GPIO_InitTypeDef *GPIO_InitStructure);
bool DEVIO_NULL(DEVIO_TypeDef *DEVIO);
#endif
#endif


/////////////////////////    通信协议实现部分    /////////////////////////
//模拟通信的句柄, 指向活动设备io操作的函数段
#include "protocol.h"
typedef enum {
    I2C = 1,
    SPI,
    ONEWIRE,
} DEVCMNI_ProtocolTypeDef;
typedef enum {
    SOFTWARE = 1,
    HARDWARE,
} DEVCMNI_WareTypeDef;
typedef struct {
    DEVIO_TypeDef SCL_SCK;
    DEVIO_TypeDef SDA_SDI_OWRE;
    DEVIO_TypeDef SDO;
    DEVIO_TypeDef CS;
} DEVCMNIIO_TypeDef;
typedef struct DEVCMNI_TypeDef {
    DEVCMNI_ProtocolTypeDef protocol;
    DEVCMNI_WareTypeDef ware;
    void *handle;
} DEVCMNI_TypeDef;
extern I2C_SoftHandleTypeDef ahi2c[I2C_SOFTBUS_NUM];
extern SPI_SoftHandleTypeDef ahspi[SPI_SOFTBUS_NUM];
extern ONEWIRE_SoftHandleTypeDef ahowre[ONEWIRE_SOFTBUS_NUM];
void DEVCMNI_WriteByte(uint8_t data, uint8_t address, bool skip);
uint8_t DEVCMNI_ReadByte(uint8_t address, bool skip);
bool DEVCMNI_ReadBit(uint8_t address, bool skip);
void DEVCMNI_Write(uint8_t *pdata, uint16_t size, uint8_t address, bool skip);
void DEVCMNI_Read(uint8_t *pdata, uint16_t size, uint8_t address, bool skip);
#endif    // !__DEVICE_H
