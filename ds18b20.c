/*
 * 本文件包含:
 * 1. DS18B20配置结构体初始化
 * 2. DS18B20配置初始化
 */
#include "ds18b20.h"
#include "timer.h"


//    模拟通信的总线对象实例
/////////////////////////    DS18B20配置结构体初始化    /////////////////////////
//    DS18B20参数配置
DS18B20_PARTypeDef ds18b20_parameter[DS18B20_NUM] = {
    {.powermode = PARASITIC}};
//    DS18B20IO配置
DS18B20_IOTypedef ds18b20_io[DS18B20_NUM] = {
    {{.SDA_SDI_OWRE = {DS18B200_OWRE_GPIO_Port, DS18B200_OWRE_Pin}}}};
//    DS18B20通信配置
ONEWIRE_SoftHandleTypeDef ahowre[] = {{.num = 1, .flag_search = 0}};
ONEWIRE_ModuleHandleTypeDef mowre = {.rom = 0x00};
DS18B20_CMNITypeDef ds18b20_cmni[] = {
    {{.protocol = ONEWIRE, .ware = SOFTWARE, .modular = &mowre, .bus = &ahowre[0]}}};

//    DS18B20设备结构体
#define SIZE_DS18B20IO   sizeof(DS18B20_IOTypedef) / sizeof(DEVIO_TypeDef)
#define SIZE_DS18B20CMNI sizeof(DS18B20_CMNITypeDef) / sizeof(DEVCMNI_TypeDef)
DEVS_TypeDef ds18b20s = {.type = DS18B20};
DEV_TypeDef ds18b20[DS18B20_NUM] = {
    {.parameter = &ds18b20_parameter[0], .io = {.num = SIZE_DS18B20IO, .confi =(DEVIO_TypeDef *) &ds18b20_io[0], .init = NULL}, .cmni = {.num = SIZE_DS18B20CMNI, .confi = (DEVCMNI_TypeDef *)&ds18b20_cmni[0], .init = NULL}}};

/////////////////////////    DS18B20配置初始化    /////////////////////////
#define _CONVERT    0x44
#define _WR_SCRATCH 0x4E
#define _RD_SCRATCH 0xBE
#define _CP_TOROM   0x48
#define _CP_FROMROM 0xB8
#define _RD_POWER   0xB4
//    DS18B20器件驱动函数
void convertTemperature(int8_t alldevice) {    //开始温度转换
    DS18B20_PARTypeDef *ds_pa = (DS18B20_PARTypeDef *)DEV_getActDev()->parameter;
    DEVCMNI_WriteByte(_CONVERT, 0, alldevice);
    if(ds_pa->powermode == PARASITIC) {
        DEVIO_SetPin(&((DEVCMNIIO_TypeDef *)(DS18B20_IOTypedef *)DEV_getActDevIo())->SDA_SDI_OWRE);
        DEVIO_SetPin(&((DEVCMNIIO_TypeDef *)(DS18B20_IOTypedef *)DEV_getActDevIo())->SDA_SDI_OWRE);
        //tofix: 定义总线设备类型, 改为将总线置忙750ms
        DEV_setActState(37500);    //todel
    } else if(ds_pa->powermode == EXTERNAL) {
        while(DEVCMNI_ReadBit(0x00, 0))
            ;
    }
}
void writeScrpatchpad(DS18B20_SCRTypedef *scr) {
    DEVCMNI_WriteByte(_WR_SCRATCH, 0, 0);
    DEVCMNI_Write(&((uint8_t *)scr)[2], 3, 0, 1);
}
void readScrpatchpad(DS18B20_SCRTypedef *scr) {
    DEVCMNI_WriteByte(_RD_SCRATCH, 0, 0);
    DEVCMNI_Read((uint8_t *)scr, sizeof(*scr) / sizeof(uint8_t), 0x00, 0);
}
void copyScrToRom(void) {
    DS18B20_PARTypeDef *ds_pa = (DS18B20_PARTypeDef *)DEV_getActDev()->parameter;
    DEVCMNI_WriteByte(_CP_TOROM, 0, 0);
    if(ds_pa->powermode == PARASITIC) {
        DEVIO_SetPin(&((DEVCMNIIO_TypeDef *)(DS18B20_IOTypedef *)DEV_getActDevIo())->SDA_SDI_OWRE);
        //tofix: 定义总线设备类型, 改为将总线置忙10ms
        DEV_setActState(500);    //todel
    }
}
void recallScrFromRom(void) {
    DEVCMNI_WriteByte(_CP_FROMROM, 0, 0);
}
void readPowerSupply(void) {
    DS18B20_PARTypeDef *ds_pa = (DS18B20_PARTypeDef *)DEV_getActDev()->parameter;
    DEVCMNI_WriteByte(_RD_POWER, 0, 0);
    if(DEVCMNI_ReadBit(0x00, 0) == 1) {
        ds_pa->powermode = EXTERNAL;
    } else {
        ds_pa->powermode = PARASITIC;
    }
}
/**
 * @description: 从ds18b20得到温度值, 单位: 0.01C
 * @param {int16_t} *tem
 * @return {*} 温度值(-550~1250)
 */
int8_t getTemperature(int16_t *tem) {
    DS18B20_SCRTypedef scr = {0};
    static int8_t step = 0;
    if(!step) {
        convertTemperature(1);    //ds18b20 start convert
        step = !step;
    } else {
        readScrpatchpad(&scr);
        *tem = (int16_t)((scr.msb << 8) | scr.lsb) * 6.25f;
        step = !step;
        return 0;
    }
    return 1;
}

static int16_t temperature[DS18B20_NUM] = {0};
void DS18B20_Confi(void) {
    //初始化DS18B20类设备, 将参数绑定到设备池中, 并初始化通信引脚
    DEV_Init(&ds18b20s, ds18b20, DS18B20_NUM);
    //检测并更新ds18b20类设备的电源类型
    DEV_doAction(&ds18b20s, readPowerSupply);
}
int8_t DS18B20_SetTem(poolsize num) {
    if(DEV_getActState() == idle) {
        return getTemperature(&temperature[num]);
    }
    return 1;
}
int16_t DS18B20_GetTem(poolsize num) {
    return temperature[num];
}
