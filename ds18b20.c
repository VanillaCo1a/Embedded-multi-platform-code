#include "ds18b20.h"

#define _CONVERT    0x44
#define _WR_SCRATCH 0x4E
#define _RD_SCRATCH 0xBE
#define _CP_TOROM   0x48
#define _CP_FROMROM 0xB8
#define _RD_POWER   0xB4

DEVS_TypeDef *ds18b20s = NULL;
DEV_TypeDef *ds18b20 = NULL;
poolsize ds18b20Size = 0;
int16_t *ds18b20Temperature = NULL;
int8_t *ds18b20State = NULL;


/*****    DS18B20器件驱动函数    *****/

static void convertStart(void) {    //设备写指令, 进行温度转换
    bool bit = 0;
    uint8_t cm_pa = 0x00;
    DS18B20_PARTypeDef *ds_pa = (DS18B20_PARTypeDef *)DEV_GetActDev()->parameter;
    ONEWIRE_ModuleHandleTypeDef *modular = (ONEWIRE_ModuleHandleTypeDef *)DEV_GetActDevCmni();
    DEVCMNIIO_TypeDef *cmniio = DEV_GetActDevCmniIo();
    modular->skip = true;
    DEVCMNI_WriteByte(_CONVERT, &cm_pa);
    modular->skip = false;

    if(ds_pa->powermode == EXTERNAL) {
        /* 外部供电模式下的12位精度的温度转换时间约为650ms */
        DEVIO_SetPin(&cmniio->SDA_SDO_TXD_OWRE);
        DEV_SetActState(50 * 1000 / 20);

    } else if(ds_pa->powermode == PARASITIC) {
        /* 寄生供电模式下需要维持数据线强上拉750ms, 为器件内部电容充电 */
#if defined(STM32HAL)
        GPIO_InitTypeDef GPIO_InitStructure = {0};
        GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStructure.Pull = GPIO_PULLUP;
        GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        cmniio->SDA_SDO_TXD_OWRE.Init.Structure = GPIO_InitStructure;
#endif
        cmniio->SDA_SDO_TXD_OWRE.Init.State = DEVIO_PIN_SET;
        DEVIO_Init(&cmniio->SDA_SDO_TXD_OWRE);    //初始化OWIO
        DEVIO_SetPin(&cmniio->SDA_SDO_TXD_OWRE);
        DEV_SetActState(750 * 1000 / 20);    //tofix: 定义总线设备类型, 改为将总线置忙750ms
    }
}

static int8_t convertWait(void) {    //等待温度转换结束
    int8_t res = 2;
    bool bit = 0;
    uint8_t cm_pa = 0x00;
    DS18B20_PARTypeDef *ds_pa = (DS18B20_PARTypeDef *)DEV_GetActDev()->parameter;
    ONEWIRE_ModuleHandleTypeDef *modular = (ONEWIRE_ModuleHandleTypeDef *)DEV_GetActDevCmni();
    DEVCMNIIO_TypeDef *cmniio = DEV_GetActDevCmniIo();

    if(DEV_GetActState() == idle) {
        if(ds_pa->powermode == EXTERNAL) {
            DEVCMNI_ReadBit(&bit, &cm_pa);
            if(bit == 1) {
                res = 0;
            } else {
                DEV_SetActState(50 * 1000 / 20);
            }

        } else if(ds_pa->powermode == PARASITIC) {
#if defined(STM32HAL)
            GPIO_InitTypeDef GPIO_InitStructure = {0};
            GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
            GPIO_InitStructure.Pull = GPIO_PULLUP;
            GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
            cmniio->SDA_SDO_TXD_OWRE.Init.Structure = GPIO_InitStructure;
#endif
            cmniio->SDA_SDO_TXD_OWRE.Init.State = DEVIO_PIN_SET;
            DEVIO_Init(&cmniio->SDA_SDO_TXD_OWRE);    //初始化OWIO
            res = 0;
        }
    }
    return res;
}

static void writeScrpatchpad(DS18B20_SCRTypedef *scr) {
    size_t length = 0;
    uint8_t cm_pa = 0x00;
    ONEWIRE_ModuleHandleTypeDef *modular = (ONEWIRE_ModuleHandleTypeDef *)DEV_GetActDevCmni();

    DEVCMNI_WriteByte(_WR_SCRATCH, &cm_pa);
    modular->skip = true;
    DEVCMNI_Write(&((uint8_t *)scr)[2], 3, &length, &cm_pa);
    modular->skip = false;
}

