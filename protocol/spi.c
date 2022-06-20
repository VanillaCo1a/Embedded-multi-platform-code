#include "spi.h"
#define Delay_Time() ;

void SPIWIRE_Init(Spi modular) {
	modular.spiCSOut(HIGH);
    modular.spiSDOOut(HIGH);
    modular.spiSCLKOut(HIGH);
}
void SPIWIRE_WriteByte(Spi modular, uint8_t byte) {
	int8_t i = 0;
	modular.spiCSOut(LOW);          //拉低片选
	for(i=0; i<8; i++) {
		modular.spiSCLKOut(LOW);    //拉低SCL, 表示上一时钟周期结束
		if(byte&(0x80>>i)) {
			modular.spiSDOOut(HIGH);
		}else {
			modular.spiSDOOut(LOW);
		}
		modular.spiSCLKOut(HIGH);
        Delay_Time();
	}
	modular.spiCSOut(HIGH);         //拉高片选
}
void MODULAR_SPIWriteByte(Spi modular, uint8_t data) {
	SPIWIRE_Init(modular);
	SPIWIRE_WriteByte(modular, data);
}
