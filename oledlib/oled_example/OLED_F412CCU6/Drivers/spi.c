#include "spi.h"
#define Delay_Time() ;

void SPIWIRE_Init(SPI_AnalogTypedef modular) {
    modular.spiCSOut(HIGH);
    modular.spiSDOOut(HIGH);
    modular.spiSCLKOut(HIGH);
}
void SPIWIRE_WriteByte(SPI_AnalogTypedef modular, uint8_t byte) {
    int8_t i = 0;
    for(i=0; i<8; i++) {
        modular.spiSCLKOut(LOW);        //拉低SCL, 表示上一时钟周期结束
        if(byte&(0x80>>i)) {
            modular.spiSDOOut(HIGH);
        }else {
            modular.spiSDOOut(LOW);
        }
        modular.spiSCLKOut(HIGH);
        Delay_Time();
    }
}
//单字节写入函数
void MODULAR_SPIWriteByte(SPI_AnalogTypedef modular, uint8_t data) {
    SPIWIRE_Init(modular);
    modular.spiCSOut(LOW);              //拉低片选
    SPIWIRE_WriteByte(modular, data);
    modular.spiCSOut(HIGH);             //拉高片选
}
//多字节写入函数
//为了使速度接近理论值, 尽可能地减少调用函数入栈出栈等步骤, 并且对数据进行连续写入处理
void MODULAR_SPIWrite(SPI_AnalogTypedef modular, uint8_t *pdata, uint16_t size) {
    int8_t i = 0;
    uint16_t j = 0;
    SPIWIRE_Init(modular);
    modular.spiCSOut(LOW);              //拉低片选
    for(j=0; j<size; j++) {
        for(i=0; i<8; i++) {
            modular.spiSCLKOut(LOW);    //拉低SCL, 表示上一时钟周期结束
            if(*pdata&(0x80>>i)) {
                modular.spiSDOOut(HIGH);
            }else {
                modular.spiSDOOut(LOW);
            }
            modular.spiSCLKOut(HIGH);
            Delay_Time();
        }
        pdata++;
    }
    modular.spiCSOut(HIGH);             //拉高片选
}
