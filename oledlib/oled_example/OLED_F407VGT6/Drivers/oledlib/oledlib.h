////////////////////////////////////////////////////////////////////////////
//*         Ϊ�����������ͼ�ο⣬�Ҳο��˺ܶ��������㷨,���˴���ʱ��ȥ����ֲ
//*�Ż������вο�������github�е�arduboy��Ϸ�����󲿷�ͼ������ֲ���ģ�
//*����������ĳ���汾,���кܶ�ط���Ҫ�Ż��Ľ�������Ҫ�����ͼ�ο�����
//*��������������Ȥ�Ķ�������ģ�飬���붼������򵥵ķ�ʽ��д�ģ����ǿ�Դ�ġ�
//*����Ҳ�����ע��˵�����ø������Ե�Ƭ����oled��������Ӳ����̣������
//ʹ���˸ÿ⣬�����������bվ��עoximeterg���ɻ�ȡ�ÿ�����°汾����Ϣ��
//ע�⣺��������ʵ�ĵ�ͼ�Σ�Ӱ��ˢ��Ч�ʣ����Ż��У�
//*oled��ͼ�ο�0.0.0.1                                    ���ߣ�maoyongjie     
//https://github.com/hello-myj/stm32_oled/
//************************************************************************
//-------------------------------------------------------------------------         
/*                          x(0~127)                            //����˵��
                         ------------------>
                        |
                        |y(0~63)
                        |
                        v                                                 */
//--------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
#ifndef __OLED_LIB_H
#define __OLED_LIB_H
#define MCU_STM32FWLIB 0
#define MCU_STM32HAL 1
#define MCU_TIBOARD 2
#define MCU_ESP32 3
#define MCU_HC32 4
#define MCU_COMPILER    MCU_STM32HAL    //����оƬ���뻷��

#define MCU_ANALOG 0
#define MCU_HARDWARE 1
#define MCU_I2CCOM      MCU_HARDWARE    //����оƬI2Cͨ�ŷ�ʽ, 0��ʹ��ģ��ͨ��(Ĭ��),1Ӳ��ͨ��(����߾�ʹ��)
#define MCU_SPICOM      MCU_HARDWARE    //����оƬSPIͨ�ŷ�ʽ, 0��ʹ��ģ��ͨ��(Ĭ��),1Ӳ��ͨ��(����߾�ʹ��)

//ͼ�ο��һЩö������
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

#if (MCU_COMPILER == MCU_STM32FWLIB)
#include "stm32f10x.h"
//#include "delay.h"
#elif (MCU_COMPILER == MCU_STM32HAL)
#include "main.h"
#endif
#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"
#include "math.h"
#include "i2c.h"
#include "spi.h"
#include "oled_driver.h"
#include "oled_config.h"
#include "oled_buffer.h"
#include "oled_basic.h"
#include "oled_draw.h"
#include "oled_print.h"
#include "oled_font&bmp.h"
#endif
