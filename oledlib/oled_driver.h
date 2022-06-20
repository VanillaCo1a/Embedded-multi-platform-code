#ifndef __OLED_DRIVER_H
#define __OLED_DRIVER_H
#include "oledlib.h"

#if (MCU_COMPILER == MCU_STM32FWLIB)   //固件库引脚定义
    #if (OLED_TYPE==OLED_I2C4PIN || OLED_TYPE==OLED_I2C7PIN)
    //-----------------OLED I2C地址设置--------------
    #define OLED_ADDRESS 0x3C   //常用的OLED地址为0111100和0111101
    //-----------------OLED I2C端口定义--------------
    #define OLED_SCL_CLK RCC_APB2Periph_GPIOB       //SCL
    #define OLED_SCL_GPIO GPIOB
    #define OLED_SCL_PIN GPIO_Pin_1
    #define OLED_SDA_CLK RCC_APB2Periph_GPIOB       //SDA
    #define OLED_SDA_GPIO GPIOB
    #define OLED_SDA_PIN GPIO_Pin_0
    #if (OLED_TYPE == OLED_I2C7PIN)
    #define OLED_RST_CLK RCC_APB2Periph_GPIOB       //RST(RES)
    #define OLED_RST_GPIO GPIOB
    #define OLED_RST_PIN GPIO_Pin_7
    #define OLED_DC_CLK RCC_APB2Periph_GPIOB        //DC
    #define OLED_DC_GPIO GPIOB
    #define OLED_DC_PIN GPIO_Pin_8
    #define OLED_CS_CLK RCC_APB2Periph_GPIOB        //CS
    #define OLED_CS_GPIO GPIOB
    #define OLED_CS_PIN GPIO_Pin_9
    #endif
    #elif (OLED_TYPE == OLED_SPI7PIN)
    //-----------------OLED SPI端口定义--------------
    #define OLED_SCLK_CLK RCC_APB2Periph_GPIOB      //SCLK(D0)
    #define OLED_SCLK_GPIO GPIOB
    #define OLED_SCLK_PIN GPIO_Pin_1
    #define OLED_SDO_CLK RCC_APB2Periph_GPIOB       //SDO(D1)
    #define OLED_SDO_GPIO GPIOB
    #define OLED_SDO_PIN GPIO_Pin_0
    #define OLED_RST_CLK RCC_APB2Periph_GPIOB       //RST(RES)
    #define OLED_RST_GPIO GPIOB
    #define OLED_RST_PIN GPIO_Pin_7
    #define OLED_DC_CLK RCC_APB2Periph_GPIOB        //DC
    #define OLED_DC_GPIO GPIOB
    #define OLED_DC_PIN GPIO_Pin_8
    #define OLED_CS_CLK RCC_APB2Periph_GPIOB        //CS
    #define OLED_CS_GPIO GPIOB
    #define OLED_CS_PIN GPIO_Pin_9
    #endif
#elif (MCU_COMPILER == MCU_STM32HAL)       //HAL库引脚定义及配置于main文件中
    #if (OLED_TYPE==OLED_I2C4PIN || OLED_TYPE==OLED_I2C7PIN)
    //-----------------OLED I2C地址设置--------------
    #define OLED_ADDRESS 0x3C   //常用的OLED地址为0111100和0111101
    //-----------------OLED I2C端口定义--------------        
    #define OLED_SCL_GPIO OLED_SCL_SCLK_GPIO_Port   //SCL
    #define OLED_SCL_PIN OLED_SCL_SCLK_Pin 
    #define OLED_SDA_GPIO OLED_SDA_SDO_GPIO_Port    //SDA
    #define OLED_SDA_PIN OLED_SDA_SDO_Pin
    #if (OLED_TYPE == OLED_I2C7PIN)
    #define OLED_RST_GPIO OLED_RST_GPIO_Port        //RST(RES)
    #define OLED_RST_PIN OLED_RST_Pin
    #define OLED_DC_GPIO OLED_DC_GPIO_Port          //DC
    #define OLED_DC_PIN OLED_DC_Pin 
    #define OLED_CS_GPIO OLED_CS_GPIO_Port          //CS
    #define OLED_CS_PIN OLED_CS_Pin
    #endif
    #elif (OLED_TYPE == OLED_SPI7PIN)
    //-----------------OLED SPI端口定义--------------
    #define OLED_SCLK_GPIO OLED_SCL_SCLK_GPIO_Port  //SCLK(D0)
    #define OLED_SCLK_PIN OLED_SCL_SCLK_Pin
    #define OLED_SDO_GPIO OLED_SDA_SDO_GPIO_Port    //SDO(D1)
    #define OLED_SDO_PIN OLED_SDA_SDO_Pin
    #define OLED_RST_GPIO OLED_RST_GPIO_Port        //RST(RES)
    #define OLED_RST_PIN OLED_RST_Pin 
    #define OLED_DC_GPIO OLED_DC_GPIO_Port          //DC
    #define OLED_DC_PIN OLED_DC_Pin
    #define OLED_CS_GPIO OLED_CS_GPIO_Port          //CS
    #define OLED_CS_PIN OLED_CS_Pin
    #endif
#endif

void OLED_Confi(void);
void OLED_On(void);
void OLED_Off(void);
void OLED_fillScreen(uint8_t*);
void OLED_directByte(uint8_t, uint8_t, uint8_t);
void OLED_clearScreen(void);
void OLED_delayms(uint16_t);

#endif
