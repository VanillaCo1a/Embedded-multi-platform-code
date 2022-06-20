#include "i2c.h"
__attribute__((weak)) void DEVCMNI_SCL_Set(Direct_TypeDef dir) {}
__attribute__((weak)) void DEVCMNI_SDA_Set(Direct_TypeDef dir) {}
__attribute__((weak)) void DEVCMNI_SCL_Out(Potential_TypeDef pot) {}
__attribute__((weak)) void DEVCMNI_SDA_Out(Potential_TypeDef pot) {}
__attribute__((weak)) Potential_TypeDef DEVCMNI_SDA_In(void) { return HIGH; }
__attribute__((weak)) void DEVCMNI_Error(void) {}
__attribute__((weak)) void DEVCMNI_Delayus(uint16_t us) {}
__attribute__((weak)) void DEVCMNI_Delayms(uint16_t ms) {}

//在F401CDU6下, OLED的通信太快会出错
