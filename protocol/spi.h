#ifndef __SPI_H
#define __SPI_H
#include "stdint.h"
#define SPITime 1
// #define SPI_USEPOINTER

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

typedef struct {    //SPI模拟总线结构体
#ifdef SPI_USEPOINTER
    void (*SCLK_Out)(uint8_t);
    void (*SDO_Out)(uint8_t);
    void (*CS_Out)(uint8_t);
    void (*error)(void);
    void (*delayus)(uint16_t us);
    void (*delayms)(uint16_t ms);
#endif    //SPI_USEPOINTER
} SPI_SoftHandleTypeDef;
typedef struct {        //SPI总线模块结构体
    void *bushandle;    //SPI模拟/硬件总线句柄
} SPI_ModuleHandleTypeDef;

////////////////////////////////////////////////////////////////////////////
#if defined(SPI_USEPOINTER)
#define SPIBUS_SCLK_Out(pot) ((SPI_SoftHandleTypeDef *)modular->bushandle)->SCLK_Out(pot)
#define SPIBUS_SDO_Out(pot) ((SPI_SoftHandleTypeDef *)modular->bushandle)->SDO_Out(pot)
#define SPIBUS_CS_Out(pot) ((SPI_SoftHandleTypeDef *)modular->bushandle)->CS_Out(pot)
#define SPIBUS_ERROR() ((SPI_SoftHandleTypeDef *)modular->bushandle)->error()
#define SPIBUS_delayus(us) ({if(us) {((SPI_SoftHandleTypeDef *)modular->bushandle)->delayus(us);} })
#define SPIBUS_delayms(ms) ({if(ms) {((SPI_SoftHandleTypeDef *)modular->bushandle)->delayms(ms);} })
#else
#define SPIBUS_SCLK_Out(pot) DEVCMNI_SCLK_Out(pot)
#define SPIBUS_SDO_Out(pot) DEVCMNI_SDO_Out(pot)
#define SPIBUS_CS_Out(pot) DEVCMNI_CS_Out(pot)
#define SPIBUS_ERROR() DEVCMNI_Error()
#define SPIBUS_delayus(us) ({if(us) {DEVCMNI_Delayus(us);} })
#define SPIBUS_delayms(ms) ({if(ms) {DEVCMNI_Delayms(ms);} })
#endif    // SPI_USEPOINTER
void DEVCMNI_SCLK_Out(Potential_TypeDef pot);
void DEVCMNI_SDO_Out(Potential_TypeDef pot);
void DEVCMNI_CS_Out(Potential_TypeDef pot);
void DEVCMNI_Error(void);
void DEVCMNI_Delayus(uint16_t us);
void DEVCMNI_Delayms(uint16_t ms);

static inline void SPI_Init(SPI_ModuleHandleTypeDef *modular) {
    SPIBUS_CS_Out(HIGH);
    SPIBUS_SDO_Out(HIGH);
    SPIBUS_SCLK_Out(HIGH);
    SPIBUS_CS_Out(LOW);
}
static inline void SPI_WriteBit(SPI_ModuleHandleTypeDef *modular, uint8_t bit) {
    SPIBUS_SCLK_Out(LOW);    //拉低SCL, 表示上一时钟周期结束
    if(bit) {
        SPIBUS_SDO_Out(HIGH);
    } else {
        SPIBUS_SDO_Out(LOW);
    }
    SPIBUS_SCLK_Out(HIGH);
    SPIBUS_delayus(SPITime);
}
//单字节写入函数
static inline void MODULAR_SPI_WriteByte(SPI_ModuleHandleTypeDef *modular, uint8_t byte, int8_t skip, uint32_t timeout) {
    SPI_Init(modular);
    if(!skip) {
        SPIBUS_CS_Out(LOW);    //拉低片选
    }
    for(uint8_t i = 0x80; i; i >>= 1) {
        SPI_WriteBit(modular, byte & i ? 1 : 0);
    }
    if(!skip) {
        SPIBUS_CS_Out(HIGH);    //拉高片选
    }
}
//多字节写入函数
static inline void MODULAR_SPI_Write(SPI_ModuleHandleTypeDef *modular, uint8_t *pdata, uint16_t size, int8_t skip, uint32_t timeout) {
    SPI_Init(modular);
    if(!skip) {
        SPIBUS_CS_Out(LOW);    //拉低片选
    }
    for(uint16_t j = 0; j < size; j++) {
        for(uint8_t i = 0x80; i; i >>= 1) {
            SPI_WriteBit(modular, pdata[j] & i ? 1 : 0);
        }
    }
    if(!skip) {
        SPIBUS_CS_Out(HIGH);    //拉高片选
    }
}

#undef SPIBUS_SCLK_Out
#undef SPIBUS_SDO_Out
#undef SPIBUS_CS_Out
#undef SPIBUS_delayus
#undef SPIBUS_delayms
#endif
