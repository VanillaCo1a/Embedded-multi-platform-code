#ifndef __OLED_DRIVER_H
#define __OLED_DRIVER_H
#include "oledlib.h"

#define SSD1306 0
#define SH1106 1
#define MODE_SPI 0
#define MODE_I2C 1
#define MODE_I2C_7PIN 2

#define OLED_CHIP SSD1306       //设置OLED使用芯片, 两型号芯片在配置上略有不同
#define OLED_MODE MODE_I2C      //设置OLED通信及引脚, 依次为7脚SPI, 4脚I2C, 7脚I2C
#if (OLED_MODE==MODE_I2C || OLED_MODE==MODE_I2C_7PIN)
#define OLED_ADDRESS 0x3C       //修改I2C从机地址, 常用的OLED地址为0111100和0111101
#endif

#if (OLED_MODE==MODE_I2C || OLED_MODE==MODE_I2C_7PIN)
//-----------------OLED I2C端口定义--------------
#define OLED_SCL_CLK RCC_APB2Periph_GPIOB       //SCL
#define OLED_SCL_GPIO GPIOB
#define OLED_SCL_PIN GPIO_Pin_1
#define OLED_SDA_CLK RCC_APB2Periph_GPIOB       //SDA
#define OLED_SDA_GPIO GPIOB
#define OLED_SDA_PIN GPIO_Pin_0
#if (OLED_MODE == MODE_I2C_7PIN)
#define OLED_RST_CLK RCC_APB2Periph_GPIOB       //RST(RES)
#define OLED_RST_GPIO GPIOB
#define OLED_RST_PIN GPIO_Pin_8
#define OLED_DC_CLK RCC_APB2Periph_GPIOB        //DC
#define OLED_DC_GPIO GPIOB
#define OLED_DC_PIN GPIO_Pin_7
#define OLED_CS_CLK RCC_APB2Periph_GPIOB        //CS
#define OLED_CS_GPIO GPIOB
#define OLED_CS_PIN GPIO_Pin_9
#endif
#else
//-----------------OLED SPI端口定义--------------
#define OLED_SCLK_CLK RCC_APB2Periph_GPIOB      //SCLK(D0)
#define OLED_SCLK_GPIO GPIOB
#define OLED_SCLK_PIN GPIO_Pin_1
#define OLED_SDO_CLK RCC_APB2Periph_GPIOB       //SDO(D1)
#define OLED_SDO_GPIO GPIOB
#define OLED_SDO_PIN GPIO_Pin_0
#define OLED_RST_CLK RCC_APB2Periph_GPIOB       //RST(RES)
#define OLED_RST_GPIO GPIOB
#define OLED_RST_PIN GPIO_Pin_8
#define OLED_DC_CLK RCC_APB2Periph_GPIOB        //DC
#define OLED_DC_GPIO GPIOB
#define OLED_DC_PIN GPIO_Pin_7
#define OLED_CS_CLK RCC_APB2Periph_GPIOB        //CS
#define OLED_CS_GPIO GPIOB
#define OLED_CS_PIN GPIO_Pin_9
#endif

void OLED_Confi(void);
void OLED_Clear(void);
void OLED_On(void);
void OLED_Off(void);
void OLED_Fill(unsigned char *);
void OLED_Delayms(uint16_t);

#endif
