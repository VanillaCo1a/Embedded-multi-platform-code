#ifndef __DS18B20_H
#define __DS18B20_H
#include "device.h"

typedef enum {
    PARASITIC,
    EXTERNAL,
} DS18B20_POWMDTypedef;

typedef struct {
    DEVCMNIIO_TypeDef cmniio;
} DS18B20_IOTypedef;
typedef struct {
    DS18B20_POWMDTypedef powermode;
} DS18B20_PARTypeDef;

typedef struct {
    uint8_t lsb;
    uint8_t msb;
    uint8_t tl;
    uint8_t th;
    uint8_t conf;
    uint8_t resvd0;
    uint8_t resvd1;
    uint8_t resvd2;
    uint8_t crc;
} DS18B20_SCRTypedef;

void DS18B20_Init(DEVS_TypeDef *devs, DEV_TypeDef dev[], poolsize devSize,
                  int16_t devTemperature[], int8_t devState[]);
void DS18B20_Deinit(DEVS_TypeDef *devs, DEV_TypeDef dev[], poolsize devSize);
int8_t DS18B20_SetTemperature(poolsize num);
int16_t DS18B20_GetTemperature(poolsize num);
void DS18B20_ClearTemperature(poolsize num);

#endif
