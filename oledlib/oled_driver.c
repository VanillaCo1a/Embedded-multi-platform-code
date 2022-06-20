#include "oled_driver.h"
/***    本文件包含:
            1. 各对象结构体定义
            2. MPU_OLED引脚配置
            3. I2C/SPI模拟通信配置
            4. I2C/SPI通信底层函数
            5. OLED器件驱动函数                  ***/

////////////////////////////////////////////////////////////////////////////
//    各对象结构体定义
//OLED对象定义                      通信类型, 通信方式, 芯片型号, 硬件地址, 通信句柄, 引脚对象
Oled_Typedef OLED[OLED_NUM] = {
    {OLED_SPI, OLED_HARDWARE, OLED_SH1106, 0x00, &hspi2, NULL},
    //{OLED_I2C, OLED_ANALOG,     OLED_SH1106,    OLED_I2CADDR1,  NULL,       NULL},
    //{OLED_I2C, OLED_HARDWARE,   OLED_SH1106,    OLED_I2CADDR1,  &hi2c1,     NULL},
};
//OLED引脚对象定义, 该结构体数组内的元素与OLED数组元素按序号一一对应
OLEDIO_TypeDef OLEDIO[OLED_NUM] = {0};
//OLED模拟通信对象定义, 两个结构体数组内的元素分别与OLEDD数组中使用I2C与SPI通信的元素一一对应(因此, 定义的数组实际上有一半是闲置的)
I2C_AnalogTypedef OLEDI2C[OLED_NUM] = {0};
SPI_AnalogTypedef OLEDSPI[OLED_NUM] = {0};
//OLED输出流控制
DEV_TypeDef oled;

