#ifndef __OLED_DRIVER_H
#define __OLED_DRIVER_H
#include "oledlib.h"
#define MCU_ANALOG 0
#define MCU_BOTH 1
#define MCU_I2CCOM      MCU_ANALOG      //主控芯片I2C通信方式, 0仅使用模拟通信(默认),1硬件通信(或二者均使用)
#define MCU_SPICOM      MCU_BOTH        //主控芯片SPI通信方式, 0仅使用模拟通信(默认),1硬件通信(或二者均使用)

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
    DEVIO_TypeDef SCL_SCLK;
    DEVIO_TypeDef SDA_SDO;
    DEVIO_TypeDef RST;
    DEVIO_TypeDef DC;
    DEVIO_TypeDef CS;
}OLEDIO_TypeDef;
typedef struct {
    OledProp_Type oledprop;
    OledComi_Type oledcomi;
    OledChip_Type oledchip;
    uint8_t oledaddr;
    void *communication_handle; //通信句柄
    OLEDIO_TypeDef *oledio;
}Oled_Typedef;

extern DEV_TypeDef oled;

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
