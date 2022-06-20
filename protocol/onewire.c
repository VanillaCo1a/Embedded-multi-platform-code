#include "onewire.h"

//复位信号
void ONEWIRE_Init(Onewire_AnalogTypedef *modular) {
    modular->onewireIOSet(OUT);    //SET PG11 OUTPUT
    modular->onewireIOOut(LOW);    //拉低DQ
    modular->delayus(750);       //拉低750us
    modular->onewireIOOut(HIGH);   //DQ=1
    modular->delayus(15);        //15US
}
//等待应答
uint8_t ONEWIRE_Wait(Onewire_AnalogTypedef *modular) {
    uint8_t retry=0;
    modular->onewireIOSet(IN);    //SET PG11 INPUT
    while(modular->onewireIOIn() && retry<200) {
        retry++;
        modular->delayus(1);
    }
    if(retry>=200) {
        return 1;
    }
    else {
        retry=0;
    }
    while(!modular->onewireIOIn() && retry<240) {
        retry++;
        modular->delayus(1);
    }
    if(retry >= 240) {
        return 1;
    }else {
        return 0;
    }
}
//读取信号
uint8_t ONEWIRE_Read_Bit(Onewire_AnalogTypedef *modular) {
    uint8_t bit;
    modular->onewireIOSet(OUT);    //SET OUTPUT
    modular->onewireIOOut(LOW);
    modular->delayus(2);
    modular->onewireIOOut(HIGH);
    modular->onewireIOSet(IN);     //SET INPUT
    modular->delayus(12);
    if(modular->onewireIOIn()) {
        bit = 1;
    }else {
        bit = 0;
    }
  modular->delayus(50);
  return bit;
}

//字节读取
uint8_t ONEWIRE_Read_Byte(Onewire_AnalogTypedef *modular) {
    int8_t i = 0;
    uint8_t data = 0;
    for(i=0; i<8; i++) {
        data = (ONEWIRE_Read_Bit(modular)<<7) | (data>>1);
    }                            
    return data;
}
//字节写入
void ONEWIRE_Write_Byte(Onewire_AnalogTypedef *modular, int8_t data) {
    int8_t i;
    modular->onewireIOSet(OUT);
    for(i=0; i<8; i++) {
        if(data&0x01) {
            modular->onewireIOOut(LOW);    // Write 1
            modular->delayus(2);
            modular->onewireIOOut(HIGH);
            modular->delayus(60);
        }else {
            modular->onewireIOOut(LOW);    // Write 0
            modular->delayus(60);
            modular->onewireIOOut(HIGH);
            modular->delayus(2);
        }
        data >>= 1;
    }
}
