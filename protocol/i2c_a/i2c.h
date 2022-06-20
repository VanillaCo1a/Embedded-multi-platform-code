#ifndef __I2C_H
#define __I2C_H
#include "stdint.h"
typedef struct {
    uint8_t ADDRESS;
    void (*i2cSCLSet)(int8_t);
    void (*i2cSDASet)(int8_t);
    void (*i2cSCLOut)(uint8_t);
    void (*i2cSDAOut)(uint8_t);
    uint8_t (*i2cSDAIn)(void);
    void (*delayus)(int16_t us);
}I2c;

#ifndef ENUM_Potential
#define ENUM_Potential
enum Potential{LOW, HIGH};
#endif
#ifndef ENUM_Direct
#define ENUM_Direct
enum Direct{IN, OUT};
#endif
enum I2CWIRE{SCL, SDA};
enum I2CSPEED{I2CLOW, I2CMEDIUM, I2CHIGH};

int8_t MODULAR_I2CWriteByte(I2c, int8_t, int8_t, uint8_t, uint8_t);
#endif
