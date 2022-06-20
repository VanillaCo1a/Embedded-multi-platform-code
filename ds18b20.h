#ifndef __DS18B20_H
#define __DS18B20_H
#include "onewire.h"
#include "main.h"
#define MCU_STM32FWLIB 0
#define MCU_STM32HAL 1
#define MCU_TIBOARD 2
#define MCU_ESP32 3
#define MCU_HC32 4
#define MCU_COMPILER    MCU_STM32HAL    //主控芯片编译环境

#define DS18B20_NUM 1
typedef struct {
    #if (MCU_COMPILER == MCU_STM32FWLIB)    //固件库IO结构
    uint32_t CLK_io;
    GPIO_TypeDef *GPIO_io;
    uint16_t PIN_io;
    #elif (MCU_COMPILER == MCU_STM32HAL)    //HAL库IO结构
    GPIO_TypeDef *GPIO_io;
    uint16_t PIN_io;
    #endif
}Ds18b20io_Typedef;
typedef struct {
    void *communication_handle; //通信句柄
    Ds18b20io_Typedef *Ds18b20io;
}Ds18b20_Typedef;

void setTemperature(int16_t);
int16_t getTemperature(void);
void DS18B20_Ctrl(void);
void DS18B20_Confi(void);
void DS18B20_StartTem(void); 
int16_t DS18B20_GetTem(void);
#endif
