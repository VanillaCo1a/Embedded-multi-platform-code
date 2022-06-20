#include "spi.h"
#define Delay_Time() ;

void SPIWIRE_Init(SPI_AnalogHandleTypeDef *modular) {
    modular->method->CS_Out(HIGH);
    modular->method->SDO_Out(HIGH);
    modular->method->SCLK_Out(HIGH);
}
void SPIWIRE_WriteByte(SPI_AnalogHandleTypeDef *modular, uint8_t byte) {
    int8_t i = 0;
    for(i=0; i<8; i++) {
        modular->method->SCLK_Out(LOW);        //拉低SCL, 表示上一时钟周期结束
        if(byte&(0x80>>i)) {
            modular->method->SDO_Out(HIGH);
        }else {
            modular->method->SDO_Out(LOW);
        }
        modular->method->SCLK_Out(HIGH);
        Delay_Time();
    }
}
//单字节写入函数
void MODULAR_SPI_WriteByte(SPI_AnalogHandleTypeDef *modular, uint8_t data) {
    SPIWIRE_Init(modular);
    modular->method->CS_Out(LOW);              //拉低片选
    SPIWIRE_WriteByte(modular, data);
    modular->method->CS_Out(HIGH);             //拉高片选
}
//多字节写入函数
//为了使速度接近理论值, 尽可能地减少调用函数入栈出栈等步骤, 并且对数据进行连续写入处理
void MODULAR_SPI_Write(SPI_AnalogHandleTypeDef *modular, uint8_t *pdata, uint16_t size) {
    int8_t i = 0;
    uint16_t j = 0;
    modular->method->CS_Out(HIGH);
    modular->method->SDO_Out(HIGH);
    modular->method->SCLK_Out(HIGH);
    modular->method->CS_Out(LOW);              //拉低片选
    for(j=0; j<size; j++) {
        for(i=0; i<8; i++) {
            modular->method->SCLK_Out(LOW);    //拉低SCL, 表示上一时钟周期结束
            if(*pdata&(0x80>>i)) {
                modular->method->SDO_Out(HIGH);
            }else {
                modular->method->SDO_Out(LOW);
            }
            modular->method->SCLK_Out(HIGH);
            Delay_Time();
        }
        pdata++;
    }
    modular->method->CS_Out(HIGH);             //拉高片选
}
