#include "spi.h"
__attribute__((weak)) void DEVCMNI_SCLK_Out(Potential_TypeDef pot) {}
__attribute__((weak)) void DEVCMNI_SDO_Out(Potential_TypeDef pot) {}
__attribute__((weak)) void DEVCMNI_CS_Out(Potential_TypeDef pot) {}
__attribute__((weak)) void DEVCMNI_Error(void) {}
__attribute__((weak)) void DEVCMNI_Delayus(uint16_t us) {}
__attribute__((weak)) void DEVCMNI_Delayms(uint16_t ms) {}