////////////////////////////////////////////////////////////////////////////
//    OLED引脚配置
void OLED_ioDef(void) {
//在此处设置所使用OLED的引脚(SCL_SCLK*,SDA_SDO*,RST,DC,CS), 未用到的引脚则不设置, 其默认无效
//使用I2C通信时, 至少需定义[模拟I2C]SCL_SCLK*,SDA_SDO*/[硬件I2C]无; RST,DC,CS脚根据实际设计定义
//RST(复位,接主控芯片RST脚或直接接VCC),DC(I2C地址选择,接IO口或接VCC/地),CS脚(片选, 接IO口或直接接地)
//使用SPI通信时, 至少需定义[模拟SPI]SCL_SCLK*,SDA_SDO*,DC/[硬件SPI]DC; RST,CS脚根据实际设计定义
//RST(复位,接主控芯片RST脚或直接接VCC),DC(命令/数据选择,接IO口或接VCC/地),CS脚(片选, 接IO口或直接接地)
#if(MCU_COMPILER == MCU_STM32HAL)
    OLEDIO[0].SCL_SCLK.GPIOx = OLED0_SCL_SCLK_GPIO_Port;
    OLEDIO[0].SCL_SCLK.GPIO_Pin = OLED0_SCL_SCLK_Pin;
    OLEDIO[0].SDA_SDO.GPIOx = OLED0_SDA_SDO_GPIO_Port;
    OLEDIO[0].SDA_SDO.GPIO_Pin = OLED0_SDA_SDO_Pin;
    OLEDIO[0].CS.GPIOx = OLED0_CS_GPIO_Port;
    OLEDIO[0].CS.GPIO_Pin = OLED0_CS_Pin;
    OLEDIO[0].DC.GPIOx = OLED0_DC_GPIO_Port;
    OLEDIO[0].DC.GPIO_Pin = OLED0_DC_Pin;
#elif(MCU_COMPILER == MCU_STM32FWLIBF1)
    OLEDIO[0].SCL_SCLK.CLK = RCC_APB2Periph_GPIOB;
    OLEDIO[0].SCL_SCLK.GPIOx = GPIOB;
    OLEDIO[0].SCL_SCLK.GPIO_Pin = GPIO_Pin_12;
    OLEDIO[0].SDA_SDO.CLK = RCC_APB2Periph_GPIOB;
    OLEDIO[0].SDA_SDO.GPIOx = GPIOB;
    OLEDIO[0].SDA_SDO.GPIO_Pin = GPIO_Pin_13;
#endif
}
void OLED_ioSet(void) {
    OLED[DEV_getiostream(&oled)].oledio = &OLEDIO[DEV_getiostream(&oled)];
}
void OLED_gpioInit(void) {
#if(MCU_COMPILER == MCU_STM32FWLIBF1)
    if(OLED[DEV_getiostream(&oled)].oledprop == OLED_I2C) {
        GPIO_InitTypeDef GPIO_InitStructure;    //定义一个配置GPIO的结构体变量
        if(OLED[DEV_getiostream(&oled)].oledcomi == OLED_ANALOG) {
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;    //初始化SCL
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            DEV_CLK_GPIO_PIN_CONFI(OLED[DEV_getiostream(&oled)].oledio->SCL_SCLK);
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;    //初始化SDA
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            DEV_CLK_GPIO_PIN_CONFI(OLED[DEV_getiostream(&oled)].oledio->SDA_SDO);
        } else if(OLED[DEV_getiostream(&oled)].oledcomi == OLED_HARDWARE) {
            //固件库的硬件I2C初始化,待补充
        }
        //七脚oled使用I2C时还需复位RST(维持低电平200ms), 拉低CS, DC用于控制I2C地址
        if(OLED[DEV_getiostream(&oled)].oledio->RST.GPIOx != 0x00) {
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //初始化RST
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            DEV_CLK_GPIO_PIN_CONFI(OLED[DEV_getiostream(&oled)].oledio->RST);
        }
        if(OLED[DEV_getiostream(&oled)].oledio->DC.GPIOx != 0x00) {
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //初始化DC
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            DEV_CLK_GPIO_PIN_CONFI(OLED[DEV_getiostream(&oled)].oledio->DC);
        }
        if(OLED[DEV_getiostream(&oled)].oledio->CS.GPIOx != 0x00) {
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //初始化CS
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            DEV_CLK_GPIO_PIN_CONFI(OLED[DEV_getiostream(&oled)].oledio->CS);
        }
    } else if(OLED[DEV_getiostream(&oled)].oledprop == OLED_SPI) {
        GPIO_InitTypeDef GPIO_InitStructure;
        if(OLED[DEV_getiostream(&oled)].oledcomi == OLED_ANALOG) {
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;    //初始化SCL
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            DEV_CLK_GPIO_PIN_CONFI(OLED[DEV_getiostream(&oled)].oledio->SCL_SCLK);
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;    //初始化SDA
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            DEV_CLK_GPIO_PIN_CONFI(OLED[DEV_getiostream(&oled)].oledio->SDA_SDO);
        } else if(OLED[DEV_getiostream(&oled)].oledcomi == OLED_HARDWARE) {
            //固件库的硬件SPI初始化,待补充
        }
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //初始化DC
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        DEV_CLK_GPIO_PIN_CONFI(OLED[DEV_getiostream(&oled)].oledio->DC);
        if(OLED[DEV_getiostream(&oled)].oledio->RST.GPIOx != 0x00) {
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //初始化RST
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            DEV_CLK_GPIO_PIN_CONFI(OLED[DEV_getiostream(&oled)].oledio->RST);
        }
        if(OLED[DEV_getiostream(&oled)].oledio->CS.GPIOx != 0x00) {
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //初始化CS
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            DEV_CLK_GPIO_PIN_CONFI(OLED[DEV_getiostream(&oled)].oledio->CS);
        }
    }
#elif(MCU_COMPILER == MCU_STM32HAL)
//HAL库的IO初始化在main函数中完成
#endif
    //IO初始化电平配置, 通过调用多平台的IO操作函数实现
    if(OLED[DEV_getiostream(&oled)].oledprop == OLED_I2C) {
        if((OLED[DEV_getiostream(&oled)].oledcomi == OLED_ANALOG)) {
            DEV_GPIO_WritePin(OLED[DEV_getiostream(&oled)].oledio->SCL_SCLK, DEVIO_PIN_SET);
            DEV_GPIO_WritePin(OLED[DEV_getiostream(&oled)].oledio->SDA_SDO, DEVIO_PIN_SET);
        }
        if(OLED[DEV_getiostream(&oled)].oledio->RST.GPIOx != 0x00) {
            DEV_GPIO_WritePin(OLED[DEV_getiostream(&oled)].oledio->RST, DEVIO_PIN_RESET);
        }
        if(OLED[DEV_getiostream(&oled)].oledio->DC.GPIOx != 0x00) {
            if(OLED[DEV_getiostream(&oled)].oledaddr == OLED_I2CADDR1) {
                DEV_GPIO_WritePin(OLED[DEV_getiostream(&oled)].oledio->DC, DEVIO_PIN_RESET);
            } else {
                DEV_GPIO_WritePin(OLED[DEV_getiostream(&oled)].oledio->DC, DEVIO_PIN_SET);
            }
        }
        if(OLED[DEV_getiostream(&oled)].oledio->CS.GPIOx != 0x00) {
            DEV_GPIO_WritePin(OLED[DEV_getiostream(&oled)].oledio->CS, DEVIO_PIN_SET);
        }
    } else if(OLED[DEV_getiostream(&oled)].oledprop == OLED_SPI) {
        if((OLED[DEV_getiostream(&oled)].oledcomi == OLED_ANALOG)) {
            DEV_GPIO_WritePin(OLED[DEV_getiostream(&oled)].oledio->SCL_SCLK, DEVIO_PIN_SET);
            DEV_GPIO_WritePin(OLED[DEV_getiostream(&oled)].oledio->SDA_SDO, DEVIO_PIN_SET);
        }
        DEV_GPIO_WritePin(OLED[DEV_getiostream(&oled)].oledio->DC, DEVIO_PIN_SET);
        if(OLED[DEV_getiostream(&oled)].oledio->RST.GPIOx != 0x00) {
            DEV_GPIO_WritePin(OLED[DEV_getiostream(&oled)].oledio->RST, DEVIO_PIN_RESET);
        }
        if(OLED[DEV_getiostream(&oled)].oledio->CS.GPIOx != 0x00) {
            DEV_GPIO_WritePin(OLED[DEV_getiostream(&oled)].oledio->CS, DEVIO_PIN_SET);
        }
    }
}

