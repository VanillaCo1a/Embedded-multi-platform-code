/*
 * 本文件包含:
 * 1. DS18B20配置结构体初始化
 * 2. DS18B20配置初始化
 */
#include "ds18b20.h"

/////////////////////////    DS18B20配置结构体初始化    /////////////////////////
//    DS18B20参数配置
DS18B20_TypeDef ds18b20_parameter[DS18B20_NUM] = {
    {.powermode = PARASITIC},
};
//    DS18B20IO配置
DS18B20_IOTypedef ds18b20_io[DS18B20_NUM] = {
    {{.SDA_SDO_OWIO = {DS18B200_OWIO_GPIO_Port, DS18B200_OWIO_Pin}}},
};
//    DS18B20通信配置
#ifdef DEVICE_ONEWIRE_SOFTWARE_ENABLED
ONEWIRE_ModuleHandleTypeDef mowre = {.rom = 0x00, .bushandle = &ahowre[0]};
#endif    // DEVICE_ONEWIRE_SOFTWARE_ENABLED
DEVCMNI_TypeDef ds18b20_cmni[DS18B20_NUM] = {
    {.type = ONEWIRE, .dev = SOFTWARE, .handle = &mowre},
};

//    DS18B20设备结构体
DEVS_TypeDef ds18b20s = {.type = DS18B20, .size = DS18B20_NUM, .pool = (devpool_size)-1, .stream = 0};
DEV_TypeDef ds18b20[DS18B20_NUM] = {
    {.state = 0, .parameter = &ds18b20_parameter[0], .io = &ds18b20_io[0], .cmni = &ds18b20_cmni[0]},
};


/////////////////////////    DS18B20配置初始化    /////////////////////////
//    DS18B20器件驱动函数
#ifdef DEVICE_ONEWIRE_SOFTWARE_ENABLED
void convertTemperature(int8_t alldevice) {    //开始温度转换
    ONEWIRE_ModuleHandleTypeDef *handle = DEV_getdev(&ds18b20s)->cmni->handle;
    DEV_TypeDef *ds18b20 = DEV_getdev(&ds18b20s);
    DEVCMNI_WriteByte(0x44, 0, alldevice);
    //todo: 串行转并行处理
    if(((DS18B20_TypeDef *)DEV_getdev(&ds18b20s)->parameter)->powermode == PARASITIC) {
        ((ONEWIRE_AnalogHandleTypeDef *)((ONEWIRE_ModuleHandleTypeDef *)ds18b20->cmni->handle)->bushandle)->OWIO_Out(HIGH);
        //todo: 将总线置忙500ms
        delayms_timer(500);    //todel
    } else if(((DS18B20_TypeDef *)DEV_getdev(&ds18b20s)->parameter)->powermode == EXTERNAL) {
        while(MODULAR_ONEWIRE_ReadBit(handle))
            ;
    }
}
void writeScratchpad(DS18B20_SCRTypedef *scr) {
    DEVCMNI_WriteByte(0x4E, 0, 0);
    DEVCMNI_Write(&((uint8_t *)scr)[2], 3, 0, 1);
}
void readScratchpad(DS18B20_SCRTypedef *scr) {
    ONEWIRE_ModuleHandleTypeDef *handle = DEV_getdev(&ds18b20s)->cmni->handle;
    DEVCMNI_WriteByte(0xBE, 0, 0);
    MODULAR_ONEWIRE_Read(handle, (uint8_t *)scr, sizeof(*scr) / sizeof(uint8_t));
}
void copyScrToRom(void) {
    DEV_TypeDef *ds18b20 = DEV_getdev(&ds18b20s);
    DEVCMNI_WriteByte(0x48, 0, 0);
    if(((DS18B20_TypeDef *)ds18b20->parameter)->powermode == PARASITIC) {
        ((ONEWIRE_AnalogHandleTypeDef *)((ONEWIRE_ModuleHandleTypeDef *)ds18b20->cmni->handle)->bushandle)->OWIO_Out(HIGH);
        //todo: 将总线置忙10ms
        delayms_timer(10);    //todel
    }
}
void recallScrFromRom(void) {
    DEVCMNI_WriteByte(0xB8, 0, 0);
}
void readPowerSupply(void) {
    ONEWIRE_ModuleHandleTypeDef *handle = DEV_getdev(&ds18b20s)->cmni->handle;
    DEVCMNI_WriteByte(0xB4, 0, 0);
    if(MODULAR_ONEWIRE_ReadBit(handle) == 1) {
        ((DS18B20_TypeDef *)DEV_getdev(&ds18b20s)->parameter)->powermode = EXTERNAL;
    } else {
        ((DS18B20_TypeDef *)DEV_getdev(&ds18b20s)->parameter)->powermode = PARASITIC;
    }
}
/**
 * @description: 从ds18b20得到温度值, 单位: 0.01C
 * @param {int16_t} *tem
 * @return {*} 温度值(-550~1250)
 */
float getTem(int16_t *tem) {
    DS18B20_SCRTypedef scr = {0};
    convertTemperature(1);    //ds18b20 start convert
    readScratchpad(&scr);
    *tem = (int16_t)((scr.msb << 8) | scr.lsb) * 6.25f;
    return (int16_t)((scr.msb << 8) | scr.lsb) * 0.0625f;
}

int16_t temperature[DS18B20_NUM] = {0};
void DS18B20_Confi(void) {
    //初始化DS18B20类设备, 将参数绑定到设备池中, 并初始化通信引脚
    DEV_Confi(&ds18b20s, ds18b20);
    //检测并更新ds18b20类设备的电源类型
    DEV_Do(&ds18b20s, readPowerSupply);
}
void DS18B20_SetTem(devpool_size num) {
    DEV_setistm(&ds18b20s, num);
    getTem(&temperature[num]);
}
int16_t DS18B20_GetTem(devpool_size num) {
    return temperature[num];
}
#endif    // DEVICE_ONEWIRE_SOFTWARE_ENABLED
