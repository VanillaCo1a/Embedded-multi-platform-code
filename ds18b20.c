#include "ds18b20.h"

/***** DS18B20配置初始化 *****/

#define _CONVERT    0x44
#define _WR_SCRATCH 0x4E
#define _RD_SCRATCH 0xBE
#define _CP_TOROM   0x48
#define _CP_FROMROM 0xB8
#define _RD_POWER   0xB4

DEVS_TypeDef *ds18b20s = NULL;
DEV_TypeDef *ds18b20 = NULL;
poolsize ds18b20Size = 0;
int16_t *ds18b20Temp = NULL;
int8_t *ds18b20State;

/* DS18B20器件驱动函数 */
static void convertTemperature(int8_t alldevice) {    //开始温度转换
    DS18B20_PARTypeDef *ds_pa = (DS18B20_PARTypeDef *)DEV_GetActDev()->parameter;
    ONEWIRE_ModuleHandleTypeDef *modular = (ONEWIRE_ModuleHandleTypeDef *)DEV_GetActDevCmni();
    modular->skip = true;
    DEVCMNI_WriteByte(_CONVERT, 0);
    modular->skip = false;
    if(ds_pa->powermode == PARASITIC) {
        DEVIO_SetPin(&((DEVCMNIIO_TypeDef *)((DS18B20_IOTypedef *)DEV_GetActDevIo()))->SDA_SDO_TXD_OWRE);
        //tofix: 定义总线设备类型, 改为将总线置忙750ms
        DEV_SetActState(37500);    //todel
    } else if(ds_pa->powermode == EXTERNAL) {
        while(DEVCMNI_ReadBit(0x00)) continue;
    }
}
static void writeScrpatchpad(DS18B20_SCRTypedef *scr) {
    ONEWIRE_ModuleHandleTypeDef *modular = (ONEWIRE_ModuleHandleTypeDef *)DEV_GetActDevCmni();
    DEVCMNI_WriteByte(_WR_SCRATCH, 0);
    modular->skip = true;
    DEVCMNI_Write(&((uint8_t *)scr)[2], 3, 0);
    modular->skip = false;
}
static void readScrpatchpad(DS18B20_SCRTypedef *scr) {
    size_t length = 0;
    DEVCMNI_WriteByte(_RD_SCRATCH, 0);
    DEVCMNI_Read((uint8_t *)scr, sizeof(*scr) / sizeof(uint8_t), &length, 0x00);
}
static void copyScrToRom(void) {
    DS18B20_PARTypeDef *ds_pa = (DS18B20_PARTypeDef *)DEV_GetActDev()->parameter;
    DEVCMNI_WriteByte(_CP_TOROM, 0);
    if(ds_pa->powermode == PARASITIC) {
        DEVIO_SetPin(&((DEVCMNIIO_TypeDef *)((DS18B20_IOTypedef *)DEV_GetActDevIo()))->SDA_SDO_TXD_OWRE);
        //tofix: 定义总线设备类型, 改为将总线置忙10ms
        DEV_SetActState(500);    //todel
    }
}
static void recallScrFromRom(void) {
    DEVCMNI_WriteByte(_CP_FROMROM, 0);
}
void readPowerSupply(void) {
    DS18B20_PARTypeDef *ds_pa = (DS18B20_PARTypeDef *)DEV_GetActDev()->parameter;
    DEVCMNI_WriteByte(_RD_POWER, 0);
    if(DEVCMNI_ReadBit(0x00) == 1) {
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
static int8_t getTemperature(int16_t *tem, int8_t *state) {
    DS18B20_SCRTypedef scr = {0};
    if(!(*state)) {
        /* ds18b20 start convert */
        convertTemperature(1);
        (*state) = !(*state);
    } else {
        readScrpatchpad(&scr);
        *tem = (int16_t)((scr.msb << 8) | scr.lsb) * 6.25f;
        (*state) = !(*state);
        return 0;
    }
    return 1;
}

int8_t DS18B20_SetTem(poolsize num) {
    if(DEV_GetActState() == idle) {
        return getTemperature(&ds18b20Temp[num], &ds18b20State[num]);
    }
    return 1;
}
int16_t DS18B20_GetTem(poolsize num) {
    return ds18b20Temp[num];
}

/* DS18B20构造函数 */
void DS18B20_Init(DEVS_TypeDef *devs, DEV_TypeDef dev[], poolsize uSize, int16_t temp[], int8_t state[]) {
    ds18b20s = devs;
    ds18b20 = dev;
    ds18b20Size = uSize;
    ds18b20Temp = temp;
    ds18b20State = state;

    /* 初始化DS18B20类设备, 将参数绑定到设备池中, 并初始化通信引脚 */
    DEV_Init(ds18b20s, ds18b20, ds18b20Size);

    if(DEV_SetActStream(ds18b20s, 0) == 1) { DEV_Error(1); }
    /* 检测并更新ds18b20类设备的电源类型 */
    DEV_DoAction(ds18b20s, readPowerSupply);
}

/* TODO: DS18B20析构函数 */
void DS18B20_Deinit(DEVS_TypeDef *devs, DEV_TypeDef dev[], poolsize size) {}
