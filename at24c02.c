#include "at24c02.h"
/***    本文件包含: 
                IO操作宏
                各对象结构体定义
                AT24C02输出流控制函数
                MPU_AT24C02引脚配置
                I2C/SPI模拟通信配置
                I2C/SPI通信底层函数
                AT24C02器件驱动函数                  ***/
////////////////////////////////////////////////////////////////////////////
//      IO操作宏
#if (MCU_COMPILER == MCU_STM32FWLIB)        //固件库IO操作宏替换
#define GPIO_OUT_SET(X) GPIO_SetBits(AT24C02[DEVICE_getiostream(&at24c02)].at24c02io->GPIO_##X, AT24C02[DEVICE_getiostream(&at24c02)].at24c02io->PIN_##X)
#define GPIO_OUT_RESET(X) GPIO_ResetBits(AT24C02[DEVICE_getiostream(&at24c02)].at24c02io->GPIO_##X, AT24C02[DEVICE_getiostream(&at24c02)].at24c02io->PIN_##X)
#define GPIO_IN(X) GPIO_ReadInputDataBit(AT24C02[DEVICE_getiostream(&at24c02)].at24c02io->GPIO_##X, AT24C02[DEVICE_getiostream(&at24c02)].at24c02io->PIN_##X)
#define DEFINE_CLOCK_SET(X) RCC_APB2PeriphClockCmd(AT24C02[DEVICE_getiostream(&at24c02)].at24c02io->CLK_##X, ENABLE)
#define DEFINE_GPIO_SET(X) GPIO_Init(AT24C02[DEVICE_getiostream(&at24c02)].at24c02io->GPIO_##X, &GPIO_InitStructure)
#define DEFINE_PIN_SET(X) GPIO_Pin = AT24C02[DEVICE_getiostream(&at24c02)].at24c02io->PIN_##X
#elif (MCU_COMPILER == MCU_STM32HAL)        //HAL库IO操作宏替换
#define GPIO_OUT_SET(X) HAL_GPIO_WritePin(AT24C02[DEVICE_getiostream(&at24c02)].at24c02io->GPIO_##X, AT24C02[DEVICE_getiostream(&at24c02)].at24c02io->PIN_##X, GPIO_PIN_SET)
#define GPIO_OUT_RESET(X) HAL_GPIO_WritePin(AT24C02[DEVICE_getiostream(&at24c02)].at24c02io->GPIO_##X, AT24C02[DEVICE_getiostream(&at24c02)].at24c02io->PIN_##X, GPIO_PIN_RESET)
#define GPIO_IN(X) HAL_GPIO_ReadPin(AT24C02[DEVICE_getiostream(&at24c02)].at24c02io->GPIO_##X, AT24C02[DEVICE_getiostream(&at24c02)].at24c02io->PIN_##X)
#endif


////////////////////////////////////////////////////////////////////////////
//      各对象结构体定义
//AT24C02对象定义                               通信类型, 通信方式, 硬件地址, 通信句柄, 引脚对象
At24c02_Typedef AT24C02[AT24C02_NUM] = {    {AT24C02_I2C, AT24C02_ANALOG, AT24C02_I2CADDR1, NULL, NULL},
                                            };
//AT24C02引脚对象定义, 该结构体数组内的元素与AT24C02数组元素按序号一一对应
At24c02io_Typedef AT24C02IO[AT24C02_NUM] = {0};
//AT24C02模拟通信对象定义, 两个结构体数组内的元素分别与AT24C02D数组中使用I2C与SPI通信的元素一一对应(因此, 定义的数组实际上有一半是闲置的)
I2C_AnalogTypedef AT24C02I2C[AT24C02_NUM] = {0};
SPI_AnalogTypedef AT24C02SPI[AT24C02_NUM] = {0};

////////////////////////////////////////////////////////////////////////////
//      AT24C02输出流控制函数
Device_TypeDef at24c02;

