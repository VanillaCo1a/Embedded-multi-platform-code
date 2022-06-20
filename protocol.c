//tofix: 在F401CDU6下, OLED的I2C通信写一页快于800us, SPI快于200us会出错
//在STM32F401CDU6上测得I2C写入OLED屏幕(8*3byte+8*128byte)最短耗时: (4.88*((1+1+3)+(1+1+128))+10.15+10.15)*8 = 5432us
//进行延时后: 5432+(2*940+N*9*((1+1+3)+(1+1+128)))*8
//在STM32F401CDU6, OD模式下, OLED的I2C模拟通信页写入短于800us, SPI页写入短于200us会出错

#include "protocol.h"
__attribute__((weak)) void DEVCMNI_SCL_Set(bool dir) {}
__attribute__((weak)) void DEVCMNI_SDA_OWRE_Set(bool dir) {}
__attribute__((weak)) void DEVCMNI_SCL_SCK_Out(bool pot) {}
__attribute__((weak)) void DEVCMNI_SDA_SDI_OWRE_Out(bool pot) {}
__attribute__((weak)) bool DEVCMNI_SCL_In(void) { return HIGH; }
__attribute__((weak)) bool DEVCMNI_SDA_OWRE_In(void) { return HIGH; }
__attribute__((weak)) bool DEVCMNI_SDO_In(void) { return HIGH; }
__attribute__((weak)) void DEVCMNI_CS_Out(bool pot) {}
__attribute__((weak)) void DEVCMNI_Error(int8_t err) {}
__attribute__((weak)) void DEVCMNI_Delayus(uint16_t us) {}
__attribute__((weak)) void DEVCMNI_Delayms(uint16_t ms) {}
__attribute__((weak)) int8_t DEVCMNI_Delayus_paral(uint16_t us) { return 1; }
