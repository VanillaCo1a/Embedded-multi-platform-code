#ifndef __AT24C02_H
#define __AT24C02_H
#include "main.h"
#define MCU_STM32FWLIB 0
#define MCU_STM32HAL 1
#define MCU_TIBOARD 2
#define MCU_ESP32 3
#define MCU_HC32 4
#define MCU_COMPILER    MCU_STM32HAL  //主控芯片编译环境
#define AT24C02_NUM 1
#define AT24C02_I2CADDR1 0X57

typedef enum {
    AT24C02_I2C, AT24C02_ONEWIRE
}At24c02Prop_Type;
typedef enum {
    AT24C02_ANALOG, AT24C02_HARDWARE
}At24c02Comi_Type;
typedef struct {
    #if (MCU_COMPILER == MCU_STM32FWLIB)    //固件库IO结构
    uint32_t CLK_scl_sclk;
    GPIO_TypeDef *GPIO_scl_sclk;
    uint16_t PIN_scl_sclk;
    uint32_t CLK_sda_sdo;
    GPIO_TypeDef *GPIO_sda_sdo;
    uint16_t PIN_sda_sdo;
    uint32_t CLK_rst;
    GPIO_TypeDef *GPIO_rst;
    uint16_t PIN_rst;
    uint32_t CLK_dc;
    GPIO_TypeDef *GPIO_dc;
    uint16_t PIN_dc;
    uint32_t CLK_cs;
    GPIO_TypeDef *GPIO_cs;
    uint16_t PIN_cs;
    #elif (MCU_COMPILER == MCU_STM32HAL)    //HAL库IO结构
    GPIO_TypeDef *GPIO_scl_sclk;
    uint16_t PIN_scl_sclk;
    GPIO_TypeDef *GPIO_sda_sdo;
    uint16_t PIN_sda_sdo;
    GPIO_TypeDef *GPIO_rst;
    uint16_t PIN_rst;
    GPIO_TypeDef *GPIO_dc;
    uint16_t PIN_dc;
    GPIO_TypeDef *GPIO_cs;
    uint16_t PIN_cs;
    #endif
}At24c02io_Typedef;
typedef struct {
    At24c02Prop_Type at24c02prop;
    At24c02Comi_Type at24c02comi;
    uint8_t at24c02addr;
    void *communication_handle; //通信句柄
    At24c02io_Typedef *at24c02io;
}At24c02_Typedef;

void AT24C02_Confi(void);
void *AT24C02_ReadOrWrite(void *, uint16_t, uint8_t, int8_t);
void AT24C02_GPIO_Init(void);
#endif
