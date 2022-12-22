#include "dht11.h"


/*****    DHT11通信驱动函数    *****/

/* SOFTWARE STRUCTURE DEFINITION & FUNCTION IMPLEMENTATION OF DHT11 DEVICE COMMUNITCATION */
#ifdef DEVDHT11_SOFTWARE_ENABLED
#define DEVDHT11_IO_Set(pot)           DEVCMNI_SDA_OWRE_Set(pot)
#define DEVDHT11_IO_Out(pot)           DEVCMNI_SDA_SDI_RXD_OWRE_Out(pot)
#define DEVDHT11_IO_In()               DEVCMNI_SDA_OWRE_In()
#define DEVDHT11_Error(err)            DEVCMNI_Error(err)
#define DEVDHT11_Delayus(us)           ({if(us) {DEVCMNI_Delayus(us);} })
#define DEVDHT11_Delayms(ms)           ({if(ms) {DEVCMNI_Delayms(ms);} })
#define DEVDHT11_Delayus_paral(us)     DEVCMNI_Delayus_paral(us, 1)
#define DEVDHT11_Delayus_paral_close() DEVCMNI_Delayus_paral(-1, 0)

#define DEVDHT11_OWRE_HIGH_TIME 1
#define DEVDHT11_OWRE_LOW_TIME  0
#define DEVDHT11_ERROR_WAITING 100
#define DEVDHT11_MEASURE_PRE   10

static inline int8_t DEVDHT11_Init(ONEWIRE_ModuleHandleTypeDef *modular) {
    DEVDHT11_IO_Out(HIGH);
    DEVDHT11_Delayus(DEVDHT11_OWRE_HIGH_TIME);
    DEVDHT11_IO_Set(IN);
    if(DEVDHT11_IO_In() != HIGH) {
        return -1;    //若总线没有被释放, 返回错误值
    }
    DEVDHT11_IO_Set(OUT);
    return 0;
}
static inline int8_t DEVDHT11_Reset(ONEWIRE_ModuleHandleTypeDef *modular) {
    //todo: 关中断
    DEVDHT11_IO_Out(HIGH);    //释放总线, 等待进行采样
    DEVDHT11_Delayus(15 + DEVDHT11_OWRE_HIGH_TIME - DEVDHT11_MEASURE_PRE);
    DEVDHT11_IO_Set(IN);
    while(DEVDHT11_IO_In() == HIGH) {    //等待从机拉低信号, 进行响应
        if(DEVDHT11_Delayus_paral((40 - 15) + (DEVDHT11_MEASURE_PRE + DEVDHT11_ERROR_WAITING))) { return -1; }
    }
    DEVDHT11_Delayus_paral_close();

    DEVDHT11_Delayus(83 + DEVDHT11_OWRE_LOW_TIME - DEVDHT11_MEASURE_PRE);
    while(DEVDHT11_IO_In() == LOW) {
        if(DEVDHT11_Delayus_paral(DEVDHT11_MEASURE_PRE + DEVDHT11_ERROR_WAITING)) { return -1; }
    }
    DEVDHT11_Delayus_paral_close();

    DEVDHT11_Delayus(87 + DEVDHT11_OWRE_HIGH_TIME - DEVDHT11_MEASURE_PRE);
    while(DEVDHT11_IO_In() == HIGH) {    //等待拉高80us, 准备发送数据
        if(DEVDHT11_Delayus_paral(DEVDHT11_MEASURE_PRE + DEVDHT11_ERROR_WAITING)) { return -1; }
    }
    DEVDHT11_Delayus_paral_close();

    return 0;
}
static inline int8_t DEVDHT11_ReadBit(ONEWIRE_ModuleHandleTypeDef *modular) {
    bool bit = 0;
    DEVDHT11_Delayus(30 + DEVDHT11_OWRE_LOW_TIME - DEVDHT11_MEASURE_PRE);
    while(DEVDHT11_IO_In() == LOW) {
        if(DEVDHT11_Delayus_paral((54 - 30) + (DEVDHT11_MEASURE_PRE + DEVDHT11_ERROR_WAITING))) { return -1; }
    }
    DEVDHT11_Delayus_paral_close();

    DEVDHT11_Delayus(40 + DEVDHT11_OWRE_HIGH_TIME);
    if(DEVDHT11_IO_In() == HIGH) {
        bit = 1;
    } else {
        bit = 0;
    }
    while(DEVDHT11_IO_In() == HIGH) {
        if(DEVDHT11_Delayus_paral(74 - 40 + DEVDHT11_ERROR_WAITING)) { return -1; }
    }
    DEVDHT11_Delayus_paral_close();

    return bit;
}
static inline int8_t DEVDHT11_Read(ONEWIRE_ModuleHandleTypeDef *modular, uint8_t *pdata, size_t size) {
    int8_t bit = 0;
    for(uint16_t j = 0; j < size; j++) {
        for(uint8_t i = 0x80; i != 0; i >>= 1) {
            if((bit = DEVDHT11_ReadBit(modular)) == -1) { return -1; }
            pdata[j] |= bit ? i : 0x00;
        }
    }
    //todo: 开中断
    return 0;
}
DEV_StatusTypeDef DEVDHT11_Dispatch(void *handle, uint8_t *pdata, size_t size, bool rw,
                                    size_t *length, void *parameter, uint32_t timeout) {
    ONEWIRE_ModuleHandleTypeDef *modular = handle;
    if(rw == 1) {
        do {
            if(DEVDHT11_Init(modular) == -1) { break; }
            if(DEVDHT11_Reset(modular) == -1) { break; }
            if(DEVDHT11_Read(modular, pdata, size) == -1) { break; }
            return DEV_OK;
        } while(0);
        return DEV_ERROR;
    } else {
        return DEV_OK;
    }
}
DEV_StatusTypeDef DEVDHT11_DispatchBit(void *handle, bool *bit, bool rw,
                                       void *parameter, uint32_t timeout) {
    ONEWIRE_ModuleHandleTypeDef *modular = handle;
    if(rw) {
        DEVDHT11_IO_Set(IN);
        *bit = DEVDHT11_IO_In();
    } else {
        DEVDHT11_IO_Set(OUT);
        DEVDHT11_IO_Out(*bit);
    }
    return DEV_OK;
}
#undef DEVDHT11_OWIO_Set
#undef DEVDHT11_OWIO_Out
#undef DEVDHT11_OWIO_In
#undef DEVDHT11_Error
#undef DEVDHT11_Delayus
#undef DEVDHT11_Delayms
#undef DEVDHT11_Delayus_paral