////////////////////////////////////////////////////////////////////////////
//    I2C/SPI模拟通信配置
//模拟通信抽象类的实现函数, 不需要改动;
//若只使用硬件通信, 不调用本部分函数即可, 未用到的代码段不会被编译
void OLED_SCLSet(int8_t dir) {}
void OLED_SDASet(int8_t dir) {}
void OLED_SCLOut(uint8_t bit) {
    if(bit == HIGH) {
        DEV_GPIO_WritePin(OLED[DEV_getiostream(&oled)].oledio->SCL_SCLK, DEVIO_PIN_SET);
    } else {
        DEV_GPIO_WritePin(OLED[DEV_getiostream(&oled)].oledio->SCL_SCLK, DEVIO_PIN_RESET);
    }
}
void OLED_SDAOut(uint8_t bit) {
    if(bit == HIGH) {
        DEV_GPIO_WritePin(OLED[DEV_getiostream(&oled)].oledio->SDA_SDO, DEVIO_PIN_SET);
    } else {
        DEV_GPIO_WritePin(OLED[DEV_getiostream(&oled)].oledio->SDA_SDO, DEVIO_PIN_RESET);
    }
}
uint8_t OLED_SDAIn(void) {
    return !((DEV_GPIO_ReadPin(OLED[DEV_getiostream(&oled)].oledio->SDA_SDO) == DEVIO_PIN_RESET) ^ LOW);
}
void OLED_SCLKOut(uint8_t bit) {
    if(bit == HIGH) {
        DEV_GPIO_WritePin(OLED[DEV_getiostream(&oled)].oledio->SCL_SCLK, DEVIO_PIN_SET);
    } else {
        DEV_GPIO_WritePin(OLED[DEV_getiostream(&oled)].oledio->SCL_SCLK, DEVIO_PIN_RESET);
    }
}
void OLED_SDOOut(uint8_t bit) {
    if(bit == HIGH) {
        DEV_GPIO_WritePin(OLED[DEV_getiostream(&oled)].oledio->SDA_SDO, DEVIO_PIN_SET);
    } else {
        DEV_GPIO_WritePin(OLED[DEV_getiostream(&oled)].oledio->SDA_SDO, DEVIO_PIN_RESET);
    }
}
void OLED_CSOut(uint8_t bit) {
    if(OLED[DEV_getiostream(&oled)].oledio->CS.GPIOx != 0x00) {
        if(bit == HIGH) {
            DEV_GPIO_WritePin(OLED[DEV_getiostream(&oled)].oledio->CS, DEVIO_PIN_SET);
        } else {
            DEV_GPIO_WritePin(OLED[DEV_getiostream(&oled)].oledio->CS, DEVIO_PIN_RESET);
        }
    }
}
void OLED_Delayus(uint16_t us) {
#if(MCU_COMPILER == MCU_STM32HAL)
    delayus_timer(us);
#elif(MCU_COMPILER == MCU_STM32FWLIBF1)
    delayus_timer(us);
#endif
}
//通信类的实现函数, 初始化通信成员后将地址存入对应OLED结构体中
void OLED_wireSet(void) {
    static int8_t j = 0, k = 0;

    if(OLED[DEV_getiostream(&oled)].oledprop == OLED_I2C) {
        if(OLED[DEV_getiostream(&oled)].oledcomi == OLED_ANALOG) {
            OLEDI2C[j].ADDRESS = OLED[DEV_getiostream(&oled)].oledaddr == 0x00 ? OLED_I2CADDR1 : OLED[DEV_getiostream(&oled)].oledaddr;
            OLEDI2C[j].i2cSCLSet = OLED_SCLSet;
            OLEDI2C[j].i2cSDASet = OLED_SDASet;
            OLEDI2C[j].i2cSCLOut = OLED_SCLOut;
            OLEDI2C[j].i2cSDAOut = OLED_SDAOut;
            OLEDI2C[j].i2cSDAIn = OLED_SDAIn;
            OLEDI2C[j].delayus = OLED_Delayus;
            OLED[DEV_getiostream(&oled)].communication_handle = (I2C_AnalogTypedef *)&OLEDI2C[j];
            j++;
        }
    } else if(OLED[DEV_getiostream(&oled)].oledprop == OLED_SPI) {
        if(OLED[DEV_getiostream(&oled)].oledcomi == OLED_ANALOG) {
            OLEDSPI[k].spiSCLKOut = OLED_SCLKOut;
            OLEDSPI[k].spiSDOOut = OLED_SDOOut;
            OLEDSPI[k].spiCSOut = OLED_CSOut;
            OLEDSPI[k].delayus = OLED_Delayus;
            OLED[DEV_getiostream(&oled)].communication_handle = (SPI_AnalogTypedef *)&OLEDSPI[k];
            k++;
        }
    }
}

