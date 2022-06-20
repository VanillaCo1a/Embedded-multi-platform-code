#ifndef __1_WIRE_H
#define __1_WIRE_H
#include "stdint.h"
#ifndef ENUM_Potential
#define ENUM_Potential
typedef enum {
    LOW,
    HIGH
} Potential_TypeDef;
#endif    // !ENUM_Potential
#ifndef ENUM_Direct
#define ENUM_Direct
typedef enum {
    IN,
    OUT
} Direct_TypeDef;
#endif    // !ENUM_Direct

typedef struct {           //ONEWIRE模拟总线结构体
    uint64_t num;          //总线上的设备数量
    int8_t flag_search;    //总线在最近一段时间内是否进行过搜索以更新设备数量
    void (*OWIO_Set)(Direct_TypeDef);
    void (*OWIO_Out)(Potential_TypeDef);
    Potential_TypeDef (*OWIO_In)(void);
    void (*delayus)(uint16_t us);
    void (*delayms)(uint16_t ms);
} ONEWIRE_AnalogHandleTypeDef;
typedef struct {
    uint64_t rom;       //模块64位ROM编码
    void *bushandle;    //ONEWIRE总线句柄
} ONEWIRE_ModuleHandleTypeDef;

static inline int8_t ONEWIRE_Init(ONEWIRE_ModuleHandleTypeDef *modular) {
    ((ONEWIRE_AnalogHandleTypeDef *)modular->bushandle)->OWIO_Out(HIGH);
    ((ONEWIRE_AnalogHandleTypeDef *)modular->bushandle)->OWIO_Set(IN);
    if(((ONEWIRE_AnalogHandleTypeDef *)modular->bushandle)->OWIO_In() != HIGH) {
        return 1;    //若总线没有被释放, 返回错误值
    }
    ((ONEWIRE_AnalogHandleTypeDef *)modular->bushandle)->OWIO_Set(OUT);
    return 0;
}
static inline uint8_t ONEWIRE_Reset(ONEWIRE_ModuleHandleTypeDef *modular) {
    uint8_t result = 0;
    ((ONEWIRE_AnalogHandleTypeDef *)modular->bushandle)->OWIO_Out(LOW);    //拉低总线480us, 发出复位信号
    ((ONEWIRE_AnalogHandleTypeDef *)modular->bushandle)->delayus(480);
    //todo: 关中断      //应答信号最好在60-120us期间读取, 应关闭中断
    ((ONEWIRE_AnalogHandleTypeDef *)modular->bushandle)->OWIO_Out(HIGH);    //释放总线, 等待60us
    ((ONEWIRE_AnalogHandleTypeDef *)modular->bushandle)->delayus(60);
    ((ONEWIRE_AnalogHandleTypeDef *)modular->bushandle)->OWIO_Set(IN);              //读取总线信息
    if(((ONEWIRE_AnalogHandleTypeDef *)modular->bushandle)->OWIO_In() == HIGH) {    //若总线被拉低, 返回0, 否则返回1
        result = 1;
    } else {
        result = 0;
    }
    //todo: 开中断
    ((ONEWIRE_AnalogHandleTypeDef *)modular->bushandle)->delayus(420);
    ((ONEWIRE_AnalogHandleTypeDef *)modular->bushandle)->OWIO_Set(OUT);
    return result;
}
static inline void ONEWIRE_WriteBit(ONEWIRE_ModuleHandleTypeDef *modular, uint8_t bit) {
    if(bit) {
        //todo: 关中断
        ((ONEWIRE_AnalogHandleTypeDef *)modular->bushandle)->OWIO_Out(LOW);
        ((ONEWIRE_AnalogHandleTypeDef *)modular->bushandle)->delayus(5);
        ((ONEWIRE_AnalogHandleTypeDef *)modular->bushandle)->OWIO_Out(HIGH);
        //todo: 开中断
        ((ONEWIRE_AnalogHandleTypeDef *)modular->bushandle)->delayus(55);
    } else {
        ((ONEWIRE_AnalogHandleTypeDef *)modular->bushandle)->OWIO_Out(LOW);
        ((ONEWIRE_AnalogHandleTypeDef *)modular->bushandle)->delayus(60);
        ((ONEWIRE_AnalogHandleTypeDef *)modular->bushandle)->OWIO_Out(HIGH);
        ((ONEWIRE_AnalogHandleTypeDef *)modular->bushandle)->delayus(1);
    }
}
static inline uint8_t ONEWIRE_ReadBit(ONEWIRE_ModuleHandleTypeDef *modular) {
    uint8_t bit;
    //todo: 关中断
    ((ONEWIRE_AnalogHandleTypeDef *)modular->bushandle)->OWIO_Out(LOW);     //拉低总线1us, 开始读时隙
    ((ONEWIRE_AnalogHandleTypeDef *)modular->bushandle)->delayus(1);        //若主控的主频较低, 可跳过1us的延时
    ((ONEWIRE_AnalogHandleTypeDef *)modular->bushandle)->OWIO_Out(HIGH);    //释放总线
    ((ONEWIRE_AnalogHandleTypeDef *)modular->bushandle)->OWIO_Set(IN);
    ((ONEWIRE_AnalogHandleTypeDef *)modular->bushandle)->delayus(10);    //在15us内的最后时刻读取总线, 考虑到误差只延时10us
    if(((ONEWIRE_AnalogHandleTypeDef *)modular->bushandle)->OWIO_In() == HIGH) {
        bit = 1;
    } else {
        bit = 0;
    }
    //todo: 开中断
    if(bit) {
        ((ONEWIRE_AnalogHandleTypeDef *)modular->bushandle)->delayus(50);
    } else {
        ((ONEWIRE_AnalogHandleTypeDef *)modular->bushandle)->delayus(51);
    }
    ((ONEWIRE_AnalogHandleTypeDef *)modular->bushandle)->OWIO_Set(OUT);
    return bit;
}
static inline void ONEWIRE_WriteByte(ONEWIRE_ModuleHandleTypeDef *modular, uint8_t byte) {
    for(uint8_t i = 0x01; i; i <<= 1) {
        ONEWIRE_WriteBit(modular, byte & i ? 1 : 0);
    }
}
static inline uint8_t ONEWIRE_ReadByte(ONEWIRE_ModuleHandleTypeDef *modular) {
    uint8_t byte = 0;
    for(uint8_t i = 0x01; i; i <<= 1) {
        byte |= ONEWIRE_ReadBit(modular) ? i : 0x00;
    }
    return byte;
}
static inline void ONEWIRE_Write(ONEWIRE_ModuleHandleTypeDef *modular, uint8_t *pdata, uint16_t size) {
    for(uint16_t j = 0; j < size; j++) {
        for(uint8_t i = 0x01; i; i <<= 1) {
            ONEWIRE_WriteBit(modular, pdata[j] & i ? 1 : 0);
        }
    }
    return;
}
static inline void ONEWIRE_Read(ONEWIRE_ModuleHandleTypeDef *modular, uint8_t *pdata, uint16_t size) {
    for(uint16_t j = 0; j < size; j++) {
        for(uint8_t i = 0x01; i != 0; i <<= 1) {
            pdata[j] |= ONEWIRE_ReadBit(modular) ? i : 0x00;
        }
    }
    return;
}
////////////////////////////////////////////////////////////////////////////
static inline void ONEWIRE_Skip(ONEWIRE_ModuleHandleTypeDef *modular) {    //ROM跳过匹配指令[限单个设备/接多个设备能同时进行的操作]
    ONEWIRE_WriteByte(modular, 0xCC);
}
static inline void ONEWIRE_Match(ONEWIRE_ModuleHandleTypeDef *modular) {    //ROM匹配指令
    ONEWIRE_WriteByte(modular, 0x55);
    ONEWIRE_Write(modular, (uint8_t *)&modular->rom, 8);
}
static inline void ONEWIRE_Query(ONEWIRE_ModuleHandleTypeDef *modular) {    //ROM读取指令[限单个设备]
    ONEWIRE_WriteByte(modular, 0x33);
}
static inline void ONEWIRE_Search(ONEWIRE_ModuleHandleTypeDef *modular) {    //ROM搜索指令
    ONEWIRE_WriteByte(modular, 0xF0);
}
static inline void ONEWIRE_AlarmSearch(ONEWIRE_ModuleHandleTypeDef *modular) {    //ROM报警搜索指令
    ONEWIRE_WriteByte(modular, 0xEC);
}

