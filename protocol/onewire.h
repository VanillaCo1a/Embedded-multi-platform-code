#ifndef __1_WIRE_H
#define __1_WIRE_H
#include "stdint.h"
#include "stdlib.h"
#ifndef ENUM_Potential
#define ENUM_Potential
enum Potential{LOW, HIGH};
#endif
#ifndef ENUM_Direct
#define ENUM_Direct
enum Direct{IN, OUT};
#endif
typedef struct {
    void (*onewireIOSet)(int8_t);
    void (*onewireIOOut)(uint8_t);
    uint8_t (*onewireIOIn)(void);
    void (*delayus)(uint16_t us);
    void (*delayms)(uint16_t ms);
} Onewire_AnalogTypedef;

void ONEWIRE_Init(Onewire_AnalogTypedef *);
uint8_t ONEWIRE_Wait(Onewire_AnalogTypedef *);
uint8_t ONEWIRE_Read_Bit(Onewire_AnalogTypedef *);
uint8_t ONEWIRE_Read_Byte(Onewire_AnalogTypedef *);
void ONEWIRE_Write_Byte(Onewire_AnalogTypedef *, int8_t);

#endif
