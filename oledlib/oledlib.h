////////////////////////////////////////////////////////////////////////////
//*         为了做这个绘制图形库，我参考了很多其他的算法,花了大量时间去做移植
//*优化，其中参考最多的是github中的arduboy游戏机，大部分图形是移植它的，
//*现在是最初的初稿版本,还有很多地方需要优化改进。我想要将这个图形库做大，
//*后续会加入更多有趣的东西，和模块，代码都是用最简单的方式编写的，都是开源的。
//*后续也会加上注释说明，让更多人以单片机和oled屏来入门硬件编程，如果你
//使用了该库，请标明出处。b站关注oximeterg，可获取该库的最新版本和消息。
//注意：绘制填充或实心的图形，影响刷新效率（待优化中）
//*oled屏图形库0.0.0.1                                    作者：maoyongjie     
//https://github.com/hello-myj/stm32_oled/
//************************************************************************
//-------------------------------------------------------------------------         
/*                          x(0~127)                            //坐标说明
                         ------------------>
                        |
                        |y(0~63)
                        |
                        v                                                 */
//--------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
#ifndef __OLED_LIB_H
#define __OLED_LIB_H

#define I2C_4PIN 0
#define I2C_7PIN 1
#define SPI_7PIN 2
#define OLED_TYPE       I2C_4PIN        //OLED的引脚数量及通信方式, 依次为4脚I2C,7脚I2C,7脚SPI

#define SSD1306 0
#define SH1106 1
#define OLED_CHIP       SSD1306         //OLED芯片型号, 不同型号的芯片在寄存器配置上略有不同

#define STM32FWLIB 0
#define STM32HAL 1
#define TIBOARD 2
#define ESP32 3
#define HC32 4
#define MCU_COMPILER    STM32HAL        //主控芯片编译环境

#define HARDWARE 0
#define ANALOG 1
#define MCU_COMMUNICA   HARDWARE        //主控芯片通信方式, 0硬件通信,1模拟通信

//图形库的一些枚举类型
typedef enum {
    TEXT_BK_NULL = 0,
    TEXT_BK_NOT_NULL,
}Type_textbk;
typedef enum {
    pix_black,
    pix_white,
    //............
}Type_color;
typedef struct COORDINATE {
    int x;
    int y;
}TypeXY;
typedef struct ROATE {
    TypeXY center;
    float angle;
    int direct;
}TypeRoate;

#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"
#include "math.h"

#if (MCU_COMPILER == STM32HAL)
#include "main.h"
#elif (MCU_COMPILER == STM32FWLIB)
#include "stm32f10x.h"
#include "delay.h"
#endif
#if ((OLED_TYPE==I2C_4PIN||OLED_TYPE==I2C_7PIN) && MCU_COMMUNICA==ANALOG)
#include "i2c.h"
#elif (OLED_TYPE==SPI_7PIN && MCU_COMMUNICA==ANALOG)
#include "spi.h"
#endif
#include "oled_driver.h"
#include "oled_config.h"
#include "oled_buffer.h"
#include "oled_basic.h"
#include "oled_draw.h"
#include "oled_print.h"
#include "oled_font&bmp.h"

#endif
