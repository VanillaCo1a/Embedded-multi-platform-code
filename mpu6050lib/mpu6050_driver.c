#include "mpu6050_driver.h"

/***    I2C连续写            返回值: 0,正常; 其他,错误代码
    addr:器件地址 reg:寄存器地址 len:写入长度 buf:数据区    ***/
uint8_t MPU_Write_Len(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf) {
    MPU6050_writeConti(buf,len, reg);
    return 0;
}
/***    I2C连续读            返回值: 0,正常; 其他,错误代码
    addr:器件地址 reg:要读取的寄存器地址 len:要读取的长度 buf:读取到的数据存储区    ***/
uint8_t MPU_Read_Len(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf) {
    MPU6050_readConti(buf,len, reg);
    return 0;
}
/***    I2C写一个字节            返回值: 0,正常; 其他,错误代码
    reg: 寄存器地址 data:数据    ***/
uint8_t MPU_Write_Byte(uint8_t reg,uint8_t data) {
    MPU6050_writeConti(&data,1, reg);
    return 0;
}
/***    I2C读一个字节            返回值: 读到的数据
    reg: 寄存器地址    ***/
uint8_t MPU_Read_Byte(uint8_t reg) {
    uint8_t data = 0;
    MPU6050_readConti(&data,1, reg);
    return data;
}



/***    本文件包含: 
                IO操作宏
                各对象结构体定义
                MPU6050输出流控制函数
                MPU_MPU6050引脚配置
                I2C/SPI模拟通信配置
                I2C/SPI通信底层函数
                MPU6050器件驱动函数                  ***/
////////////////////////////////////////////////////////////////////////////
//      IO操作宏
#if (MCU_COMPILER == MCU_STM32FWLIB)        //固件库IO操作宏替换
#define GPIO_OUT_SET(X) GPIO_SetBits(MPU6050[_streamnum2].mpu6050io->GPIO_##X, MPU6050[_streamnum2].mpu6050io->PIN_##X)
#define GPIO_OUT_RESET(X) GPIO_ResetBits(MPU6050[_streamnum2].mpu6050io->GPIO_##X, MPU6050[_streamnum2].mpu6050io->PIN_##X)
#define GPIO_IN(X) GPIO_ReadInputDataBit(MPU6050[_streamnum2].mpu6050io->GPIO_##X, MPU6050[_streamnum2].mpu6050io->PIN_##X)
#define DEFINE_CLOCK_SET(X) RCC_APB2PeriphClockCmd(MPU6050[_streamnum2].mpu6050io->CLK_##X, ENABLE)
#define DEFINE_GPIO_SET(X) GPIO_Init(MPU6050[_streamnum2].mpu6050io->GPIO_##X, &GPIO_InitStructure)
#define DEFINE_PIN_SET(X) GPIO_Pin = MPU6050[_streamnum2].mpu6050io->PIN_##X
#elif (MCU_COMPILER == MCU_STM32HAL)        //HAL库IO操作宏替换
#define GPIO_OUT_SET(X) HAL_GPIO_WritePin(MPU6050[_streamnum2].mpu6050io->GPIO_##X, MPU6050[_streamnum2].mpu6050io->PIN_##X, GPIO_PIN_SET)
#define GPIO_OUT_RESET(X) HAL_GPIO_WritePin(MPU6050[_streamnum2].mpu6050io->GPIO_##X, MPU6050[_streamnum2].mpu6050io->PIN_##X, GPIO_PIN_RESET)
#define GPIO_IN(X) HAL_GPIO_ReadPin(MPU6050[_streamnum2].mpu6050io->GPIO_##X, MPU6050[_streamnum2].mpu6050io->PIN_##X)
#endif


////////////////////////////////////////////////////////////////////////////
//      各对象结构体定义
//MPU6050对象定义                               通信类型, 通信方式, 硬件地址, 通信句柄, 引脚对象
Mpu6050_Typedef MPU6050[MPU6050_NUM] = {    {MPU6050_I2C, MPU6050_ANALOG, MPU6050_I2CADDR1, NULL, NULL},
                                            };
//MPU6050引脚对象定义, 该结构体数组内的元素与MPU6050数组元素按序号一一对应
Mpu6050io_Typedef MPU6050IO[MPU6050_NUM] = {0};
//MPU6050模拟通信对象定义, 两个结构体数组内的元素分别与MPU6050D数组中使用I2C与SPI通信的元素一一对应(因此, 定义的数组实际上有一半是闲置的)
I2C_AnalogTypedef MPU6050I2C[MPU6050_NUM] = {0};

