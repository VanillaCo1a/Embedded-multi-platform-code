#ifndef __SPI_H
#define __SPI_H
#include "stdint.h"
#ifndef ENUM_Potential
#define ENUM_Potential
enum Potential{LOW, HIGH};
#endif
#ifndef ENUM_Direct
#define ENUM_Direct
enum Direct{IN, OUT};
#endif
typedef struct {
    void (*spiSCLKOut)(uint8_t);
    void (*spiSDOOut)(uint8_t);
    void (*spiCSOut)(uint8_t);
    void (*delayus)(int16_t us);
}SPI_AnalogTypedef;

void MODULAR_SPIWriteByte(SPI_AnalogTypedef *, uint8_t);
void MODULAR_SPIWrite(SPI_AnalogTypedef *, uint8_t *, uint16_t);
#endif
