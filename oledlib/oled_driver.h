#ifndef __OLED_DRIVER_H
#define	__OLED_DRIVER_H
#include "oledlib.h"

#define OLED_Communication_MODE 0	//设置OLED通信方式, 0为I2C通信, 1为SPI通信
//-----------------OLED I2C端口定义--------------
#define OLED_I2C_GPIO_L RCC_APB2Periph_GPIOB
#define OLED_SCL_GPIO_L OLED_I2C_GPIO_L
#define OLED_SDA_GPIO_L OLED_I2C_GPIO_L
#define OLED_I2C_GPIO_S GPIOB
#define OLED_SCL_GPIO_S OLED_I2C_GPIO_S
#define OLED_SDA_GPIO_S OLED_I2C_GPIO_S
#define OLED_SCL_PIN GPIO_Pin_1
#define OLED_SDA_PIN GPIO_Pin_0
//-----------------OLED SPI端口定义--------------
//#define OLED_SPI_GPIO_L RCC_APB2Periph_GPIOB
//#define OLED_SCLK_GPIO_L OLED_SPI_GPIO_L
//#define OLED_SDO_GPIO_L OLED_SPI_GPIO_L
//#define OLED_RST_GPIO_L OLED_SPI_GPIO_L
//#define OLED_DC_GPIO_L OLED_SPI_GPIO_L
//#define OLED_CS_GPIO_L OLED_SPI_GPIO_L
//#define OLED_SPI_GPIO_S GPIOB
//#define OLED_SCLK_GPIO_S OLED_SPI_GPIO_S
//#define OLED_SDO_GPIO_S OLED_SPI_GPIO_S
//#define OLED_RST_GPIO_S OLED_SPI_GPIO_S
//#define OLED_DC_GPIO_S OLED_SPI_GPIO_S
//#define OLED_CS_GPIO_S OLED_SPI_GPIO_S
//#define OLED_SCLK_PIN GPIO_Pin_9
//#define OLED_SDO_PIN GPIO_Pin_8
//#define OLED_RST_PIN GPIO_Pin_7
//#define OLED_DC_PIN GPIO_Pin_6
//#define OLED_CS_PIN GPIO_Pin_5

#define OLED_MODE_CMD  0	//写命令模式
#define OLED_MODE_DATA 1	//写数据模式
#define OLED_ADDRESS 0x3C	//修改需要通讯的从机地址, 常用的OLED默认地址为0111100

void OLED_GPIO_Init_function(void);
void OLED_Init(void);
void OLED_Write_Byte(unsigned char, unsigned char);
void OLED_Clear(void);
void OLED_On(void);
void OLED_Off(void);
void OLED_Fill(unsigned char BMP[]);
#endif