////////////////////////////////////////////////////////////////////////////
//    I2C/SPI通信底层函数
//OLED模拟通信写字节函数
//对于不同的通信方式的OLED, 区别主要在于此处的IO操作, 上层的操作基本相同
void OLED_writeByte(uint8_t data, uint8_t address) {
    if(OLED[DEV_getiostream(&oled)].oledprop == OLED_I2C) {
        if(OLED[DEV_getiostream(&oled)].oledio->CS.GPIOx != 0x00) {
            DEV_GPIO_WritePin(OLED[DEV_getiostream(&oled)].oledio->CS, DEVIO_PIN_RESET);
        }
        if(OLED[DEV_getiostream(&oled)].oledcomi == OLED_ANALOG) {
            MODULAR_I2CWrite(((I2C_AnalogTypedef *)OLED[DEV_getiostream(&oled)].communication_handle), address, &data, 1, 0, I2CHIGH);
        } else if(OLED[DEV_getiostream(&oled)].oledcomi == OLED_HARDWARE) {
#if(MCU_I2CCOM == MCU_BOTH)
#if(MCU_COMPILER == MCU_STM32FWLIBF1)
            //固件库的硬件I2C驱动函数,待补充
#elif(MCU_COMPILER == MCU_STM32HAL)
            HAL_I2C_Mem_Write(((I2C_HandleTypeDef *)OLED[DEV_getiostream(&oled)].communication_handle), (OLED[DEV_getiostream(&oled)].oledaddr << 1) | 0X00, address, I2C_MEMADD_SIZE_8BIT, &data, 1, 0x100);
            //HAL_FMPI2C_Mem_Write(((FMPI2C_HandleTypeDef *)OLED[DEV_getiostream(&oled)].communication_handle), (OLED[DEV_getiostream(&oled)].oledaddr << 1) | 0X00, address, FMPI2C_MEMADD_SIZE_8BIT, &data, 1, 0x100);
#endif
#endif
        }
        if(OLED[DEV_getiostream(&oled)].oledio->CS.GPIOx != 0x00) {
            DEV_GPIO_WritePin(OLED[DEV_getiostream(&oled)].oledio->CS, DEVIO_PIN_SET);
        }
    } else if(OLED[DEV_getiostream(&oled)].oledprop == OLED_SPI) {
        if(address == 0X40) {
            DEV_GPIO_WritePin(OLED[DEV_getiostream(&oled)].oledio->DC, DEVIO_PIN_SET);
        } else {
            DEV_GPIO_WritePin(OLED[DEV_getiostream(&oled)].oledio->DC, DEVIO_PIN_RESET);
        }
        if(OLED[DEV_getiostream(&oled)].oledcomi == OLED_ANALOG) {
            MODULAR_SPIWriteByte(OLED[DEV_getiostream(&oled)].communication_handle, data);
        } else if(OLED[DEV_getiostream(&oled)].oledcomi == OLED_HARDWARE) {
#if(MCU_SPICOM == MCU_BOTH)
            if(OLED[DEV_getiostream(&oled)].oledio->CS.GPIOx != 0x00) {
                DEV_GPIO_WritePin(OLED[DEV_getiostream(&oled)].oledio->CS, DEVIO_PIN_RESET);
            }
#if(MCU_COMPILER == MCU_STM32FWLIBF1)
            //固件库的硬件SPI驱动函数,待补充
#elif(MCU_COMPILER == MCU_STM32HAL)
            HAL_SPI_Transmit(((SPI_HandleTypeDef *)OLED[DEV_getiostream(&oled)].communication_handle), &data, 1, 0x100);
#endif
            if(OLED[DEV_getiostream(&oled)].oledio->CS.GPIOx != 0x00) {
                DEV_GPIO_WritePin(OLED[DEV_getiostream(&oled)].oledio->CS, DEVIO_PIN_SET);
            }
#endif
        }
        DEV_GPIO_WritePin(OLED[DEV_getiostream(&oled)].oledio->DC, DEVIO_PIN_SET);
    }
}
//OLED模拟通信连续写多字节函数
void OLED_writeConti(uint8_t *pdata, uint16_t size, uint8_t address) {
    if(OLED[DEV_getiostream(&oled)].oledprop == OLED_I2C) {
        if(OLED[DEV_getiostream(&oled)].oledio->CS.GPIOx != 0x00) {
            DEV_GPIO_WritePin(OLED[DEV_getiostream(&oled)].oledio->CS, DEVIO_PIN_RESET);
        }
        if(OLED[DEV_getiostream(&oled)].oledcomi == OLED_ANALOG) {
            MODULAR_I2CWrite(((I2C_AnalogTypedef *)OLED[DEV_getiostream(&oled)].communication_handle), address, pdata, size, 0, I2CHIGH);
        } else if(OLED[DEV_getiostream(&oled)].oledcomi == OLED_HARDWARE) {
#if(MCU_I2CCOM == MCU_BOTH)
#if(MCU_COMPILER == MCU_STM32FWLIBF1)
            //固件库的硬件I2C驱动函数,待补充
#elif(MCU_COMPILER == MCU_STM32HAL)
            HAL_I2C_Mem_Write(((I2C_HandleTypeDef *)OLED[DEV_getiostream(&oled)].communication_handle), (OLED[DEV_getiostream(&oled)].oledaddr << 1) | 0X00, address, I2C_MEMADD_SIZE_8BIT, pdata, size, 0x100);
            //HAL_FMPI2C_Mem_Write(((FMPI2C_HandleTypeDef *)OLED[DEV_getiostream(&oled)].communication_handle), (OLED[DEV_getiostream(&oled)].oledaddr << 1) | 0X00, address, FMPI2C_MEMADD_SIZE_8BIT, pdata, size, 0x100);
#endif
#endif
        }
        if(OLED[DEV_getiostream(&oled)].oledio->CS.GPIOx != 0x00) {
            DEV_GPIO_WritePin(OLED[DEV_getiostream(&oled)].oledio->CS, DEVIO_PIN_SET);
        }
    } else if(OLED[DEV_getiostream(&oled)].oledprop == OLED_SPI) {
        if(address == 0X40) {
            DEV_GPIO_WritePin(OLED[DEV_getiostream(&oled)].oledio->DC, DEVIO_PIN_SET);
        } else {
            DEV_GPIO_WritePin(OLED[DEV_getiostream(&oled)].oledio->DC, DEVIO_PIN_RESET);
        }
        if(OLED[DEV_getiostream(&oled)].oledcomi == OLED_ANALOG) {
            MODULAR_SPIWrite(OLED[DEV_getiostream(&oled)].communication_handle, pdata, size);
        } else if(OLED[DEV_getiostream(&oled)].oledcomi == OLED_HARDWARE) {
#if(MCU_SPICOM == MCU_BOTH)
            if(OLED[DEV_getiostream(&oled)].oledio->CS.GPIOx != 0x00) {
                DEV_GPIO_WritePin(OLED[DEV_getiostream(&oled)].oledio->CS, DEVIO_PIN_RESET);
            }
#if(MCU_COMPILER == MCU_STM32FWLIBF1)
            //固件库的硬件SPI驱动函数,待补充
#elif(MCU_COMPILER == MCU_STM32HAL)
            HAL_SPI_Transmit(((SPI_HandleTypeDef *)OLED[DEV_getiostream(&oled)].communication_handle), pdata, size, 0x100);
#endif
            if(OLED[DEV_getiostream(&oled)].oledio->CS.GPIOx != 0x00) {
                DEV_GPIO_WritePin(OLED[DEV_getiostream(&oled)].oledio->CS, DEVIO_PIN_SET);
            }
#endif
        }
        DEV_GPIO_WritePin(OLED[DEV_getiostream(&oled)].oledio->DC, DEVIO_PIN_SET);
    }
}