////////////////////////////////////////////////////////////////////////////
//      MPU_AT24C02引脚配置
void AT24C02_ioDef(void) {
    //在此处设置所使用AT24C02的引脚(SCL_SCLK*,SDA_SDO*,RST,DC,CS), 未用到的引脚则不设置, 其默认无效
    //使用I2C通信时, 至少需定义[模拟I2C]SCL_SCLK*,SDA_SDO*/[硬件I2C]无;
    //使用SPI通信时, 至少需定义[模拟SPI]SCL_SCLK*,SDA_SDO*,DC/[硬件SPI]DC;
    #if (MCU_COMPILER == MCU_STM32FWLIB)
    AT24C02IO[0].CLK_scl_sclk =    RCC_APB2Periph_GPIOB;
    AT24C02IO[0].GPIO_scl_sclk =   GPIOB;
    AT24C02IO[0].PIN_scl_sclk =    GPIO_Pin_12;
    AT24C02IO[0].CLK_sda_sdo =     RCC_APB2Periph_GPIOB;
    AT24C02IO[0].GPIO_sda_sdo =    GPIOB;
    AT24C02IO[0].PIN_sda_sdo =     GPIO_Pin_13;     
    #elif (MCU_COMPILER == MCU_STM32HAL)
    AT24C02IO[0].GPIO_scl_sclk =   AT24C020_SCL_SCLK_GPIO_Port;
    AT24C02IO[0].PIN_scl_sclk =    AT24C020_SCL_SCLK_Pin;
    AT24C02IO[0].GPIO_sda_sdo =    AT24C020_SDA_SDO_GPIO_Port;
    AT24C02IO[0].PIN_sda_sdo =     AT24C020_SDA_SDO_Pin;
    #endif
}
void AT24C02_ioSet(void) {
    AT24C02[DEVICE_getiostream(&at24c02)].at24c02io = &AT24C02IO[DEVICE_getiostream(&at24c02)];
}
void AT24C02_gpioInit(void) {
    #if (MCU_COMPILER == MCU_STM32FWLIB)
    if(AT24C02[DEVICE_getiostream(&at24c02)].at24c02prop == AT24C02_I2C) {
        GPIO_InitTypeDef GPIO_InitStructure;    //定义一个配置GPIO的结构体变量
        if(AT24C02[DEVICE_getiostream(&at24c02)].at24c02comi == AT24C02_ANALOG) {
            DEFINE_CLOCK_SET(scl_sclk);         //初始化SCL
            GPIO_InitStructure.DEFINE_PIN_SET(scl_sclk);
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            DEFINE_GPIO_SET(scl_sclk);
            DEFINE_CLOCK_SET(sda_sdo);          //初始化SDA
            GPIO_InitStructure.DEFINE_PIN_SET(sda_sdo);
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            DEFINE_GPIO_SET(sda_sdo);
            GPIO_OUT_SET(scl_sclk);             //设置SCL,SDA输出电平
            GPIO_OUT_SET(sda_sdo);
        }else if(AT24C02[DEVICE_getiostream(&at24c02)].at24c02comi == AT24C02_HARDWARE) {
            //固件库的硬件I2C初始化,待补充
        }
    }
    #endif
}
////////////////////////////////////////////////////////////////////////////
//      I2C/SPI模拟通信配置
//模拟通信抽象类的实现函数, 通过'IO操作宏'适配不同编译环境, 不需要修改; 
//若只使用硬件通信, 不调用本部分函数即可, 未用到的代码段不会被编译
void AT24C02_SCLSet(int8_t dir) {}
void AT24C02_SDASet(int8_t dir) {}
void AT24C02_SCLOut(uint8_t bit) {
    if(bit == HIGH) {
        GPIO_OUT_SET(scl_sclk);
    }else {
        GPIO_OUT_RESET(scl_sclk);
    }
}
void AT24C02_SDAOut(uint8_t bit) {
    if(bit == HIGH) {
        GPIO_OUT_SET(sda_sdo);
    }else {
        GPIO_OUT_RESET(sda_sdo);
    }
}
uint8_t AT24C02_SDAIn(void) {
    return GPIO_IN(sda_sdo);
}
void AT24C02_Delayus(uint16_t us) {
    #if (MCU_COMPILER == MCU_STM32FWLIB)
    delayus_timer(us);
    #elif (MCU_COMPILER == MCU_STM32HAL)
    delayus_timer(us);
    #endif
}
//通信类的实现函数, 初始化通信成员后将地址存入对应AT24C02结构体中
void AT24C02_wireSet(void) {
    static int8_t j = 0, k = 0;
    if(AT24C02[DEVICE_getiostream(&at24c02)].at24c02prop == AT24C02_I2C) {
        if(AT24C02[DEVICE_getiostream(&at24c02)].at24c02comi == AT24C02_ANALOG) {
            AT24C02I2C[j].ADDRESS = AT24C02[DEVICE_getiostream(&at24c02)].at24c02addr==0x00 ? AT24C02_I2CADDR1 : AT24C02[DEVICE_getiostream(&at24c02)].at24c02addr;
            AT24C02I2C[j].i2cSCLSet = AT24C02_SCLSet;
            AT24C02I2C[j].i2cSDASet = AT24C02_SDASet;
            AT24C02I2C[j].i2cSCLOut = AT24C02_SCLOut;
            AT24C02I2C[j].i2cSDAOut = AT24C02_SDAOut;
            AT24C02I2C[j].i2cSDAIn = AT24C02_SDAIn;
            AT24C02I2C[j].delayus = AT24C02_Delayus;
            AT24C02[DEVICE_getiostream(&at24c02)].communication_handle = (I2C_AnalogTypedef *)&AT24C02I2C[j];
            j++;
        }
    }else if(AT24C02[DEVICE_getiostream(&at24c02)].at24c02prop == AT24C02_ONEWIRE) {
        k++;
    }
}
////////////////////////////////////////////////////////////////////////////
//      I2C/SPI通信底层函数
//AT24C02模拟通信写字节函数
void AT24C02_writeByte(uint8_t data, uint8_t address) {
    if(AT24C02[DEVICE_getiostream(&at24c02)].at24c02prop == AT24C02_I2C) {
        if(AT24C02[DEVICE_getiostream(&at24c02)].at24c02comi == AT24C02_ANALOG) {
            MODULAR_I2CWrite(((I2C_AnalogTypedef *)AT24C02[DEVICE_getiostream(&at24c02)].communication_handle), address, &data,1, 1,I2CHIGH);
        }else if(AT24C02[DEVICE_getiostream(&at24c02)].at24c02comi == AT24C02_HARDWARE) {
          #if (MCU_I2CCOM == MCU_HARDWARE)
            #if (MCU_COMPILER == MCU_STM32FWLIB)
            //固件库的硬件I2C驱动函数,待补充
            #elif (MCU_COMPILER == MCU_STM32HAL)
            HAL_I2C_Mem_Write(((I2C_HandleTypeDef *)AT24C02[DEVICE_getiostream(&at24c02)].communication_handle), (AT24C02[DEVICE_getiostream(&at24c02)].at24c02addr<<1)|0X00, address,I2C_MEMADD_SIZE_8BIT, &data,1, 0x100);
            //HAL_FMPI2C_Mem_Write(((I2C_HandleTypeDef *)AT24C02[DEVICE_getiostream(&at24c02)].communication_handle), (AT24C02[DEVICE_getiostream(&at24c02)].at24c02addr<<1)|0X00, address,FMPI2C_MEMADD_SIZE_8BIT, &data,1, 0x100);
            #endif
          #endif
        }
    }
}
//AT24C02模拟通信连续写多字节函数
void AT24C02_writeConti(uint8_t *pdata, uint16_t size, uint8_t address) {
    if(AT24C02[DEVICE_getiostream(&at24c02)].at24c02prop == AT24C02_I2C) {
        if(AT24C02[DEVICE_getiostream(&at24c02)].at24c02comi == AT24C02_ANALOG) {
            MODULAR_I2CWrite(((I2C_AnalogTypedef *)AT24C02[DEVICE_getiostream(&at24c02)].communication_handle), address, pdata,size, 1,I2CHIGH);
        }else if(AT24C02[DEVICE_getiostream(&at24c02)].at24c02comi == AT24C02_HARDWARE) {
          #if (MCU_I2CCOM == MCU_HARDWARE)
            #if (MCU_COMPILER == MCU_STM32FWLIB)
            //固件库的硬件I2C驱动函数,待补充
            #elif (MCU_COMPILER == MCU_STM32HAL)
            HAL_I2C_Mem_Write(((I2C_HandleTypeDef *)AT24C02[DEVICE_getiostream(&at24c02)].communication_handle), (AT24C02[DEVICE_getiostream(&at24c02)].at24c02addr<<1)|0X00, address,I2C_MEMADD_SIZE_8BIT, pdata,size, 0x100);
            //HAL_FMPI2C_Mem_Write(((I2C_HandleTypeDef *)AT24C02[DEVICE_getiostream(&at24c02)].communication_handle), (AT24C02[DEVICE_getiostream(&at24c02)].at24c02addr<<1)|0X00, address,FMPI2C_MEMADD_SIZE_8BIT, pdata,size, 0x100);
            #endif
          #endif
        }
    }
}
//AT24C02模拟通信读字节函数
uint8_t AT24C02_readByte(uint8_t address) {
    uint8_t data = 0;
    if(AT24C02[DEVICE_getiostream(&at24c02)].at24c02prop == AT24C02_I2C) {
        if(AT24C02[DEVICE_getiostream(&at24c02)].at24c02comi == AT24C02_ANALOG) {
            MODULAR_I2CRead(((I2C_AnalogTypedef *)AT24C02[DEVICE_getiostream(&at24c02)].communication_handle), address, &data,1, 1,I2CHIGH);
            return data;
        }else if(AT24C02[DEVICE_getiostream(&at24c02)].at24c02comi == AT24C02_HARDWARE) {
          #if (MCU_I2CCOM == MCU_HARDWARE)
            #if (MCU_COMPILER == MCU_STM32FWLIB)
            //固件库的硬件I2C驱动函数,待补充
            #elif (MCU_COMPILER == MCU_STM32HAL)
            HAL_I2C_Mem_Read(((I2C_HandleTypeDef *)AT24C02[DEVICE_getiostream(&at24c02)].communication_handle), (AT24C02[DEVICE_getiostream(&at24c02)].at24c02addr<<1)|0X00, address,I2C_MEMADD_SIZE_8BIT, &data,1, 0x100);
            //HAL_FMPI2C_Mem_Write(((I2C_HandleTypeDef *)AT24C02[DEVICE_getiostream(&at24c02)].communication_handle), (AT24C02[DEVICE_getiostream(&at24c02)].at24c02addr<<1)|0X00, address,FMPI2C_MEMADD_SIZE_8BIT, &data,1, 0x100);
            #endif
          #endif
        }
    }
    return data;
}
//AT24C02模拟通信连续读多字节函数
void AT24C02_readConti(uint8_t *pdata, uint16_t size, uint8_t address) {
    if(AT24C02[DEVICE_getiostream(&at24c02)].at24c02prop == AT24C02_I2C) {
        if(AT24C02[DEVICE_getiostream(&at24c02)].at24c02comi == AT24C02_ANALOG) {
            MODULAR_I2CRead(((I2C_AnalogTypedef *)AT24C02[DEVICE_getiostream(&at24c02)].communication_handle), address, pdata,size, 1,I2CHIGH);
        }else if(AT24C02[DEVICE_getiostream(&at24c02)].at24c02comi == AT24C02_HARDWARE) {
          #if (MCU_I2CCOM == MCU_HARDWARE)
            #if (MCU_COMPILER == MCU_STM32FWLIB)
            //固件库的硬件I2C驱动函数,待补充
            #elif (MCU_COMPILER == MCU_STM32HAL)
            HAL_I2C_Mem_Read(((I2C_HandleTypeDef *)AT24C02[DEVICE_getiostream(&at24c02)].communication_handle), (AT24C02[DEVICE_getiostream(&at24c02)].at24c02addr<<1)|0X00, address,I2C_MEMADD_SIZE_8BIT, pdata,size, 0x100);
            //HAL_FMPI2C_Mem_Write(((I2C_HandleTypeDef *)AT24C02[DEVICE_getiostream(&at24c02)].communication_handle), (AT24C02[DEVICE_getiostream(&at24c02)].at24c02addr<<1)|0X00, address,FMPI2C_MEMADD_SIZE_8BIT, pdata,size, 0x100);
            #endif
          #endif
        }
    }
}


