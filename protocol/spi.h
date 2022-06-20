#ifndef __SPI_H
#define __SPI_H
#include "stdint.h"

//#define SPI_USEMACRO

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
    void (*SCLK_Out)(uint8_t);
    void (*SDO_Out)(uint8_t);
    void (*CS_Out)(uint8_t);
    void (*delayus)(uint16_t us);
    void (*delayms)(uint16_t ms);
} SPI_AnalogHandleTypeDef;
typedef struct {        //SPI总线模块结构体
    void *bushandle;    //SPI模拟/硬件总线句柄
} SPI_ModuleHandleTypeDef;

#define SPI_delaytime()

#ifdef SPI_USEMACRO
#define SPI_Init(modular) ({                                           \
    ((SPI_AnalogHandleTypeDef *)(modular)->bushandle)->CS_Out(HIGH);   \
    ((SPI_AnalogHandleTypeDef *)(modular)->bushandle)->SDO_Out(HIGH);  \
    ((SPI_AnalogHandleTypeDef *)(modular)->bushandle)->SCLK_Out(HIGH); \
    ((SPI_AnalogHandleTypeDef *)(modular)->bushandle)->CS_Out(LOW);    \
})
#define SPI_WriteBit(modular, bit) ({                                     \
    ((SPI_AnalogHandleTypeDef *)(modular)->bushandle)->SCLK_Out(LOW);     \
    if(bit) {                                                             \
        ((SPI_AnalogHandleTypeDef *)(modular)->bushandle)->SDO_Out(HIGH); \
    } else {                                                              \
        ((SPI_AnalogHandleTypeDef *)(modular)->bushandle)->SDO_Out(LOW);  \
    }                                                                     \
    ((SPI_AnalogHandleTypeDef *)(modular)->bushandle)->SCLK_Out(HIGH);    \
    SPI_delaytime();                                                      \
})
//单字节写入函数
#define MODULAR_SPI_WriteByte(modular, byte, skip, timeout) ({           \
    SPI_Init((modular));                                                 \
    if(!skip) {                                                          \
        ((SPI_AnalogHandleTypeDef *)(modular)->bushandle)->CS_Out(LOW);  \
    }                                                                    \
    for(uint8_t i = 0x80; i; i >>= 1) {                                  \
        SPI_WriteBit((modular), byte &i ? 1 : 0);                        \
    }                                                                    \
    if(!skip) {                                                          \
        ((SPI_AnalogHandleTypeDef *)(modular)->bushandle)->CS_Out(HIGH); \
    }                                                                    \
})
//多字节写入函数
#define MODULAR_SPI_Write(modular, pdata, size, skip, timeout) ({        \
    SPI_Init((modular));                                                 \
    if(!skip) {                                                          \
        ((SPI_AnalogHandleTypeDef *)(modular)->bushandle)->CS_Out(LOW);  \
    }                                                                    \
    for(uint16_t j = 0; j < size; j++) {                                 \
        for(uint8_t i = 0x80; i; i >>= 1) {                              \
            SPI_WriteBit((modular), pdata[j] & i ? 1 : 0);               \
        }                                                                \
    }                                                                    \
    if(!skip) {                                                          \
        ((SPI_AnalogHandleTypeDef *)(modular)->bushandle)->CS_Out(HIGH); \
    }                                                                    \
})
#else
static inline void SPI_Init(SPI_ModuleHandleTypeDef *modular) {
    ((SPI_AnalogHandleTypeDef *)modular->bushandle)->CS_Out(HIGH);
    ((SPI_AnalogHandleTypeDef *)modular->bushandle)->SDO_Out(HIGH);
    ((SPI_AnalogHandleTypeDef *)modular->bushandle)->SCLK_Out(HIGH);
    ((SPI_AnalogHandleTypeDef *)modular->bushandle)->CS_Out(LOW);
}
static inline void SPI_WriteBit(SPI_ModuleHandleTypeDef *modular, uint8_t bit) {
    ((SPI_AnalogHandleTypeDef *)modular->bushandle)->SCLK_Out(LOW);    //拉低SCL, 表示上一时钟周期结束
    if(bit) {
        ((SPI_AnalogHandleTypeDef *)modular->bushandle)->SDO_Out(HIGH);
    } else {
        ((SPI_AnalogHandleTypeDef *)modular->bushandle)->SDO_Out(LOW);
    }
    ((SPI_AnalogHandleTypeDef *)modular->bushandle)->SCLK_Out(HIGH);
    SPI_delaytime();
}
//单字节写入函数
static inline void MODULAR_SPI_WriteByte(SPI_ModuleHandleTypeDef *modular, uint8_t byte, int8_t skip, uint32_t timeout) {
    SPI_Init(modular);
    if(!skip) {
        ((SPI_AnalogHandleTypeDef *)modular->bushandle)->CS_Out(LOW);    //拉低片选
    }
    for(uint8_t i = 0x80; i; i >>= 1) {
        SPI_WriteBit(modular, byte & i ? 1 : 0);
    }
    if(!skip) {
        ((SPI_AnalogHandleTypeDef *)modular->bushandle)->CS_Out(HIGH);    //拉高片选
    }
}
//多字节写入函数
static inline void MODULAR_SPI_Write(SPI_ModuleHandleTypeDef *modular, uint8_t *pdata, uint16_t size, int8_t skip, uint32_t timeout) {
    SPI_Init(modular);
    if(!skip) {
        ((SPI_AnalogHandleTypeDef *)modular->bushandle)->CS_Out(LOW);    //拉低片选
    }
    for(uint16_t j = 0; j < size; j++) {
        for(uint8_t i = 0x80; i; i >>= 1) {
            SPI_WriteBit(modular, pdata[j] & i ? 1 : 0);
        }
    }
    if(!skip) {
        ((SPI_AnalogHandleTypeDef *)modular->bushandle)->CS_Out(HIGH);    //拉高片选
    }
}
#endif    // SPI_USEMACRO
#undef SPI_USEMACRO
#endif
