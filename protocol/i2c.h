#ifndef __I2C_H
#define __I2C_H
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
typedef enum {
    SCL,
    SDA
} I2C_WireTypeDef;
typedef enum {
    I2CLOW,
    I2CMEDIUM,
    I2CHIGH
} I2C_SpeedTypeDef;
typedef struct {
    void (*SCL_Set)(Direct_TypeDef);
    void (*SDA_Set)(Direct_TypeDef);
    void (*SCL_Out)(Potential_TypeDef);
    void (*SDA_Out)(Potential_TypeDef);
    Potential_TypeDef (*SDA_In)(void);
    void (*delayus)(uint16_t us);
    void (*delayms)(uint16_t ms);
} I2C_AnalogTypeDef;
typedef struct {
    uint8_t addr;
    I2C_AnalogTypeDef *method;
} I2C_AnalogHandleTypeDef;

int8_t MODULAR_I2C_WriteByte(I2C_AnalogHandleTypeDef *, uint8_t, uint32_t, int8_t);
int8_t MODULAR_I2C_Write(I2C_AnalogHandleTypeDef *, uint8_t, uint8_t *, uint16_t, uint32_t, int8_t);
int8_t MODULAR_I2C_ReadByte(I2C_AnalogHandleTypeDef *, uint8_t, uint32_t, int8_t);
int8_t MODULAR_I2C_Read(I2C_AnalogHandleTypeDef *, uint8_t, uint8_t *, uint16_t, uint32_t, int8_t);
#endif