////////////////////////////////////////////////////////////////////////////
/**
 * @description: 单总线协议ROM搜索/报警搜索操作
 * 在发起一个复位信号后, 根据总线设备数量, 发出ROM读取/搜索指令, 或发出ROM报警搜索指令
 * @param {ONEWIRE_ModuleHandleTypeDef} *modular
 * @param {int8_t} searchtype
 * @return {*}
 */
static inline void MODULAR_ONEWIRE_Search(ONEWIRE_ModuleHandleTypeDef *modular, int8_t searchtype) {
    ONEWIRE_Init(modular);
    ONEWIRE_Reset(modular);
    if(searchtype == 0) {
        if(((ONEWIRE_AnalogHandleTypeDef *)modular->bushandle)->num == 1) {
            ONEWIRE_Query(modular);
        } else if(((ONEWIRE_AnalogHandleTypeDef *)modular->bushandle)->num > 1) {
            ONEWIRE_Search(modular);
            //...
        }
    } else {
        ONEWIRE_AlarmSearch(modular);
        //...
    }
}
static inline void MODULAR_ONEWIRE_WriteByte(ONEWIRE_ModuleHandleTypeDef *modular, uint8_t byte, int8_t skip, uint32_t timeout) {
    ONEWIRE_Init(modular);
    ONEWIRE_Reset(modular);
    if(skip || ((ONEWIRE_AnalogHandleTypeDef *)modular->bushandle)->num <= 1) {
        ONEWIRE_Skip(modular);
    } else if(((ONEWIRE_AnalogHandleTypeDef *)modular->bushandle)->num > 1) {
        ONEWIRE_Match(modular);
    }
    ONEWIRE_WriteByte(modular, byte);
}
static inline void MODULAR_ONEWIRE_WriteWord(ONEWIRE_ModuleHandleTypeDef *modular, uint16_t byte, int8_t skip, uint32_t timeout) {
    ONEWIRE_Init(modular);
    ONEWIRE_Reset(modular);
    if(skip || ((ONEWIRE_AnalogHandleTypeDef *)modular->bushandle)->num == 1) {
        ONEWIRE_Skip(modular);
    } else if(((ONEWIRE_AnalogHandleTypeDef *)modular->bushandle)->num > 1) {
        ONEWIRE_Match(modular);
    }
    ONEWIRE_WriteByte(modular, (uint8_t)byte);
    ONEWIRE_WriteByte(modular, (uint8_t)(byte >> 8));
}
static inline uint8_t MODULAR_ONEWIRE_ReadBit(ONEWIRE_ModuleHandleTypeDef *modular) {
    return ONEWIRE_ReadBit(modular);
}
static inline uint8_t MODULAR_ONEWIRE_ReadByte(ONEWIRE_ModuleHandleTypeDef *modular) {
    return ONEWIRE_ReadByte(modular);
}
static inline uint16_t MODULAR_ONEWIRE_ReadWord(ONEWIRE_ModuleHandleTypeDef *modular) {
    return (uint16_t)ONEWIRE_ReadByte(modular) | ((uint16_t)ONEWIRE_ReadByte(modular) << 8);
}
static inline void MODULAR_ONEWIRE_Write(ONEWIRE_ModuleHandleTypeDef *modular, uint8_t *pdata, uint16_t size, int8_t skip, uint32_t timeout) {
    ONEWIRE_Init(modular);
    ONEWIRE_Reset(modular);
    if(skip || ((ONEWIRE_AnalogHandleTypeDef *)modular->bushandle)->num == 1) {
        ONEWIRE_Skip(modular);
    } else if(((ONEWIRE_AnalogHandleTypeDef *)modular->bushandle)->num > 1) {
        ONEWIRE_Match(modular);
    }
    ONEWIRE_Write(modular, pdata, size);
    return;
}
static inline void MODULAR_ONEWIRE_Read(ONEWIRE_ModuleHandleTypeDef *modular, uint8_t *pdata, uint16_t size) {
    //tofix: 单总线的读时隙只能跟随在特定的主机写指令之后吗? 当需要并发地与多个设备进行通信时, 怎样进行独立的读操作?
    ONEWIRE_Read(modular, pdata, size);
    return;
}
#endif
