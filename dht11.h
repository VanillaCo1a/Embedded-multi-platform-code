#ifndef __DHT11_H
#define __DHT11_H
#include "device.h"

/* MODULE STRUCTURE DEFINITION & FUNCTION DECLARAION OF DHT11 DEVICE COMMUNITCATION */
typedef struct {
    DEVCMNI_TypeDef cmni;    //设备通信基类
} DHT11_ModuleHandleTypeDef;

/* SOFTWARE STRUCTURE DEFINITION & FUNCTION IMPLEMENTATION OF DHT11 DEVICE COMMUNITCATION */
typedef struct {
    bool unused;
} DHT11_SoftHandleTypeDef;


typedef struct {
    DEVCMNIIO_TypeDef cmniio;
} DHT11_IOTypedef;

typedef struct {
    uint8_t humiInt;
    uint8_t humiDec;
    uint8_t tempInt;
    uint8_t tempDec;
    uint8_t check;
} DHT11_SCRTypedef;

void DHT11_Init(DEVS_TypeDef *devs, DEV_TypeDef dev[], poolsize devSize,
                int16_t devTemperature[], int16_t devHumidity[], int8_t devState[]);
void DHT11_Deinit(DEVS_TypeDef *devs, DEV_TypeDef dev[], poolsize devSize);
int8_t DHT11_SetTempHumi(poolsize num);
int16_t DHT11_GetTemperature(poolsize num);
int16_t DHT11_GetHumidity(poolsize num);
void DHT11_ClearTempHumi(poolsize num);

#endif
