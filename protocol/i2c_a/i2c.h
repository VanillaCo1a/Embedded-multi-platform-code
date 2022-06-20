#ifndef __I2C_H
#define __I2C_H
#include "stdint.h"
typedef enum {
    SCL, SDA
}I2Cwire_Type;
enum I2CSPEED{I2CLOW, I2CMEDIUM, I2CHIGH};
#ifndef ENUM_Potential
#define ENUM_Potential
enum Potential{LOW, HIGH};
#endif
#ifndef ENUM_Direct
#define ENUM_Direct
enum Direct{IN, OUT};
#endif
typedef struct {
    uint8_t ADDRESS;
    void (*i2cSCLSet)(int8_t);
    void (*i2cSDASet)(int8_t);
    void (*i2cSCLOut)(uint8_t);
    void (*i2cSDAOut)(uint8_t);
    uint8_t (*i2cSDAIn)(void);
    void (*delayus)(uint16_t us);
}I2C_AnalogTypedef;

int8_t MODULAR_I2CWriteByte(I2C_AnalogTypedef *, uint8_t, uint32_t, int8_t);
int8_t MODULAR_I2CWrite(I2C_AnalogTypedef *, uint8_t, uint8_t *, uint16_t, uint32_t, int8_t);
int8_t MODULAR_I2CReadByte(I2C_AnalogTypedef *, uint8_t, uint32_t, int8_t);
int8_t MODULAR_I2CRead(I2C_AnalogTypedef *, uint8_t, uint8_t *, uint16_t, uint32_t, int8_t);
#endif
