#ifndef __SPI_H
#define __SPI_H
#include "stdint.h"
#ifndef ENUM_Potential
#define ENUM_Potential
typedef enum {
    LOW,
    HIGH
} Potential_TypeDef;
#endif // !ENUM_Potential
#ifndef ENUM_Direct
#define ENUM_Direct
typedef enum {
    IN,
    OUT
} Direct_TypeDef;
#endif // !ENUM_Direct
typedef struct {
    void (*SCLK_Out)(uint8_t);
    void (*SDO_Out)(uint8_t);
    void (*CS_Out)(uint8_t);
    void (*delayus)(uint16_t us);
    void (*delayms)(uint16_t ms);
} SPI_AnalogTypeDef;
typedef struct {
    SPI_AnalogTypeDef *method;
} SPI_AnalogHandleTypeDef;

void MODULAR_SPI_WriteByte(SPI_AnalogHandleTypeDef *, uint8_t);
void MODULAR_SPI_Write(SPI_AnalogHandleTypeDef *, uint8_t *, uint16_t);
#endif