////////////////////////////////////////////////////////////////////////////
//      MPU6050输出流控制函数
int8_t _streamnum2 = 0;  //设置缓存区输出流向哪一个MPU6050
void MPU6050_setOutputStream(int8_t num) {
    _streamnum2 = num;
}
Mpu6050_Typedef MPU6050_getOutputStream(void) {
    return MPU6050[_streamnum2];
}
int8_t MPU6050_getOutputNum(void) {
    return _streamnum2;
}

////////////////////////////////////////////////////////////////////////////
//      MPU_MPU6050引脚配置
void MPU6050_ioDef(void) {
    //在此处设置所使用MPU6050的引脚(SCL_SCLK*,SDA_SDO*,RST,DC,CS), 未用到的引脚则不设置, 其默认无效
    //使用I2C通信时, 至少需定义[模拟I2C]SCL_SCLK*,SDA_SDO*/[硬件I2C]无;
    //使用SPI通信时, 至少需定义[模拟SPI]SCL_SCLK*,SDA_SDO*,DC/[硬件SPI]DC;
    #if (MCU_COMPILER == MCU_STM32FWLIB)
    MPU6050IO[0].CLK_scl_sclk =    MPU6050_SCL_CLK;
    MPU6050IO[0].GPIO_scl_sclk =   MPU6050_SCL_GPIO;
    MPU6050IO[0].PIN_scl_sclk =    MPU6050_SCL_PIN;
    MPU6050IO[0].CLK_sda_sdo =     MPU6050_SDA_CLK;
    MPU6050IO[0].GPIO_sda_sdo =    MPU6050_SDA_GPIO;
    MPU6050IO[0].PIN_sda_sdo =     MPU6050_SDA_PIN;
    #elif (MCU_COMPILER == MCU_STM32HAL)
    MPU6050IO[0].GPIO_scl_sclk =   MPU6050_SCL_GPIO_Port;
    MPU6050IO[0].PIN_scl_sclk =    MPU6050_SCL_Pin;
    MPU6050IO[0].GPIO_sda_sdo =    MPU6050_SDA_GPIO_Port;
    MPU6050IO[0].PIN_sda_sdo =     MPU6050_SDA_Pin;
    #endif
}
void MPU6050_ioSet(void) {
    MPU6050[_streamnum2].mpu6050io = &MPU6050IO[_streamnum2];
}
void MPU6050_GPIOInit(void) {
    #if (MCU_COMPILER == MCU_STM32FWLIB)
    if(MPU6050[_streamnum2].mpu6050prop == MPU6050_I2C) {
        if(MPU6050[_streamnum2].mpu6050comi == MPU6050_ANALOG) {
            GPIO_InitTypeDef GPIO_InitStructure;
            RCC_APB2PeriphClockCmd(MPU6050_SCL_CLK, ENABLE);
            GPIO_InitStructure.GPIO_Pin = MPU6050_SCL_PIN;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_Init(MPU6050_SCL_GPIO, &GPIO_InitStructure);
            RCC_APB2PeriphClockCmd(MPU6050_SDA_CLK, ENABLE);
            GPIO_InitStructure.GPIO_Pin = MPU6050_SDA_PIN;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_Init(MPU6050_SDA_GPIO, &GPIO_InitStructure);
            GPIO_SetBits(MPU6050_SCL_GPIO, MPU6050_SCL_PIN);
            GPIO_SetBits(MPU6050_SDA_GPIO, MPU6050_SDA_PIN);
            
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_Init(MPU6050_SDA_GPIO, &GPIO_InitStructure);
            GPIO_ResetBits(GPIOB, GPIO_Pin_8);
        }else if(MPU6050[_streamnum2].mpu6050comi == MPU6050_HARDWARE) {
            //固件库的硬件I2C初始化,待补充
        }
    }
    #elif (MCU_COMPILER == MCU_STM32HAL)
    //HAL库的IO初始化在main函数中完成
    if(MPU6050[_streamnum2].mpu6050prop == MPU6050_I2C) {
        if((MPU6050[_streamnum2].mpu6050comi == MPU6050_ANALOG)) {
            GPIO_OUT_SET(scl_sclk);
            GPIO_OUT_SET(sda_sdo);
        }
        if(MPU6050[_streamnum2].mpu6050io->GPIO_rst != NULL) {
            GPIO_OUT_RESET(rst);
        }
        if(MPU6050[_streamnum2].mpu6050io->GPIO_dc != NULL) {
            if(MPU6050[_streamnum2].mpu6050addr == MPU6050_I2CADDR1) {
                GPIO_OUT_RESET(dc);
            }else {
                GPIO_OUT_SET(dc);
            }
        }
        if(MPU6050[_streamnum2].mpu6050io->GPIO_cs != NULL) {
            GPIO_OUT_SET(cs);
        }
    }
    #endif
}
void MPU6050_NVICInit(void) {
    #if (MCU_COMPILER == MCU_STM32FWLIB)
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB2PeriphClockCmd(MPU6050_INT_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin = MPU6050_INT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(MPU6050_INT_GPIO, &GPIO_InitStructure);
    GPIO_SetBits(MPU6050_INT_GPIO, MPU6050_INT_PIN);    
    #ifndef NVICGROUP
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    #endif
    NVIC_InitStructure.NVIC_IRQChannel = MPU6050_INT_EXTISTATE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    #endif
}
void MPU6050_EXTIInit(void) {
    #if (MCU_COMPILER == MCU_STM32FWLIB)
    EXTI_InitTypeDef EXTI_InitStructure;
    //1.初始化GPIO时钟
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource9);
    EXTI_InitStructure.EXTI_Line = EXTI_Line9;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_Init(&EXTI_InitStructure);
    #endif
}
#if (MCU_COMPILER == MCU_STM32FWLIB)
void EXTI9_5_IRQHandler(void) {
    if(EXTI_GetITStatus(EXTI_Line9) != RESET) {
        EXTI_ClearITPendingBit(EXTI_Line9);
        flag_mpu6050 = 1;
    }
}
#elif (MCU_COMPILER == MCU_STM32HAL)
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if(GPIO_Pin == MPU6050_EXTI_Pin) {
        flag_mpu6050 = 1;
    }
}
#endif


