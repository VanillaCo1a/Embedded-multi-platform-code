#ifndef __DEVICE_H
#define __DEVICE_H
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

#define MCU_STM32HAL 0
#define MCU_STM32FWLIBF1 1
#define MCU_STM32FWLIBF4 2
#define MCU_51 3
#define MCU_TC264 4
#define MCU_TI 5
#define MCU_ESP32 6
#define MCU_HC32 7
#define MCU_COMPILER MCU_STM32HAL    //主控芯片编译环境

#define DEV_USEMACRO 0


/////////////////////////    IO操作部分    /////////////////////////
#if MCU_COMPILER == MCU_STM32HAL
#include "stm32f4xx_hal.h"
#include "main.h"
typedef enum {
    DEVIO_PIN_RESET = GPIO_PIN_RESET,
    DEVIO_PIN_SET = GPIO_PIN_SET
} DEVIO_PinState;
typedef struct {    //HAL库IO结构
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;
} DEVIO_TypeDef;
#endif
#if MCU_COMPILER == MCU_STM32HAL
void DEVIO_WritePin_SET(DEVIO_TypeDef *DEVIO);
void DEVIO_WritePin_RESET(DEVIO_TypeDef *DEVIO);
DEVIO_PinState DEVIO_ReadPin(DEVIO_TypeDef *DEVIO);
#if DEV_USEMACRO
#define DEVIO_WritePin_SET(DEVIO) HAL_GPIO_WritePin((DEVIO)->GPIOx, (DEVIO)->GPIO_Pin, GPIO_PIN_SET)
#define DEVIO_WritePin_RESET(DEVIO) HAL_GPIO_WritePin((DEVIO)->GPIOx, (DEVIO)->GPIO_Pin, GPIO_PIN_RESET)
#define DEVIO_ReadPin(DEVIO) (DEVIO_PinState) HAL_GPIO_ReadPin((DEVIO)->GPIOx, (DEVIO)->GPIO_Pin)
#endif    // DEV_USEMACRO
#elif MCU_COMPILER == MCU_STM32FWLIBF1
#include "stm32f10x.h"
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
void DEV_CLK_GPIO_PIN_CONFI(DEVIO_TypeDef *DEVIO);
#if DEV_USEMACRO
#define DEVIO_WritePin_SET(DEVIO) GPIO_SetBits((DEVIO)->GPIOx, (DEVIO)->GPIO_Pin)
#define DEVIO_WritePin_RESET(DEVIO) GPIO_ResetBits((DEVIO)->GPIOx, (DEVIO)->GPIO_Pin)
#define DEVIO_ReadPin(DEVIO) (DEVIO_PinState) GPIO_ReadInputDataBit((DEVIO)->GPIOx, (DEVIO)->GPIO_Pin)
#define DEV_CLK_GPIO_PIN_CONFI(DEVIO)                    \
    ({                                                   \
        RCC_APB2PeriphClockCmd((DEVIO)->CLK, ENABLE);    \
        GPIO_InitStructure.GPIO_Pin = (DEVIO)->GPIO_Pin; \
        GPIO_Init((DEVIO)->GPIOx, &GPIO_InitStructure);  \
    })
#endif    // DEV_USEMACRO
#endif


/////////////////////////    通信协议实现部分    /////////////////////////
#include "i2c.h"
#include "spi.h"
#include "owre.h"
typedef enum {
    I2C = 1,
    SPI,
    OWRE,
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
typedef struct {
    I2C_AnalogHandleTypeDef hai2c;
    SPI_AnalogHandleTypeDef haspi;
    OWRE_AnalogHandleTypeDef haowre;
    void *hi2c;
    void *hspi;
    void *howre;
} DEVCMNI_HandleTypeDef;
typedef struct {
    DEVCMNI_TypeTypeDef type;
    DEVCMNI_DevTypeDef dev;
    DEVCMNI_HandleTypeDef handle;
} DEVCMNI_TypeDef;
//模拟通信的句柄, 指向活动设备io操作的函数段
extern I2C_AnalogTypeDef ai2c;
extern SPI_AnalogTypeDef aspi;
extern OWRE_AnalogTypedef aowre;
void DEVCMNI_WriteByte(uint8_t data, uint8_t address);
void DEVCMNI_WriteConti(uint8_t *pdata, uint16_t size, uint8_t address);


/////////////////////////    设备控制部分    /////////////////////////
typedef enum {
    OLED,
    MPU6050,
    DS18B20,
    OTHER,
} DEV_TypeTypeDef;
typedef enum {
    idle,
    busy
} DEV_StateTypeDef;
#define DEVPOOL_MAXNUM 100        //设备池大小
typedef uint8_t devpool_size;     //设备池大小的数据类型
#define DEVBUSYLIST_MAXNUM 20     //忙设备列表大小
typedef uint8_t busypool_size;    //忙设备列表大小的数据类型
typedef struct {                  //设备类结构体
    DEV_TypeTypeDef type;         //设备类型
    devpool_size size;            //设备数量
    devpool_size pool;            //设备池起始序号
    devpool_size stream;          //活动设备序号
} DEVS_TypeDef;
typedef struct {              //设备结构体
    busypool_size state;      //设备状态值
    void *parameter;          //设备参数配置
    void *io;                 //设备IO配置
    DEVCMNI_TypeDef *cmni;    //设备通信配置
} DEV_TypeDef;
extern DEV_TypeDef devpool[DEVPOOL_MAXNUM];
//设备初始化
int8_t DEV_Init(DEVS_TypeDef *devs, void *parameter, void *io, DEVCMNI_TypeDef *cmni);
void DEV_Error(void);
//设备io流控制
int8_t DEV_setistm(DEVS_TypeDef *self, devpool_size stream);
devpool_size DEV_getistm(DEVS_TypeDef *self);
DEV_TypeDef *DEV_getdev(DEVS_TypeDef *self);
void DEV_Do(DEVS_TypeDef *devs, void (*action)(void));
//设备状态控制
int8_t DEV_setState(DEVS_TypeDef *self, uint16_t us);
DEV_StateTypeDef DEV_getState(DEVS_TypeDef *self);

void DEV_Confi(DEVS_TypeDef *devs, void *parameter, void *io, DEVCMNI_TypeDef *cmni);
#endif    // !__DEVICE_H
