#ifndef __DS18B20_H
#define __DS18B20_H
#include "device.h"
#include "timer.h"
#define DS18B20_NUM 1
typedef enum {
    PARASITIC,
    EXTERNAL,
} DS18B20_POWMDTypedef;

typedef struct {
    DS18B20_POWMDTypedef powermode;
} DS18B20_TypeDef;
typedef struct {
    DEVCMNI_IOTypeDef cmniio;
} DS18B20_IOTypedef;
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
void DS18B20_Confi(void);
void DS18B20_SetTem(void);
int16_t DS18B20_GetTem(void);

#endif
