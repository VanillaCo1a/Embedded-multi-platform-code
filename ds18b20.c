#include "ds18b20.h"

////////////////////////////////////////////////////////////////////////////
//      IO操作宏
#if (MCU_COMPILER == MCU_STM32FWLIB)        //固件库IO操作宏替换
#define GPIO_OUT_SET(X) GPIO_SetBits(DS18B20[_outputnum].Ds18b20io->GPIO_##X, DS18B20[_outputnum].Ds18b20io->PIN_##X)
#define GPIO_OUT_RESET(X) GPIO_ResetBits(DS18B20[_outputnum].Ds18b20io->GPIO_##X, DS18B20[_outputnum].Ds18b20io->PIN_##X)
#define GPIO_IN(X) GPIO_ReadInputDataBit(DS18B20[_outputnum].Ds18b20io->GPIO_##X, DS18B20[_outputnum].Ds18b20io->PIN_##X)
#define DEFINE_CLOCK_SET(X) RCC_APB2PeriphClockCmd(DS18B20[_outputnum].Ds18b20io->CLK_##X, ENABLE)
#define DEFINE_GPIO_SET(X) GPIO_Init(DS18B20[_outputnum].Ds18b20io->GPIO_##X, &GPIO_InitStructure)
#define DEFINE_PIN_SET(X) GPIO_Pin = DS18B20[_outputnum].Ds18b20io->PIN_##X
#elif (MCU_COMPILER == MCU_STM32HAL)        //HAL库IO操作宏替换
#define GPIO_OUT_SET(X) HAL_GPIO_WritePin(DS18B20[_outputnum].Ds18b20io->GPIO_##X, DS18B20[_outputnum].Ds18b20io->PIN_##X, GPIO_PIN_SET)
#define GPIO_OUT_RESET(X) HAL_GPIO_WritePin(DS18B20[_outputnum].Ds18b20io->GPIO_##X, DS18B20[_outputnum].Ds18b20io->PIN_##X, GPIO_PIN_RESET)
#define GPIO_IN(X) HAL_GPIO_ReadPin(DS18B20[_outputnum].Ds18b20io->GPIO_##X, DS18B20[_outputnum].Ds18b20io->PIN_##X)
#endif


////////////////////////////////////////////////////////////////////////////
//      各对象结构体定义
//DS18B20对象定义                      通信句柄, 引脚对象
Ds18b20_Typedef DS18B20[DS18B20_NUM] = { {NULL,	    NULL},
                                };
//DS18B20引脚对象定义, 该结构体数组内的元素与DS18B20数组元素按序号一一对应
Ds18b20io_Typedef DS18B20IO[DS18B20_NUM] = {0};
//DS18B20模拟通信对象定义
Onewire_AnalogTypedef DS18B20OW[DS18B20_NUM] = {0};



////////////////////////////////////////////////////////////////////////////
//      DS18B20输出流控制函数
static int8_t _outputnum;      //设置缓存区输出流向哪一个DS18B20
void DS18B20_setOutputStream(int8_t num) {
    _outputnum = num;
}
Ds18b20_Typedef DS18B20_getOutputStream(void) {
    return DS18B20[_outputnum];
}
int8_t DS18B20_getOutputNum(void) {
    return _outputnum;
}


////////////////////////////////////////////////////////////////////////////
//      DS18B20引脚配置
void DS18B20_ioDef(void) {
     #if (MCU_COMPILER == MCU_STM32FWLIB)
    DS18B20IO[0].CLK_io =    RCC_APB2Periph_GPIOC;
    DS18B20IO[0].GPIO_io =   GPIOC;
    DS18B20IO[0].PIN_io =    GPIO_Pin_13;
    #elif (MCU_COMPILER == MCU_STM32HAL)
    DS18B20IO[0].GPIO_io =   DS18B200_IO_GPIO_Port;
    DS18B20IO[0].PIN_io =    DS18B200_IO_Pin;
    #endif
}
void DS18B20_ioSet(void) {
    DS18B20[_outputnum].Ds18b20io = &DS18B20IO[_outputnum];
}
void DS18B20_gpioInit(void) {
    #if (MCU_COMPILER == MCU_STM32FWLIB)
	//0.定义一个配置GPIO的结构体变量
	GPIO_InitTypeDef GPIO_InitStructure;
	//1.初始化GPIO时钟
	RCC_APB2PeriphClockCmd(DS18B20IO[_outputnum].CLK_io, ENABLE);			//设置需要开启的APB2时钟; 此处设置GPIOBCD的时钟开启
	//2.1调用配置GPIO工作方式的库函数
	GPIO_InitStructure.GPIO_Pin = DS18B20IO[_outputnum].PIN_io;				//初始化所需引脚;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;	//设置GPIO的工作模式; 此处设置为推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//设置GPIO的工作频率; 此处设置为50Mhz
	//2.2将配置好GPIO设置的结构体变量传入初始化函数
	GPIO_Init(DS18B20IO[_outputnum].GPIO_io, &GPIO_InitStructure);			//设置要配置的GPIO;
	GPIO_OUT_SET(io);
    #elif (MCU_COMPILER == MCU_STM32HAL)
    GPIO_OUT_SET(io);
    #endif
}


