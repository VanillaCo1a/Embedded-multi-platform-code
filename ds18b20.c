/*
 * 本文件包含:
 * 1. DS18B20配置结构体初始化
 * 2. DS18B20配置初始化
 */
#include "ds18b20.h"
#include "timer.h"

/////////////////////////    DS18B20配置结构体初始化    /////////////////////////
//    DS18B20参数配置
DS18B20_PATypeDef ds18b20_parameter[DS18B20_NUM] = {
    {.powermode = PARASITIC},
};
//    DS18B20IO配置
DS18B20_IOTypedef ds18b20_io[DS18B20_NUM] = {
    {{.SDA_SDI_OWRE = {DS18B200_OWRE_GPIO_Port, DS18B200_OWRE_Pin}}},
};
//    DS18B20通信配置
ONEWIRE_ModuleHandleTypeDef mowre = {.rom = 0x00, .bushandle = &ahowre[0]};
DEVCMNI_TypeDef ds18b20_cmni[DS18B20_NUM] = {
    {.protocol = ONEWIRE, .ware = SOFTWARE, .handle = &mowre},
};

//    DS18B20设备结构体
DEVS_TypeDef ds18b20s = {.type = DS18B20, .size = DS18B20_NUM};
DEV_TypeDef ds18b20[DS18B20_NUM] = {
    {.state = 0, .parameter = &ds18b20_parameter[0], .io = &ds18b20_io[0], .cmni = &ds18b20_cmni[0]},
};

/////////////////////////    DS18B20配置初始化    /////////////////////////
#define _CONVERT 0x44
#define _WR_SCRATCH 0x4E
#define _RD_SCRATCH 0xBE
#define _CP_TOROM 0x48
#define _CP_FROMROM 0xB8
#define _RD_POWER 0xB4
//    DS18B20器件驱动函数
void convertTemperature(int8_t alldevice) {    //开始温度转换
    DS18B20_PATypeDef *ds_pa = DEV_GetActStream()->parameter;
    DEVCMNI_WriteByte(_CONVERT, 0, alldevice);
    if(ds_pa->powermode == PARASITIC) {
        DEVIO_WritePin_SET(&((DEVCMNIIO_TypeDef *)DEV_GetActStream()->io)->SDA_SDI_OWRE);
        DEVIO_WritePin_SET(&((DEVCMNIIO_TypeDef *)DEV_GetActStream()->io)->SDA_SDI_OWRE);
        //tofix: 定义总线设备类型, 改为将总线置忙750ms
        DEV_SetActState(37500);    //todel
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
    DS18B20_PATypeDef *ds_pa = DEV_GetActStream()->parameter;
    DEVCMNI_WriteByte(_CP_TOROM, 0, 0);
    if(ds_pa->powermode == PARASITIC) {
        DEVIO_WritePin_SET(&((DEVCMNIIO_TypeDef *)DEV_GetActStream()->io)->SDA_SDI_OWRE);
        //tofix: 定义总线设备类型, 改为将总线置忙10ms
        DEV_SetActState(500);    //todel
    }
}
void recallScrFromRom(void) {
    DEVCMNI_WriteByte(_CP_FROMROM, 0, 0);
}
void readPowerSupply(void) {
    DS18B20_PATypeDef *ds_pa = DEV_GetActStream()->parameter;
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
    DEV_Confi(&ds18b20s, ds18b20);
    //检测并更新ds18b20类设备的电源类型
    DEV_ReCall(&ds18b20s, readPowerSupply);
}
int8_t DS18B20_SetTem(devpool_size num) {
    if(DEV_GetActState() == idle) {
        return getTemperature(&temperature[num]);
    }
    return 1;
}
int16_t DS18B20_GetTem(devpool_size num) {
    return temperature[num];
}