////////////////////////////////////////////////////////////////////////////
//      I2C/SPI模拟通信配置
//模拟通信抽象类的实现函数, 通过'IO操作宏'适配不同编译环境, 不需要修改; 
//若只使用硬件通信, 不调用本部分函数即可, 未用到的代码段不会被编译
void MPU6050_SCLSet(int8_t dir) {}
void MPU6050_SDASet(int8_t dir) {
}
void MPU6050_SCLOut(uint8_t bit) {
    if(bit == HIGH) {
        GPIO_OUT_SET(scl_sclk);
    }else {
        GPIO_OUT_RESET(scl_sclk);
    }
}
void MPU6050_SDAOut(uint8_t bit) {
    if(bit == HIGH) {
        GPIO_OUT_SET(sda_sdo);
    }else {
        GPIO_OUT_RESET(sda_sdo);
    }
}
uint8_t MPU6050_SDAIn(void) {
    return GPIO_IN(sda_sdo);
}
void MPU6050_Delayus(int16_t nus) {
    #if (MCU_COMPILER == MCU_STM32FWLIB)
    Delay_us(nus);
    #elif (MCU_COMPILER == MCU_STM32HAL)
    int16_t i = 1;
    while(i--); 
    #endif
}
//通信类的实现函数, 初始化通信成员后将地址存入对应MPU6050结构体中
void MPU6050_wireSet(void) {
    static int8_t j = 0, k = 0;
    if(MPU6050[_streamnum2].mpu6050prop == MPU6050_I2C) {
        if(MPU6050[_streamnum2].mpu6050comi == MPU6050_ANALOG) {
            MPU6050I2C[j].ADDRESS = MPU6050[_streamnum2].mpu6050addr==0x00 ? MPU6050_I2CADDR1 : MPU6050[_streamnum2].mpu6050addr;
            MPU6050I2C[j].i2cSCLSet = MPU6050_SCLSet;
            MPU6050I2C[j].i2cSDASet = MPU6050_SDASet;
            MPU6050I2C[j].i2cSCLOut = MPU6050_SCLOut;
            MPU6050I2C[j].i2cSDAOut = MPU6050_SDAOut;
            MPU6050I2C[j].i2cSDAIn = MPU6050_SDAIn;
            MPU6050I2C[j].delayus = MPU6050_Delayus;
            MPU6050[_streamnum2].communication_handle = (I2C_AnalogTypedef *)&MPU6050I2C[j];
            j++;
        }
    }else if(MPU6050[_streamnum2].mpu6050prop == MPU6050_ONEWIRE) {
        k++;
    }
}
////////////////////////////////////////////////////////////////////////////
//      I2C/SPI通信底层函数
//MPU6050模拟通信写字节函数
void MPU6050_writeByte(uint8_t data, uint8_t address) {
    if(MPU6050[_streamnum2].mpu6050prop == MPU6050_I2C) {
        if(MPU6050[_streamnum2].mpu6050comi == MPU6050_ANALOG) {
            MODULAR_I2CWrite(((I2C_AnalogTypedef *)MPU6050[_streamnum2].communication_handle), address, &data,1, 1,I2CMEDIUM);
        }else if(MPU6050[_streamnum2].mpu6050comi == MPU6050_HARDWARE) {
          #if (MCU_I2CCOM == MCU_HARDWARE)
            #if (MCU_COMPILER == MCU_STM32FWLIB)
            //固件库的硬件I2C驱动函数,待补充
            #elif (MCU_COMPILER == MCU_STM32HAL)
            HAL_I2C_Mem_Write(((I2C_HandleTypeDef *)MPU6050[_streamnum2].communication_handle), (MPU6050[_streamnum2].mpu6050addr<<1)|0X00, address,I2C_MEMADD_SIZE_8BIT, &data,1, 0x100);
            //HAL_FMPI2C_Mem_Write(((I2C_HandleTypeDef *)MPU6050[_streamnum2].communication_handle), (MPU6050[_streamnum2].mpu6050addr<<1)|0X00, address,FMPI2C_MEMADD_SIZE_8BIT, &data,1, 0x100);
            #endif
          #endif
        }
    }
}
//MPU6050模拟通信连续写多字节函数
void MPU6050_writeConti(uint8_t *pdata, uint16_t size, uint8_t address) {
    if(MPU6050[_streamnum2].mpu6050prop == MPU6050_I2C) {
        if(MPU6050[_streamnum2].mpu6050comi == MPU6050_ANALOG) {
            MODULAR_I2CWrite(((I2C_AnalogTypedef *)MPU6050[_streamnum2].communication_handle), address, pdata,size, 1,I2CMEDIUM);
        }else if(MPU6050[_streamnum2].mpu6050comi == MPU6050_HARDWARE) {
          #if (MCU_I2CCOM == MCU_HARDWARE)
            #if (MCU_COMPILER == MCU_STM32FWLIB)
            //固件库的硬件I2C驱动函数,待补充
            #elif (MCU_COMPILER == MCU_STM32HAL)
            HAL_I2C_Mem_Write(((I2C_HandleTypeDef *)MPU6050[_streamnum2].communication_handle), (MPU6050[_streamnum2].mpu6050addr<<1)|0X00, address,I2C_MEMADD_SIZE_8BIT, pdata,size, 0x100);
            //HAL_FMPI2C_Mem_Write(((I2C_HandleTypeDef *)MPU6050[_streamnum2].communication_handle), (MPU6050[_streamnum2].mpu6050addr<<1)|0X00, address,FMPI2C_MEMADD_SIZE_8BIT, pdata,size, 0x100);
            #endif
          #endif
        }
    }
}
//MPU6050模拟通信读字节函数
uint8_t MPU6050_readByte(uint8_t address) {
    uint8_t data = 0;
    if(MPU6050[_streamnum2].mpu6050prop == MPU6050_I2C) {
        if(MPU6050[_streamnum2].mpu6050comi == MPU6050_ANALOG) {
            MODULAR_I2CRead(((I2C_AnalogTypedef *)MPU6050[_streamnum2].communication_handle), address, &data,1, 1,I2CMEDIUM);
            return data;
        }else if(MPU6050[_streamnum2].mpu6050comi == MPU6050_HARDWARE) {
          #if (MCU_I2CCOM == MCU_HARDWARE)
            #if (MCU_COMPILER == MCU_STM32FWLIB)
            //固件库的硬件I2C驱动函数,待补充
            #elif (MCU_COMPILER == MCU_STM32HAL)
            HAL_I2C_Mem_Read(((I2C_HandleTypeDef *)MPU6050[_streamnum2].communication_handle), (MPU6050[_streamnum2].mpu6050addr<<1)|0X00, address,I2C_MEMADD_SIZE_8BIT, &data,1, 0x100);
            //HAL_FMPI2C_Mem_Write(((I2C_HandleTypeDef *)MPU6050[_streamnum2].communication_handle), (MPU6050[_streamnum2].mpu6050addr<<1)|0X00, address,FMPI2C_MEMADD_SIZE_8BIT, &data,1, 0x100);
            #endif
          #endif
        }
    }
    return data;
}
//MPU6050模拟通信连续读多字节函数
void MPU6050_readConti(uint8_t *pdata, uint16_t size, uint8_t address) {
    if(MPU6050[_streamnum2].mpu6050prop == MPU6050_I2C) {
        if(MPU6050[_streamnum2].mpu6050comi == MPU6050_ANALOG) {
            MODULAR_I2CRead(((I2C_AnalogTypedef *)MPU6050[_streamnum2].communication_handle), address, pdata,size, 1,I2CMEDIUM);
        }else if(MPU6050[_streamnum2].mpu6050comi == MPU6050_HARDWARE) {
          #if (MCU_I2CCOM == MCU_HARDWARE)
            #if (MCU_COMPILER == MCU_STM32FWLIB)
            //固件库的硬件I2C驱动函数,待补充
            #elif (MCU_COMPILER == MCU_STM32HAL)
            HAL_I2C_Mem_Read(((I2C_HandleTypeDef *)MPU6050[_streamnum2].communication_handle), (MPU6050[_streamnum2].mpu6050addr<<1)|0X00, address,I2C_MEMADD_SIZE_8BIT, pdata,size, 0x100);
            //HAL_FMPI2C_Mem_Write(((I2C_HandleTypeDef *)MPU6050[_streamnum2].communication_handle), (MPU6050[_streamnum2].mpu6050addr<<1)|0X00, address,FMPI2C_MEMADD_SIZE_8BIT, pdata,size, 0x100);
            #endif
          #endif
        }
    }
}


