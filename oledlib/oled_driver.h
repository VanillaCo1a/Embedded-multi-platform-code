#ifndef __OLED_DRIVER_H
#define __OLED_DRIVER_H
#include "oledlib.h"
#define MCU_STM32FWLIB 0
#define MCU_STM32HAL 1
#define MCU_TIBOARD 2
#define MCU_ESP32 3
#define MCU_HC32 4
#define MCU_COMPILER    MCU_STM32HAL    //主控芯片编译环境

#define MCU_ANALOG 0
#define MCU_HARDWARE 1
#define MCU_I2CCOM      MCU_ANALOG      //主控芯片I2C通信方式, 0仅使用模拟通信(默认),1硬件通信(或二者均使用)
#define MCU_SPICOM      MCU_ANALOG      //主控芯片SPI通信方式, 0仅使用模拟通信(默认),1硬件通信(或二者均使用)

#define OLED_NUM 1
#define OLED_I2CADDR1   0x3C            //常用的OLED地址为0111100和0111101
#define OLED_I2CADDR2   0x3D
typedef enum {
    OLED_I2C, OLED_SPI
}OledProp_Type;
typedef enum {
    OLED_ANALOG, OLED_HARDWARE
}OledComi_Type;
typedef enum {
    OLED_SSD1306, OLED_SH1106
}OledChip_Type;
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
}Oledio_Typedef;
typedef struct {
    OledProp_Type oledprop;
    OledComi_Type oledcomi;
    OledChip_Type oledchip;
    uint8_t oledaddr;
    void *communication_handle; //通信句柄
    Oledio_Typedef *oledio;
}Oled_Typedef;

void OLED_setOutputStream(int8_t);
Oled_Typedef OLED_getOutputStream(void);
int8_t OLED_getOutputNum(void);
void OLED_Confi(void);
void OLED_Init(int8_t);
void OLED_moduleOn(void);
void OLED_moduleOff(void);
void OLED_moduleFlip(int8_t, int8_t);
void OLED_fillScreen(uint8_t*);
void OLED_directByte(uint8_t, uint8_t, uint8_t);
void OLED_clearScreen(void);
void OLED_delayms(uint16_t);

#endif