////////////////////////////////////////////////////////////////////////////
//     ONEWIRE模拟通信配置
//模拟通信抽象类的实现函数, 通过'IO操作宏'适配不同编译环境, 不需要修改; 
//若只使用硬件通信, 不调用本部分函数即可, 未用到的代码段不会被编译
void DS18B20_IOSet(int8_t dir) {
    #if (MCU_COMPILER == MCU_STM32FWLIB)
    if(dir == OUT) {
        GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Pin = DS18B20IO[_outputnum].PIN_io;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(DS18B20_GPIO, &GPIO_InitStructure);
    }else {
        GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Pin = DS18B20IO[_outputnum].PIN_io;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
        GPIO_Init(DS18B20_GPIO, &GPIO_InitStructure);
    }
    #elif (MCU_COMPILER == MCU_STM32HAL)
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(dir == OUT) {
        GPIO_InitStruct.Pin = DS18B200_IO_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        HAL_GPIO_Init(DS18B200_IO_GPIO_Port, &GPIO_InitStruct);
    }else {
        GPIO_InitStruct.Pin = DS18B200_IO_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        HAL_GPIO_Init(DS18B200_IO_GPIO_Port, &GPIO_InitStruct);
    }
    #endif
}
void DS18B20_IOOut(uint8_t bit) {
	if(bit == HIGH) {
        GPIO_OUT_SET(io);
    }else {
		GPIO_OUT_RESET(io);
	}
}
uint8_t DS18B20_IOIn(void) {
	return GPIO_IN(io);
}
void DS18B20_Delayus(uint16_t us) {
    delayus_timer(us);
}
void DS18B20_Delayms(uint16_t ms) {
    delayms_timer(ms);
}
//通信类的实现函数, 初始化通信成员后将地址存入对应DS18B20结构体中
void DS18B20_wireSet(void) {
    static int8_t j = 0;
	DS18B20OW[j].onewireIOSet = DS18B20_IOSet;
    DS18B20OW[j].onewireIOOut = DS18B20_IOOut;
    DS18B20OW[j].onewireIOIn = DS18B20_IOIn;
    DS18B20OW[j].delayus = DS18B20_Delayus;
    DS18B20OW[j].delayms = DS18B20_Delayms;
	DS18B20[_outputnum].communication_handle = (Onewire_AnalogTypedef *)&DS18B20OW[j];
    j++;
}


////////////////////////////////////////////////////////////////////////////
//        DS18B20器件驱动函数
__IO int16_t temperature = 0;

void setTemperature(int16_t tem) {
	temperature = tem;
}
int16_t getTemperature(void) {
	return temperature;
}

void DS18B20_Ctrl(void) {
	setTemperature(DS18B20_GetTem());
}
void DS18B20_Confi(void) {
    //在调用DS18B20配置函数时, 一次性初始化所有DS18B20的引脚, 并复位定义RST脚的DS18B20
    DS18B20_ioDef();
    for(_outputnum=0; _outputnum<DS18B20_NUM; _outputnum++) {
        DS18B20_ioSet();
        DS18B20_wireSet();
    }
    for(_outputnum=0; _outputnum<DS18B20_NUM; _outputnum++) {
        DS18B20_gpioInit();
    }
    DS18B20_setOutputStream(0);
    ONEWIRE_Init((Onewire_AnalogTypedef *)DS18B20[_outputnum].communication_handle);
}
//开始温度转换
void DS18B20_StartTem(void) {
	ONEWIRE_Init((Onewire_AnalogTypedef *)DS18B20[_outputnum].communication_handle);
	ONEWIRE_Wait((Onewire_AnalogTypedef *)DS18B20[_outputnum].communication_handle);
	ONEWIRE_Write_Byte((Onewire_AnalogTypedef *)DS18B20[_outputnum].communication_handle, 0xcc);	//skip rom
	ONEWIRE_Write_Byte((Onewire_AnalogTypedef *)DS18B20[_outputnum].communication_handle, 0x44);	//convert
}
//从ds18b20得到温度值
//精度：0.1C
//返回值：温度值 （-550~1250） 
int16_t DS18B20_GetTem(void) {
	uint8_t temp;
	uint8_t TL,TH;
	int16_t tem;
	DS18B20_StartTem();								//ds18b20 start convert
	ONEWIRE_Init((Onewire_AnalogTypedef *)DS18B20[_outputnum].communication_handle);
	ONEWIRE_Wait((Onewire_AnalogTypedef *)DS18B20[_outputnum].communication_handle);
	ONEWIRE_Write_Byte((Onewire_AnalogTypedef *)DS18B20[_outputnum].communication_handle, 0xcc);	//skip rom
	ONEWIRE_Write_Byte((Onewire_AnalogTypedef *)DS18B20[_outputnum].communication_handle, 0xbe);	//convert
	TL = ONEWIRE_Read_Byte((Onewire_AnalogTypedef *)DS18B20[_outputnum].communication_handle); 		//LSB
	TH = ONEWIRE_Read_Byte((Onewire_AnalogTypedef *)DS18B20[_outputnum].communication_handle); 		//MSB 	  
	if(TH > 7) {
		TH = ~TH;
		TL = ~TL;
		temp = 0;				//温度为负
	}else {
		temp = 1;				//温度为正
	}
	tem = TH;					//获得高八位
	tem <<= 8;
	tem += TL;				//获得底八位
	tem = (float)tem*0.625f;		//转换
	if(temp) {
		return tem;			//返回温度值
	}else {
		return -tem;
	}
}