////////////////////////////////////////////////////////////////////////////
//        AT24C02器件驱动函数
void AT24C02_Confi(void) {
     DEVICE_Init(&at24c02, AT24C02_NUM);
    //在调用AT24C02配置函数时, 一次性初始化所有AT24C02的引脚
    AT24C02_ioDef();
    for(uint8_t i=0; i<AT24C02_NUM; i++) {
        DEVICE_setiostream(&at24c02, i);
        AT24C02_ioSet();
        AT24C02_wireSet();
    }
    for(uint8_t i=0; i<AT24C02_NUM; i++) {
        DEVICE_setiostream(&at24c02, i);
        AT24C02_gpioInit();
    }
    DEVICE_setiostream(&at24c02, 0);
}
//EEPROM任意类型数据读写函数
//参数: 起始地址, 长度, 写入地址
//返回: 指针(成功)/空(失败)
/** 例:    uint16_t temp1 = 0;   double temp2 = 1;
    while(!AT24C02_ReadOrWrite(&temp,sizeof(temp1), 0, 1));
    while(!AT24C02_ReadOrWrite(&temp,sizeof(temp2), 0+sizeof(temp1), 1));
    while(!AT24C02_ReadOrWrite(&temp,sizeof(temp1), i, 1));
    while(!AT24C02_ReadOrWrite(&temp,sizeof(temp2), 0+sizeof(temp1), 0));
    printf("%d %lf", temp1, temp2);     ***/

void *AT24C02_ReadOrWrite(void *pdata, uint16_t size, uint8_t address, int8_t rw) {
    if(Device_getState(&at24c02) == busy) {  //设备忙, 返回NULL
        return NULL;
    }else {                         //EEPROM连续读写
        if(rw == 0) {
            AT24C02_readConti((uint8_t *)pdata,size, address);
        }else {
            AT24C02_writeConti((uint8_t *)pdata,size, address);
        }
        Device_setState(&at24c02, 500);
        return pdata;
    }
}
