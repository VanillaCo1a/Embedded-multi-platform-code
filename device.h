#ifndef __DEVICE_H
#define __DEVICE_H
#include "stdint.h"
#include "stddef.h"

#define DEV_MAXNUM 10       //设备数量上限
typedef uint8_t devsize;    //设备数量类型
typedef enum {
    idle,
    busy
} DEV_StateTypeDef;
typedef struct {
    devsize _num;                  //设备总数量
    devsize _stream;               //当前io流设备序号
    uint8_t _state[DEV_MAXNUM];    //设备状态
} DEV_TypeDef;
//设备初始化
int8_t DEV_Init(DEV_TypeDef *self, uint8_t num);
//设备io流控制
int8_t DEV_setiostream(DEV_TypeDef *self, uint8_t num);
uint8_t DEV_getiostream(DEV_TypeDef *self);
//设备状态控制
int8_t DEV_setState(DEV_TypeDef *self, uint16_t us);
DEV_StateTypeDef DEV_getState(DEV_TypeDef *self);

/////////////////////////       IO操作部分          ///////////////////////////////////////
#define MCU_STM32HAL 0
#define MCU_STM32FWLIBF1 1
#define MCU_STM32FWLIBF4 2
#define MCU_51 3
#define MCU_TC264 4
#define MCU_TI 5
#define MCU_ESP32 6
#define MCU_HC32 7
#define MCU_COMPILER MCU_STM32HAL    //主控芯片编译环境

#if(MCU_COMPILER == MCU_STM32HAL)    //HAL库IO结构
#include "stm32f4xx_hal.h"
typedef struct {
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;
} DEVIO_TypeDef;
#define DEV_GPIO_WritePin(DEV, DEV_PinState) DEV_GPIO_WritePin_##DEV_PinState##(DEV.GPIOx, DEV.GPIO_Pin)
#define DEV_GPIO_WritePin_SET(DEV) HAL_GPIO_WritePin(DEV.GPIOx, DEV.GPIO_Pin, GPIO_PIN_SET)
#define DEV_GPIO_WritePin_RESET(DEV) HAL_GPIO_WritePin(DEV.GPIOx, DEV.GPIO_Pin, GPIO_PIN_RESET)
#define DEV_GPIO_ReadPin(DEV) HAL_GPIO_ReadPin(DEV.GPIOx, DEV.GPIO_Pin)

#elif(MCU_COMPILER == MCU_STM32FWLIBF1)    //固件库IO结构
#include "stm32f10x.h"
typedef struct {
    uint32_t CLK;
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;
} DEVIO_TypeDef;
#define DEV_GPIO_WritePin(DEV, PinState) DEV_GPIO_WritePin_##PinState##(DEV.GPIOx, DEV.GPIO_Pin)
#define DEV_GPIO_WritePin_SET(DEV) GPIO_SetBits(DEV.GPIOx, DEV.GPIO_Pin)
#define DEV_GPIO_WritePin_RESET(DEV) GPIO_ResetBits(DEV.GPIOx, DEV.GPIO_Pin)
#define DEV_GPIO_ReadPin(DEV) GPIO_ReadInputDataBit(DEV.GPIOx, DEV.GPIO_Pin)
#define DEV_CLK_GPIO_PIN_CONFI(DEV)                 \
    {                                               \
        RCC_APB2PeriphClockCmd(DEV.CLK, ENABLE);    \
        GPIO_InitStructure.GPIO_Pin = DEV.GPIO_Pin; \
        GPIO_Init(DEV.GPIOx, &GPIO_InitStructure);  \
    }
#endif

#endif    // !__DEVICE_H
