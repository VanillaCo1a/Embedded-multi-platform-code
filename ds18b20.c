/*
 * 本文件包含:
 * 1. DS18B20配置结构体初始化
 * 2. DS18B20配置初始化
 */
#include "ds18b20.h"

/////////////////////////    DS18B20配置结构体初始化    /////////////////////////
//    DS18B20设备结构体
DEVS_TypeDef ds18b20s = {.type = DS18B20, .size = DS18B20_NUM, .pool = (devpool_size)-1, .stream = 0};
//    DS18B20参数配置
DS18B20_TypeDef ds18b20_parameter[DS18B20_NUM] = {
    {.powermode = PARASITIC},
};
//    DS18B20通信配置
DEVCMNI_TypeDef ds18b20_cmni[DS18B20_NUM] = {
    {.type = OWRE, .dev = SOFTWARE, .handle = {.haowre = {0x00, &aowre}, .howre = NULL}},
};
//    DS18B20IO配置
DS18B20_IOTypedef ds18b20_io[DS18B20_NUM] = {
    {{.SDA_SDO_OWIO = {DS18B200_SDA_SDO_GPIO_Port, DS18B200_SDA_SDO_Pin}}},
};


/////////////////////////    DS18B20配置初始化    /////////////////////////
//    DS18B20器件驱动函数
void convTem(int8_t alldevice) {    //开始温度转换
    MODULAR_OWRE_WriteByte(&DEV_getdev(&ds18b20s)->cmni->handle.haowre, 0x44, alldevice);
    if(((DS18B20_TypeDef *)DEV_getdev(&ds18b20s)->parameter)->powermode == EXTERNAL) {
        DEV_getdev(&ds18b20s)->cmni->handle.haowre.method->OWIO_Out(HIGH);
        //todo: 将总线置忙500ms
    }
}
DS18B20_SCRTypedef *writeScr(DS18B20_SCRTypedef *scr) {
    MODULAR_OWRE_WriteByte(&DEV_getdev(&ds18b20s)->cmni->handle.haowre, 0x4E, 0);
    MODULAR_OWRE_Write(&DEV_getdev(&ds18b20s)->cmni->handle.haowre, &((uint8_t *)scr)[2], 3, 1);
    return scr;
}
DS18B20_SCRTypedef *readScr(DS18B20_SCRTypedef *scr) {
    MODULAR_OWRE_WriteByte(&DEV_getdev(&ds18b20s)->cmni->handle.haowre, 0xBE, 0);
    MODULAR_OWRE_Read(&DEV_getdev(&ds18b20s)->cmni->handle.haowre, (uint8_t *)scr, sizeof(*scr) / sizeof(uint8_t));
    return scr;
}
void copyScr(void) {
    MODULAR_OWRE_WriteByte(&DEV_getdev(&ds18b20s)->cmni->handle.haowre, 0x48, 0);
    if(((DS18B20_TypeDef *)DEV_getdev(&ds18b20s)->parameter)->powermode == EXTERNAL) {
        DEV_getdev(&ds18b20s)->cmni->handle.haowre.method->OWIO_Out(HIGH);
        //todo: 将总线置忙10ms
    }
}
void recallEeprom(void) {
    MODULAR_OWRE_WriteByte(&DEV_getdev(&ds18b20s)->cmni->handle.haowre, 0xB8, 0);
}
void readPowerSupply(void) {
    MODULAR_OWRE_WriteByte(&DEV_getdev(&ds18b20s)->cmni->handle.haowre, 0xB4, 0);
    if(MODULAR_OWRE_ReadBit(&DEV_getdev(&ds18b20s)->cmni->handle.haowre) == 1) {
        ((DS18B20_TypeDef *)DEV_getdev(&ds18b20s)->parameter)->powermode = EXTERNAL;
    } else {
        ((DS18B20_TypeDef *)DEV_getdev(&ds18b20s)->parameter)->powermode = PARASITIC;
    }
}
//从ds18b20得到温度值
//精度：0.1C
//返回值：温度值 （-550~1250）
float getTem(int16_t *tem) {
    DS18B20_SCRTypedef scr = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    float temf;
    convTem(1);    //ds18b20 start convert
    delayms_timer(500);
    readScr(&scr);
    temf = (((uint16_t)scr.msb << 8) | (uint16_t)scr.lsb) * 0.625f;
    //*tem = temf;
    *tem = (((uint16_t)scr.msb << 8) | (uint16_t)scr.lsb);
    return temf;
}

volatile int16_t temperature = 0;
void DS18B20_Confi(void) {
    //初始化DS18B20类设备, 将参数绑定到设备池中, 并初始化通信引脚
    DEV_Confi(&ds18b20s, ds18b20_parameter, ds18b20_io, ds18b20_cmni);
}
void DS18B20_SetTem(void) {
    DEV_setistm(&ds18b20s, 0);
    getTem((int16_t *)&temperature);
}
int16_t DS18B20_GetTem(void) {
    return temperature;
}
