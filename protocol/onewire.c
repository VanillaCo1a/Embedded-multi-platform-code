#include "onewire.h"

uint8_t OWRE_Init(OWRE_AnalogHandleTypeDef *modular) {
    uint8_t result = 0;
    modular->method->OWIO_Set(IN);
    result = modular->method->OWIO_In();    //如果总线没有被释放, 返回错误值
    if(result == 1) {
        return result;
    }
    modular->method->OWIO_Out(LOW);    //拉低总线480us, 发出复位信号
    modular->method->OWIO_Set(OUT);
    modular->method->delayus(480);
    //todo: 关中断      //应答信号最好在60-120us期间读取, 应关闭中断
    modular->method->OWIO_Out(HIGH);    //释放总线, 等待60us
    modular->method->delayus(60);
    modular->method->OWIO_Set(IN);          //读取总线信息
    result = modular->method->OWIO_In();    //若总线被拉低, 返回0, 否则返回1
    //todo: 开中断
    modular->method->delayus(420);
    modular->method->OWIO_Set(OUT);
    return result;
}
void OWRE_WriteBit(OWRE_AnalogHandleTypeDef *modular, uint8_t bit) {
    if(bit) {
        //todo: 关中断
        modular->method->OWIO_Out(LOW);
        modular->method->delayus(5);
        modular->method->OWIO_Out(HIGH);
        //todo: 开中断
        modular->method->delayus(55);
    } else {
        modular->method->OWIO_Out(LOW);
        modular->method->delayus(60);
        modular->method->OWIO_Out(HIGH);
        modular->method->delayus(1);
    }
}
uint8_t OWRE_ReadBit(OWRE_AnalogHandleTypeDef *modular) {
    uint8_t bit;
    //todo: 关中断
    modular->method->OWIO_Out(LOW);    //拉低总线1us, 开始读时隙
    //modular->method->delayus(1);      //1us的延时实际上可能有3-5us, 不需要
    modular->method->OWIO_Out(HIGH);    //释放总线
    modular->method->OWIO_Set(IN);
    modular->method->delayus(10);    //在15us内的最后时刻读取总线, 考虑到误差只延时10us
    bit = modular->method->OWIO_In();
    //todo: 开中断
    if(bit) {
        modular->method->delayus(50);
    } else {
        modular->method->delayus(51);
    }
    modular->method->OWIO_Set(OUT);
    return bit;
}
void OWRE_WriteByte(OWRE_AnalogHandleTypeDef *modular, uint8_t data) {
    for(uint8_t i = 0x01; i; i <<= 1) {
        OWRE_WriteBit(modular, data & i);
    }
}
uint8_t OWRE_ReadByte(OWRE_AnalogHandleTypeDef *modular) {
    uint8_t data = 0;
    for(uint8_t i = 0x01; i; i <<= 1) {
        data |= OWRE_ReadBit(modular) & i;
    }
    return data;
}
uint8_t *OWRE_Write(OWRE_AnalogHandleTypeDef *modular, uint8_t *pdata, uint16_t size) {
    for(uint16_t j = 0; j < size; j++) {
        for(uint8_t i = 0x01; i; i <<= 1) {
            OWRE_WriteBit(modular, pdata[j] & i);
        }
    }
    return pdata;
}
uint8_t *OWRE_Read(OWRE_AnalogHandleTypeDef *modular, uint8_t *pdata, uint16_t size) {
    for(uint16_t j = 0; j < size; j++) {
        for(uint8_t i = 0x01; i; i <<= 1) {
            pdata[j] |= OWRE_ReadBit(modular) & i;
        }
    }
    return pdata;
}

void OWRE_Skip(OWRE_AnalogHandleTypeDef *modular) {    //跳过ROM匹配指令
    OWRE_WriteByte(modular, 0xCC);
}
void OWRE_Match(OWRE_AnalogHandleTypeDef *modular) {    //ROM匹配指令
    OWRE_WriteByte(modular, 0x55);
    OWRE_Write(modular, (uint8_t *)&modular->rom, sizeof(modular->rom) / sizeof(uint8_t));
}
void OWRE_Query(OWRE_AnalogHandleTypeDef *modular) {    //[限单个设备]ROM读取指令
    OWRE_WriteByte(modular, 0x33);
}
void OWRE_Search(OWRE_AnalogHandleTypeDef *modular) {    //ROM搜索指令
    OWRE_WriteByte(modular, 0xF0);
}
void OWRE_AlarmSearch(OWRE_AnalogHandleTypeDef *modular) {    //报警ROM搜索指令
    OWRE_WriteByte(modular, 0xEC);
}

void MODULAR_OWRE_Search(OWRE_AnalogHandleTypeDef *modular) {
    OWRE_Init(modular);
    if(modular->method->num == 1) {
        OWRE_Query(modular);
    } else if(modular->method->num > 1){
        OWRE_Search(modular);
        //...
    }
}
void MODULAR_OWRE_AlarmSearch(OWRE_AnalogHandleTypeDef *modular) {
    OWRE_Init(modular);
    OWRE_AlarmSearch(modular);
    //...
}
void MODULAR_OWRE_WriteByte(OWRE_AnalogHandleTypeDef *modular, uint8_t data, int8_t skiprom) {
    OWRE_Init(modular);
    if(skiprom == 1 || modular->method->num == 1) {
        OWRE_Skip(modular);
    } else if(modular->method->num > 1) {
        OWRE_Match(modular);
    }
    OWRE_WriteByte(modular, data);
}
void MODULAR_OWRE_WriteWord(OWRE_AnalogHandleTypeDef *modular, uint16_t data, int8_t skiprom) {
    OWRE_Init(modular);
    if(skiprom == 1 || modular->method->num == 1) {
        OWRE_Skip(modular);
    } else if(modular->method->num > 1) {
        OWRE_Match(modular);
    }
    OWRE_WriteByte(modular, (uint8_t)data);
    OWRE_WriteByte(modular, (uint8_t)(data >> 8));
}
uint8_t *MODULAR_OWRE_Write(OWRE_AnalogHandleTypeDef *modular, uint8_t *pdata, uint16_t size, int8_t skiprom) {
    OWRE_Init(modular);
    if(skiprom == 1 || modular->method->num == 1) {
        OWRE_Skip(modular);
    } else if(modular->method->num > 1) {
        OWRE_Match(modular);
    }
    OWRE_Write(modular, pdata, size);
    return pdata;
}
uint8_t MODULAR_OWRE_ReadBit(OWRE_AnalogHandleTypeDef *modular) {
    return OWRE_ReadBit(modular);
}
uint8_t MODULAR_OWRE_ReadByte(OWRE_AnalogHandleTypeDef *modular) {
    return OWRE_ReadByte(modular);
}
uint16_t MODULAR_OWRE_ReadWord(OWRE_AnalogHandleTypeDef *modular) {
    return (uint16_t)OWRE_ReadByte(modular) | ((uint16_t)OWRE_ReadByte(modular) << 8);
}
uint8_t *MODULAR_OWRE_Read(OWRE_AnalogHandleTypeDef *modular, uint8_t *pdata, uint16_t size) {
    OWRE_Read(modular, pdata, size);
    return pdata;
}