#endif    // DEVDHT11_SOFTWARE_ENABLED



/*****    DHT11设备驱动函数    *****/

static int8_t convertStart(void) {
    int8_t res = 2;
    bool bit = 0;
    uint8_t cm_pa = 0x00;

    if(DEV_GetActState() == idle) {
        /* 拉低设备引脚, 开始温度转换 */
        DEVCMNI_WriteBit(&bit, &cm_pa);
        /* 低电平时间不小于18ms, 不大于30ms */
        DEV_SetActState(20 * 1000 / 20);
        res = 0;
    }
    return res;
}

static int8_t convertWait(void) {
    int8_t res = 2;
    bool bit = 1;
    uint8_t cm_pa = 0x00;

    if(DEV_GetActState() == idle) {
        DEVCMNI_WriteBit(&bit, &cm_pa);
        res = 0;
    }
    return res;
}

static int8_t readScrpatchpad(DHT11_SCRTypedef *scr) {
    int8_t res;
    size_t length = 0;
    uint8_t cm_pa = 0x00;

    res = DEVCMNI_Read((uint8_t *)scr, sizeof(*scr) / sizeof(uint8_t), &length, &cm_pa);
    return res;
}



/*****    DHT11外部调用接口    *****/

#define BUSY_TIME 1200000
static DEVS_TypeDef *dht11s = NULL;
static DEV_TypeDef *dht11 = NULL;
static poolsize dht11Size = 0;
static int16_t *dht11Temperature = NULL;
static int16_t *dht11Humidity = NULL;
static int8_t *dht11State = NULL;

/* DHT11构造函数 */
void DHT11_Init(DEVS_TypeDef *devs, DEV_TypeDef dev[], poolsize devSize,
                int16_t devTemperature[], int16_t devHumidity[], int8_t devState[]) {
    dht11s = devs;
    dht11 = dev;
    dht11Size = devSize;
    dht11Temperature = devTemperature;
    dht11Humidity = devHumidity;
    dht11State = devState;
    if(dht11Temperature == NULL || dht11Humidity == NULL || dht11State == NULL) {
        //TODO: Error
    }

    /* 初始化DHT11类设备, 将参数绑定到设备池中, 并初始化通信引脚 */
    DEV_Init(dht11s, dht11, dht11Size);
    /* 尝试设置设备IO流 */
    if(DEV_SetActStream(dht11s, 0) == 1) { DEV_Error(1); }

    /* 上单后dht11设备需要等待约1s后才能开始工作 */
    DEV_SetActState(BUSY_TIME / 20);

    /* 初始化数据区域 */
    for(size_t i = 0; i < dht11Size; i++) {
        DHT11_ClearTempHumi(i);
    }
}

/* TODO: DHT11析构函数 */
void DHT11_Deinit(DEVS_TypeDef *devs, DEV_TypeDef dev[], poolsize devSize) {}

/**
 * @description: 从dht11得到温湿度值; 单位: 0.01°C, 0.01%; 范围: 0 ~ 5000, 2000 ~ 9000
 * @param {poolsize} num 设备序号
 * @return {*} 读取结果
 */
int8_t DHT11_SetTempHumi(poolsize num) {
    int8_t res = 2;
    DHT11_SCRTypedef scr = {0};
    int16_t *temperture = &dht11Temperature[num];
    int16_t *humidity = &dht11Humidity[num];
    int8_t *state = &dht11State[num];

    DEV_SetActStream(dht11s, num);
    if(DEV_GetActState() == idle) {
        if(*state == 1) {
            if(convertWait() == 0) {
                if(readScrpatchpad(&scr) == 0) {
                    *temperture = (int16_t)(scr.tempInt * 100 + scr.tempDec * 10);
                    *humidity = (int16_t)(scr.humiInt * 100 + scr.humiDec * 10);
                    res = 0;
                } else {
                    //TODO: Error
                    res = -1;
                }
                DEV_SetActState(BUSY_TIME / 20);
                (*state) = 0;
            }
        }
        if(*state == 0) {
            if(convertStart() == 0) {
                (*state)++;
            }
        }
    }
    return res;
}

int16_t DHT11_GetTemperature(poolsize num) {
    return dht11Temperature[num];
}

int16_t DHT11_GetHumidity(poolsize num) {
    return dht11Humidity[num];
}

void DHT11_ClearTempHumi(poolsize num) {
    dht11Temperature[num] = 0;
    dht11Humidity[num] = 0;
    dht11State[num] = 0;
}
