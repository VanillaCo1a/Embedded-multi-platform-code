#include "onewire.h"
__attribute__((weak)) void DEVCMNI_OWIO_Set(Direct_TypeDef dir) {}
__attribute__((weak)) void DEVCMNI_OWIO_Out(Potential_TypeDef pot) {}
__attribute__((weak)) Potential_TypeDef DEVCMNI_OWIO_In(void) { return HIGH; }
__attribute__((weak)) void DEVCMNI_Error(void) {}
__attribute__((weak)) void DEVCMNI_Delayus(uint16_t us) {}
__attribute__((weak)) void DEVCMNI_Delayms(uint16_t ms) {}