static void readScrpatchpad(DS18B20_SCRTypedef *scr) {
    size_t length = 0;
    uint8_t cm_pa = 0x00;

    DEVCMNI_WriteByte(_RD_SCRATCH, &cm_pa);
    DEVCMNI_Read((uint8_t *)scr, sizeof(*scr) / sizeof(uint8_t), &length, &cm_pa);
}

static void copyScrToRom(void) {
    uint8_t cm_pa = 0x00;
    DS18B20_PARTypeDef *ds_pa = (DS18B20_PARTypeDef *)DEV_GetActDev()->parameter;

    DEVCMNI_WriteByte(_CP_TOROM, &cm_pa);
    if(ds_pa->powermode == PARASITIC) {
        DEVIO_SetPin(&((DEVCMNIIO_TypeDef *)((DS18B20_IOTypedef *)DEV_GetActDevIo()))->SDA_SDO_TXD_OWRE);
        //tofix: 定义总线设备类型, 改为将总线置忙10ms
        DEV_SetActState(500);    //todel
    }
}

static void recallScrFromRom(void) {
    uint8_t cm_pa = 0x00;

    DEVCMNI_WriteByte(_CP_FROMROM, &cm_pa);
}

static void readPowerSupply(void) {
    bool bit = 0;
    uint8_t cm_pa = 0x00;
    DS18B20_PARTypeDef *ds_pa = (DS18B20_PARTypeDef *)DEV_GetActDev()->parameter;

    DEVCMNI_WriteByte(_RD_POWER, &cm_pa);
    DEVCMNI_ReadBit(&bit, &cm_pa);
    if(bit == 1) {
        ds_pa->powermode = EXTERNAL;
    } else {
        ds_pa->powermode = PARASITIC;
    }
}


/*****    DS18B20外部调用接口    *****/

/* DS18B20构造函数 */
void DS18B20_Init(DEVS_TypeDef *devs, DEV_TypeDef dev[], poolsize devSize,
                  int16_t devTemperature[], int8_t devState[]) {
    ds18b20s = devs;
    ds18b20 = dev;
    ds18b20Size = devSize;
    ds18b20Temperature = devTemperature;
    ds18b20State = devState;
    if(ds18b20Temperature == NULL || ds18b20State == NULL) {
        //TODO: Error
    }

    /* 初始化设备类和设备, 将参数绑定到设备池中, 并初始化通信引脚 */
    DEV_Init(ds18b20s, ds18b20, ds18b20Size);
    /* 尝试设置设备IO流 */
    if(DEV_SetActStream(ds18b20s, 0) == 1) { DEV_Error(1); }

    /* 检测并更新ds18b20设备的电源类型 */
    DEV_DoAction(ds18b20s, readPowerSupply);

    /* 初始化数据区域 */
    for(size_t i = 0; i < ds18b20Size; i++) {
        DS18B20_ClearTemperature(i);
    }
}

/* TODO: DS18B20析构函数 */
void DS18B20_Deinit(DEVS_TypeDef *devs, DEV_TypeDef dev[], poolsize devSize) {}

/**
 * @description: 从ds18b20得到温度值; 单位: 0.01°C; 范围: -5500 ~ 12500
 * @param {poolsize} num 设备序号
 * @return {*} 读取结果
 */
int8_t DS18B20_SetTemperature(poolsize num) {
    int8_t res = 2;
    DS18B20_SCRTypedef scr = {0};
    int16_t *temperture = &ds18b20Temperature[num];
    int8_t *state = &ds18b20State[num];

    DEV_SetActStream(ds18b20s, num);
    if(DEV_GetActState() == idle) {
        if(*state == 1) {
            if(convertWait() == 0) {
                readScrpatchpad(&scr);
                *temperture = (int16_t)((scr.msb << 8) | scr.lsb) * 6.25f;
                res = 0;
                (*state) = 0;
            }
        }
        if(*state == 0) {
            convertStart();
            (*state)++;
        }
    }
    return res;
}

int16_t DS18B20_GetTemperature(poolsize num) {
    return ds18b20Temperature[num];
}

void DS18B20_ClearTemperature(poolsize num) {
    ds18b20Temperature[num] = 0;
    ds18b20State[num] = 0;
}