////////////////////////////////////////////////////////////////////////////
//        MPU6050器件驱动函数
/***    初始化MPU6050  返回值: 0,初始化成功; 其他,错误代码       ***/
uint8_t MPU6050_Init(void) { 
    uint8_t res = 0;
    MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X80);    //复位MPU6050
    while(!delayms_timer(100));
    MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X00);    //唤醒MPU6050 
    MPU_Set_Gyro_Fsr(3);                    //陀螺仪传感器,±2000dps
    MPU_Set_Accel_Fsr(0);                    //加速度传感器,±2g
    MPU_Set_Rate(200);                        //设置采样率200Hz
    MPU_Write_Byte(MPU_INT_EN_REG,0X00);    //关闭所有中断
    MPU_Write_Byte(MPU_USER_CTRL_REG,0X00);    //I2C主模式关闭
    MPU_Write_Byte(MPU_FIFO_EN_REG,0X00);    //关闭FIFO
    MPU_Write_Byte(MPU_INTBP_CFG_REG,0X80);    //INT引脚低电平有效
    res = MPU_Read_Byte(MPU_DEVICE_ID_REG);
    if(res == MPU6050[_streamnum2].mpu6050addr) {   //器件ID正确
        MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0X01);    //设置CLKSEL,PLL X轴为参考
        MPU_Write_Byte(MPU_PWR_MGMT2_REG, 0X00);    //加速度与陀螺仪都工作
        MPU_Set_Rate(100);                        //设置采样率为100Hz
    }else {
        return 1;
    }
    return 0;
}
/***    设置MPU6050陀螺仪传感器满量程范围    返回值: 0,设置成功; 其他,设置失败
    fsr: 0,±250dps; 1,±500dps; 2,±1000dps; 3,±2000dps    ***/
