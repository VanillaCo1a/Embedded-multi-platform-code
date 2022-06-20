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
typedef struct {
    uint64_t num;          //总线上的设备数量
    int8_t flag_search;    //总线在最近一段时间内是否进行过搜索以更新设备数量
    void (*OWIO_Set)(Direct_TypeDef);
    void (*OWIO_Out)(Potential_TypeDef);
    Potential_TypeDef (*OWIO_In)(void);
    void (*delayus)(uint16_t us);
    void (*delayms)(uint16_t ms);
} OWRE_AnalogTypedef;
typedef struct {
    uint64_t rom;
    OWRE_AnalogTypedef *method;
} OWRE_AnalogHandleTypeDef;

void MODULAR_OWRE_WriteByte(OWRE_AnalogHandleTypeDef *modular, uint8_t data, int8_t skiprom);
void MODULAR_OWRE_WriteWord(OWRE_AnalogHandleTypeDef *modular, uint16_t data, int8_t skiprom);
uint8_t *MODULAR_OWRE_Write(OWRE_AnalogHandleTypeDef *modular, uint8_t *pdata, uint16_t size, int8_t skiprom);
uint8_t MODULAR_OWRE_ReadBit(OWRE_AnalogHandleTypeDef *modular);
uint8_t MODULAR_OWRE_ReadByte(OWRE_AnalogHandleTypeDef *modular);
uint16_t MODULAR_OWRE_ReadWord(OWRE_AnalogHandleTypeDef *modular);
uint8_t *MODULAR_OWRE_Read(OWRE_AnalogHandleTypeDef *modular, uint8_t *pdata, uint16_t size);
#endif
