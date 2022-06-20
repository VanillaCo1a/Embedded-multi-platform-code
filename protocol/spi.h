#ifndef __SPI_H
#define __SPI_H
#include "stdint.h"
typedef struct {
	void (*spiCSOut)(uint8_t);
	void (*spiSCLKOut)(uint8_t);
	void (*spiSDOOut)(uint8_t);
	void (*delayus)(int16_t us);
}Spi;

#ifndef ENUM_Potential
#define ENUM_Potential
enum Potential{LOW, HIGH};
#endif
#ifndef ENUM_Direct
#define ENUM_Direct
enum Direct{IN, OUT};
#endif

void MODULAR_SPIWriteByte(Spi, uint8_t);
#endif