////////////////////////////////////////////////////////////////////////////
//    OLED器件驱动函数
//OLED的驱动函数, 将前面的配置IO口,通信等操作封装集成为OLED驱动, 供外部调用
void OLED_moduleReset(void) {
    if(OLED[DEV_getiostream(&oled)].oledio->RST.GPIOx != 0x00) {
        DEV_GPIO_WritePin(OLED[DEV_getiostream(&oled)].oledio->RST, DEVIO_PIN_RESET);
        OLED_delayms(200);
        DEV_GPIO_WritePin(OLED[DEV_getiostream(&oled)].oledio->RST, DEVIO_PIN_SET);
    }
}
void OLED_moduleOn(void) {         //OLED唤醒
    OLED_writeByte(0X8D, 0X00);    //设置电荷泵
    OLED_writeByte(0X14, 0X00);    //开启电荷泵
    OLED_writeByte(0XAF, 0X00);    //开启OLED显示
}
void OLED_moduleOff(void) {        //OLED休眠
    OLED_writeByte(0XAE, 0X00);    //关闭OLED显示
    OLED_writeByte(0X8D, 0X00);    //设置电荷泵
    OLED_writeByte(0X10, 0X00);    //关闭电荷泵
}
void OLED_moduleFlip(int8_t horizontally, int8_t vertically) {
    if(horizontally == 0) {
        OLED_writeByte(0xA1, 0X00);
    } else {
        OLED_writeByte(0xA0, 0X00);
    }
    if(vertically == 0) {
        OLED_writeByte(0xC8, 0X00);
    } else {
        OLED_writeByte(0xC0, 0X00);
    }
}
//设置器件内部光标, 格式为(页,列)
void OLED_moduleCursor(uint8_t page, uint8_t col) {
    OLED_writeByte(0xB0 + page, 0X00);    //设置显示起始页地址（0~7）
    if(OLED[DEV_getiostream(&oled)].oledchip == OLED_SSD1306) {
        OLED_writeByte(0x00 + (col & 0x0F), 0X00);    //设置显示起始列地址低4位
    } else if(OLED[DEV_getiostream(&oled)].oledchip == OLED_SH1106) {
        OLED_writeByte(0x02 + (col & 0x0F), 0X00);    //设置显示起始列地址低4位
    }
    OLED_writeByte(0x10 + ((col >> 4) & 0x0F), 0X00);    //设置显示起始列地址高4位
}
//在指定坐标处直接刷新一个字节(页中的某一列,上低下高), 由于要重设光标,非常地缓慢
void OLED_directByte(uint8_t page, uint8_t col, uint8_t data) {
    OLED_moduleCursor(page, col);
    OLED_writeByte(data, 0X40);
}
//使用I2C/SPI从缓存区读取对应位置数据, 用连续页写入的方式刷新整个屏幕
void OLED_fillScreen(uint8_t Buffer[]) {
    int8_t i = 0;
    uint8_t *pdata = Buffer;
    for(i = 0; i < SCREEN_PAGE; i++) {
        OLED_moduleCursor(i, 0);
        OLED_writeConti(pdata, SCREEN_COLUMN, 0X40);
        pdata += SCREEN_COLUMN;
    }
}
//清屏(先清空缓存区, 再更新到屏幕)
void OLED_clearScreen(void) {
    OLED_clearBuffer();
    OLED_updateScreen(DEV_getiostream(&oled));
}
//配置器件内部寄存器  initial settings configuration
void OLED_moduleInit(void) {
    //1. 基本命令
    OLED_writeByte(0xAE, 0X00);    //设置显示关(默认)/开: 0xAE显示关闭(睡眠模式),0xAF显示正常开启  Set Display OFF(RESET)/ON: 0xAE Display OFF(sleep mode),0xAF Display ON in normal mode
    OLED_writeByte(0xA4, 0X00);    //设置从内存(默认)/完全显示: 0xA4从内存中显示,0xA5完全显示  Entire Display OFF(RESET)/ON: 0xA4 Output follows RAM content,0xA5 Output ignores RAM content
    OLED_writeByte(0xA6, 0X00);    //设置正常(默认)/反向显示: 0xA6内存中0关闭显示1开启显示,0xA7内存中1关闭显示0开启显示  Set Normal/Inverse Display: 0xA6 Normal display,0xA7 Inverse display
    OLED_writeByte(0x81, 0X00);    //设置对比度(默认0x7F)  Set Contrast Control
    OLED_writeByte(0xFF, 0X00);    //对比度范围为0~255(0x00~0xFF)
    //2. 滚动命令
    //...
    //3. 寻址设置命令
    OLED_writeByte(0x20, 0X00);    //设置内存寻址模式(默认0x10) Set Memory Addressing Mode
    OLED_writeByte(0x02, 0X00);    //0x00水平寻址模式,0x01垂直寻址模式,0x02页面寻址模式(默认),其他无效  0x00 Horizontal Addressing Mode,0x01 Vertical Addressing Mode,0x02 Page Addressing Mode,others Invalid
    OLED_writeByte(0xB0, 0X00);    //设置页起始地址(0xB0~0xB7,仅页寻址模式有效)  Set Page Start Address for Page Addressing Mode
    if(OLED[DEV_getiostream(&oled)].oledchip == OLED_SSD1306) {
        OLED_writeByte(0x00, 0X00);    //设置显示起始列地址低4位
    } else if(OLED[DEV_getiostream(&oled)].oledchip == OLED_SH1106) {
        OLED_writeByte(0x02, 0X00);    //设置显示起始列地址低4位
    }
    OLED_writeByte(0x10, 0X00);    //设置列起始地址高4位(默认0x10,0x10~0x1F,仅页寻址模式有效)  Set Higher Column Start Address for Page Addressing Mode
                                   //命令0x21,0x22仅水平寻址和垂直寻址模式有效
    //4. 硬件配置命令(面板分辨率和布局相关)
    OLED_writeByte(0x40, 0X00);    //设置显示起始行地址(默认0x40,0x40~0x7F)  Set Display Start Line ADDRESS
    OLED_writeByte(0xA8, 0X00);    //设置复用比(默认0x3F)  Set Multiplex Ratio
    OLED_writeByte(0x3F, 0X00);    //复用比的范围为16MUX~64MUX,等于设置的数值+1(0x0F~0x3F)
    OLED_writeByte(0xD3, 0X00);    //设置显示偏移  Set Display Offset
    OLED_writeByte(0x00, 0X00);    //通过COM设置从0d到63d的垂直位移(默认0x00)
    OLED_writeByte(0xDA, 0X00);    //设置COM引脚硬件配置  Set COM Pins Hardware Configuration
    OLED_writeByte(0x12, 0X00);    //第5位0/1: 禁用(默认)/启用COM左右重新映射, 第4位0/1: 顺序COM引脚/可选COM引脚配置(默认)
    OLED_writeByte(0xA1, 0X00);    //(画面水平翻转控制,0xA1正常0xA0翻转)   设置段重映射: 0xA0列地址从0映射至SEG0,0xA1列地址从127映射至SEG0  Set Segment Re-map
    OLED_writeByte(0xC8, 0X00);    //(画面垂直翻转控制,0xC8正常0xC0翻转)   设置COM输出扫描方向: 0xC0普通模式,从COM0扫描至COM[N–1]; 0xC8重映射模式,从COM[N-1]扫描到COM0(其中N为复用比)  Set COM Output Scan Direction
    //5. 定时驱动方案设置命令
    OLED_writeByte(0xD5, 0X00);    //设置振荡器频率(默认0x8)和显示时钟分频比(默认0x0,即分频比为1)  Set Oscillator Frequency & Display Clock Divide Ratio
    OLED_writeByte(0xF0, 0X00);    //高4位: 设置晶振频率,FOSC频率随高4位数值增减而增减(0x0~0xF); 低4位: 设置DCLK的分频比,分频比=高4位数值+1设置Pre-charge间隔
    OLED_writeByte(0xD9, 0X00);    //设置Pre-charge间隔(默认0x22)  Set Pre-charge Period
    OLED_writeByte(0xF1, 0X00);    //高4位： 第2阶段间隔(0x1~0xF), 高4位： 第1阶段间隔时钟周期(0x1~0xF)
    OLED_writeByte(0xDB, 0X00);    //Set VCOMH(默认0x20)
    OLED_writeByte(0x30, 0X00);    //0x00 0.65xVcc, 0x20 0.77xVCC, 0x30 0.83xVCC
}
void OLED_delayms(uint16_t ms) {    //图形库普通的延时函数 需要用户自己配置
#if(MCU_COMPILER == MCU_STM32HAL)
    delayms_timer(ms);
    //HAL_Delay(ms);
#elif(MCU_COMPILER == MCU_STM32FWLIBF1)
    delayms_timer(ms);
#endif
}
void OLED_Confi(void) {
    DEV_Init(&oled, OLED_NUM);
    //在调用OLED配置函数时, 一次性初始化所有OLED的引脚, 并复位定义RST脚的OLED
    OLED_ioDef();
    for(uint8_t i = 0; i < OLED_NUM; i++) {
        DEV_setiostream(&oled, i);
        OLED_ioSet();
        OLED_wireSet();
    }
    for(uint8_t i = 0; i < OLED_NUM; i++) {
        DEV_setiostream(&oled, i);
        OLED_gpioInit();
    }
    for(uint8_t i = 0; i < OLED_NUM; i++) {
        DEV_setiostream(&oled, i);
        OLED_moduleReset();    //延时200ms等待OLED电源稳定
    }
    //对选中的OLED进行内部寄存器配置和清屏
    DEV_setiostream(&oled, 0);    //配置第1个OLED
    OLED_moduleInit();
    OLED_moduleCursor(0, 0);    //设置光标和屏幕方向, 在初始化函数中已经配置过, 单独分离出来方便修改
    OLED_moduleFlip(0, 0);
    OLED_clearScreen();    //清空屏幕
    OLED_moduleOn();       //开启屏幕显示
}