uint8_t MPU_Set_Gyro_Fsr(uint8_t fsr) {
    return MPU_Write_Byte(MPU_GYRO_CFG_REG, fsr<<3);    //设置陀螺仪满量程范围
}
/***    设置MPU6050加速度传感器满量程范围    返回值: 0,设置成功; 其他,设置失败
    fsr: 0,±2g; 1,±4g; 2,±8g; 3,±16g    ***/
uint8_t MPU_Set_Accel_Fsr(uint8_t fsr) {
    return MPU_Write_Byte(MPU_ACCEL_CFG_REG, fsr<<3);//设置加速度传感器满量程范围
}
/***    设置MPU6050的数字低通滤波器   返回值: 0,设置成功; 其他,设置失败
    lpf: 数字低通滤波频率(Hz)    ***/
uint8_t MPU_Set_LPF(uint16_t lpf) {
    uint8_t data=0;
    if(lpf >= 188) {
        data = 1;
    }else if(lpf >= 98) {
        data = 2;
    }else if(lpf >= 42) {
        data = 3;
    }else if(lpf >= 20) {
        data = 4;
    }else if(lpf >= 10) {
        data = 5;
    }else  {
        data = 6;
    }
    return MPU_Write_Byte(MPU_CFG_REG, data);        //设置数字低通滤波器
}
/***    设置MPU6050的采样率(假定Fs=1KHz)    返回值: 0,设置成功; 其他,设置失败
    rate: 4~1000(Hz)    ***/
uint8_t MPU_Set_Rate(uint16_t rate) {
    uint8_t data;
    if(rate>1000) {
        rate = 1000;
    }
    if(rate < 4) {
        rate = 4;
    }
    data = 1000/rate - 1;
    data = MPU_Write_Byte(MPU_SAMPLE_RATE_REG, data);    //设置数字低通滤波器
     return MPU_Set_LPF(rate/2);    //自动设置LPF为采样率的一半
}
/***    得到温度值            返回值: 温度值(扩大了100倍)    ***/
int16_t MPU_Get_Temperature(void) {
    uint8_t buf[2];
    int16_t raw;
    float temp;
    MPU_Read_Len(MPU6050[_streamnum2].mpu6050addr,MPU_TEMP_OUTH_REG, 2, buf);
    raw = ((uint16_t)buf[0]<<8) | buf[1];
    temp = 36.53 + ((double)raw)/340;
    return temp*100;;
}
/***    得到陀螺仪值(原始值) 返回值: 0,成功; 其他,错误代码
    gx,gy,gz: 陀螺仪x,y,z轴的原始读数(带符号)    ***/
uint8_t MPU_Get_Gyroscope(int16_t *gx, int16_t *gy, int16_t *gz) {
    uint8_t buf[6],res;
    res = MPU_Read_Len(MPU6050[_streamnum2].mpu6050addr, MPU_GYRO_XOUTH_REG, 6, buf);
    if(res == 0) {
        *gx = ((uint16_t)buf[0]<<8) | buf[1];
        *gy = ((uint16_t)buf[2]<<8) | buf[3];
        *gz = ((uint16_t)buf[4]<<8) | buf[5];
    }
    return res;;
}
/***    得到加速度值(原始值) 返回值: 0,成功; 其他,错误代码
    gx,gy,gz: 陀螺仪x,y,z轴的原始读数(带符号) ***/
uint8_t MPU_Get_Accelerometer(int16_t *ax,int16_t *ay,int16_t *az) {
    uint8_t buf[6],res;
    res = MPU_Read_Len(MPU6050[_streamnum2].mpu6050addr, MPU_ACCEL_XOUTH_REG, 6, buf);
    if(res == 0) {
        *ax = ((uint16_t)buf[0]<<8) | buf[1];
        *ay = ((uint16_t)buf[2]<<8) | buf[3];
        *az = ((uint16_t)buf[4]<<8) | buf[5];
    }
    return res;;
}


float pitch,roll,yaw;         //欧拉角
int16_t aacx,aacy,aacz;        //加速度传感器原始数据
int16_t gyrox,gyroy,gyroz;    //陀螺仪原始数据
int16_t temperature;                //温度
uint8_t flag_mpu6050 = 0;
void MPU6050_Read(void) {
    if(flag_mpu6050 == 1) {
        flag_mpu6050 = 0;
        MPUDMP_Getdata(&pitch, &roll, &yaw);        //角度
        MPU_Get_Gyroscope(&gyrox, &gyroy, &gyroz);  //陀螺仪
        MPU_Get_Accelerometer(&aacx, &aacy, &aacz); //加速度
    }
}
void MPU6050_Confi(void) {
    //在调用MPU6050配置函数时, 一次性初始化所有MPU6050的引脚
    MPU6050_ioDef();
    for(_streamnum2=0; _streamnum2<MPU6050_NUM; _streamnum2++) {
        MPU6050_ioSet();
        MPU6050_wireSet();
    }
    for(_streamnum2=0; _streamnum2<MPU6050_NUM; _streamnum2++) {
        MPU6050_GPIOInit();
    }
    MPU6050_EXTIInit();
    MPU6050_NVICInit();
    
    MPU6050_setOutputStream(0);
    while(MPU6050_Init());
    while(MPU6050_DMPInit());
}
