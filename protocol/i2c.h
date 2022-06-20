#ifndef __I2C_H
#define __I2C_H
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
typedef enum {
    SCL,
    SDA
} I2C_WireTypeDef;
typedef enum {
    I2CLOW,
    I2CMEDIUM,
    I2CHIGH
} I2C_SpeedTypeDef;

typedef struct {    //I2C模拟总线结构体
    void (*SCL_Set)(Direct_TypeDef);
    void (*SDA_Set)(Direct_TypeDef);
    void (*SCL_Out)(Potential_TypeDef);
    void (*SDA_Out)(Potential_TypeDef);
    Potential_TypeDef (*SDA_In)(void);
    void (*delayus)(uint16_t us);
    void (*delayms)(uint16_t ms);
} I2C_AnalogHandleTypeDef;
typedef struct {               //I2C总线设备结构体
    uint8_t addr;              //模块I2C地址
    int8_t wait;               //模块是否必须响应
    I2C_SpeedTypeDef speed;    //模块I2C速率
    void *bushandle;           //I2C模拟/硬件总线句柄
} I2C_ModuleHandleTypeDef;

////////////////////////////////////////////////////////////////////////////
//高/中/低速I2C通信, 理论速率高速3.4Mbit/s,中速400kbit/s,低速100kbit/s
#define HLongTime 0    //高速I2C
#define MLongTime 0    //中速I2C
#define LLongTime 5    //低速I2C
//#define LShortTime 1
static uint8_t shorttime = 0, longtime = 0;
static inline void I2C_Settime(I2C_ModuleHandleTypeDef *modular) {
    if(modular->speed == I2CHIGH) {
        longtime = HLongTime;
        shorttime = 0;
    } else if(modular->speed == I2CMEDIUM) {
        longtime = MLongTime;
        shorttime = 0;
    } else if(modular->speed == I2CLOW) {
        longtime = LLongTime;
        shorttime = 1;
    }
}
////////////////////////////////////////////////////////////////////////////
#define WAITING_EXCEPTION 1    //读取从机应答信号后, 是否根据异常信号停止读写操作
static inline int8_t I2C_Init(I2C_ModuleHandleTypeDef *modular) {
    //初始化总线
    ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);
    ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(IN);
    if(((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_In() != HIGH) {
        return 1;    //若总线没有被释放, 返回错误值
    }
    ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(OUT);
    ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(LOW);
    ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Set(OUT);
    return 0;
}
static inline void I2C_Start(I2C_ModuleHandleTypeDef *modular) {
    //在时钟线置高时, 数据线上的一个下降沿表示开始传输数据
    ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);    //拉高数据线, 进行准备
    ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(HIGH);    //拉高时钟线
    //((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(shorttime);
    ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(LOW);    //拉低数据线, 产生一个下降沿
    if(longtime != 0) {
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(longtime);
    }
    ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(LOW);    //拉低时钟线
}
static inline void I2C_Stop(I2C_ModuleHandleTypeDef *modular) {
    //在时钟线置高时, 数据线上的一个上升沿表示停止传输数据
    ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(LOW);     //拉低数据线, 进行准备
    ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(HIGH);    //拉高时钟线
    //((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(shorttime);
    ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);    //拉高数据线, 产生一个上升沿
    if(longtime != 0) {
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(longtime);
    }
}
static inline void I2C_WriteBit(I2C_ModuleHandleTypeDef *modular, uint8_t bit) {
    //写1位数据
    //I2C通信建立后, 在时钟线拉低期间, 数据线可进行操作, 主机写数据; 时钟线拉高期间, 数据不可进行操作, 从机读数据
    if(bit) {    //写数据至数据线
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);
    } else {
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(LOW);
    }
    //((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(shorttime);
    ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(HIGH);    //拉高时钟线并保持一段时间, 等待从机读取
    if(longtime != 0) {
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(longtime);
    }
    ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(LOW);    //拉低时钟线
    //((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);    //释放数据总线
    ////((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(shorttime);
}
static inline uint8_t I2C_ReadBit(I2C_ModuleHandleTypeDef *modular) {
    //读1位数据
    //I2C通信建立后, 在时钟线拉低期间, 数据线可进行操作, 从机写数据; 时钟线拉高期间, 数据不可进行操作, 主机读数据
    uint8_t bit = 0;
    //((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(IN);      //数据线设置为读取模式
    ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(HIGH);    //拉高时钟线并保持一段时间, 待数据稳定后读入
    if(longtime != 0) {
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(longtime);
    }
    if(((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_In() == HIGH) {    //从数据线读数据
        bit = 1;
    } else {
        bit = 0;
    }
    ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(LOW);    //拉低时钟线
    //((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(OUT);      //数据线设置为写入模式
    return bit;
}
static inline uint8_t I2C_WriteWait(I2C_ModuleHandleTypeDef *modular, uint32_t timeout) {
    //每写入1字节数据后等待从机应答
    //写入完毕, 主机释放(即拉高)时钟线和数据线.
    //一定时间内若能检测到从机拉低数据线, 说明从机应答正常, 可以继续进行通信; 否则说明从机存在问题, 需要重新建立通信
    uint8_t wait = 0;
    ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(IN);      //数据线设置为读取模式
    ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(HIGH);    //拉高时钟线并保持一段时间, 待数据稳定后读入
    ((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(timeout);
    if(((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_In() == HIGH) {    //从数据线读数据
        wait = 1;
    } else {
        wait = 0;
    }
    ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(LOW);    //拉低时钟线
    ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(OUT);    //数据线设置为写入模式
    return wait;
}
static inline void I2C_ReadRespond(I2C_ModuleHandleTypeDef *modular, uint32_t timeout, uint8_t respond) {
    //每读取1字节数据后主机进行应答
    //读取完毕, 从机会释放(即拉高)数据线.
    //主机写入1位低电平信号, 表示应答正常, 从机继续进行通信; 主机写入1位高电平信号, 表示不再接收数据, 从机停止通信
    if(respond) {    //写数据至数据线
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(LOW);
    } else {
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);
    }
    //((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(shorttime);
    ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(HIGH);    //拉高时钟线并保持一段时间, 等待从机读取
    ((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(timeout);
    ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(LOW);     //拉低时钟线
    ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);    //释放数据总线
    //((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(shorttime);
}
////////////////////////////////////////////////////////////////////////////
static inline int8_t I2C_WriteByte(I2C_ModuleHandleTypeDef *modular, uint8_t byte, uint32_t timeout) {
    //写1字节数据, 将数据按位拆分后写入
    for(uint8_t i = 0x80; i; i >>= 1) {
        I2C_WriteBit(modular, byte & i ? 1 : 0);
    }
    ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);    //释放数据总线
    //((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(shorttime);
    if(I2C_WriteWait(modular, timeout) == 1) {
#if WAITING_EXCEPTION == 1
        I2C_Stop(modular);
        return 1;
#endif
    }
    return 0;
}
static inline uint8_t I2C_ReadByte(I2C_ModuleHandleTypeDef *modular, uint32_t timeout) {
    //读1字节数据, 将数据按位读取后合并
    uint8_t byte = 0;
    ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(IN);    //数据线设置为读取模式
    for(uint8_t i = 0x80; i; i >>= 1) {
        byte |= I2C_ReadBit(modular) ? i : 0x00;
    }
    ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(OUT);    //数据线设置为写入模式
    I2C_ReadRespond(modular, timeout, 0);
    return byte;
}
static inline int8_t I2C_Write(I2C_ModuleHandleTypeDef *modular, uint8_t *pdata, uint16_t size, uint32_t timeout) {
    for(uint16_t j = 0; j < size; j++) {
        for(uint8_t i = 0x80; i; i >>= 1) {
            I2C_WriteBit(modular, *pdata & i ? 1 : 0);
        }
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);    //释放数据总线
        //((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(shorttime);
        if(I2C_WriteWait(modular, timeout) == 1) {
#if WAITING_EXCEPTION == 1
            I2C_Stop(modular);
            return 1;
#endif
        }
        pdata++;
    }
    return 0;
}
static inline void I2C_Read(I2C_ModuleHandleTypeDef *modular, uint8_t *pdata, uint16_t size, uint32_t timeout) {
    for(uint16_t j = 0; j < size; j++) {
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(IN);    //数据线设置为读取模式
        for(uint8_t i = 0x80; i; i >>= 1) {
            *pdata |= I2C_ReadBit(modular) ? i : 0x00;
        }
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(OUT);    //数据线设置为写入模式
        pdata++;
        if(j + 1 < size) {
            I2C_ReadRespond(modular, timeout, 1);
        } else {
            I2C_ReadRespond(modular, timeout, 0);
        }
    }
}


////////////////////////////////////////////////////////////////////////////
static inline int8_t MODULAR_I2C_WriteByte(I2C_ModuleHandleTypeDef *modular, uint8_t address, uint8_t byte, int8_t skip, uint32_t timeout) {
    I2C_Settime(modular);
    I2C_Init(modular);
    I2C_Start(modular);
    if(I2C_WriteByte(modular, (modular->addr << 1) | 0X00, timeout) == 1) {    //发送器件地址+写命令, 将数据按位拆分后写入
        return 1;
    }
    if(!skip) {
        if(I2C_WriteByte(modular, address, timeout) == 1) {
            return 1;
        }
    }
    if(I2C_WriteByte(modular, byte, timeout) == 1) {    //发送数据
        return 1;
    }
    I2C_Stop(modular);
    return 0;
}
static inline uint8_t MODULAR_I2C_ReadByte(I2C_ModuleHandleTypeDef *modular, uint8_t address, int8_t skip, uint32_t timeout) {
    uint8_t byte = 0;
    I2C_Settime(modular);
    I2C_Init(modular);
    if(!skip) {
        I2C_Start(modular);
        if(I2C_WriteByte(modular, (modular->addr << 1) | 0X00, timeout) == 1) {    //发送器件地址+写命令, 将数据按位拆分后写入
        }
        if(I2C_WriteByte(modular, address, timeout) == 1) {
        }
    }
    I2C_Start(modular);
    if(I2C_WriteByte(modular, (modular->addr << 1) | 0X01, timeout) == 1) {    //发送器件地址+读命令, 将数据按位拆分后写入
    }
    byte = I2C_ReadByte(modular, timeout);
    I2C_Stop(modular);
    return byte;
}
//多字节写入函数, timeout应答超时,speed速度模式
static inline int8_t MODULAR_I2C_Write(I2C_ModuleHandleTypeDef *modular, uint8_t address, uint8_t *pdata, uint16_t size, int8_t skip, uint32_t timeout) {
    I2C_Settime(modular);
    I2C_Init(modular);
    I2C_Start(modular);
    if(I2C_WriteByte(modular, (modular->addr << 1) | 0X00, timeout) == 1) {    //发送器件地址+写命令, 将数据按位拆分后写入
        return 1;
    }
    if(!skip) {
        if(I2C_WriteByte(modular, address, timeout) == 1) {
            return 1;
        }
    }
    if(I2C_Write(modular, pdata, size, timeout) == 1) {
        return 1;
    }
    I2C_Stop(modular);
    return 0;
}
//多字节读取函数, timeout应答超时,speed速度模式
static inline int8_t MODULAR_I2C_Read(I2C_ModuleHandleTypeDef *modular, uint8_t address, uint8_t *pdata, uint16_t size, int8_t skip, uint32_t timeout) {
    I2C_Settime(modular);
    if(!skip) {
        I2C_Init(modular);
        I2C_Start(modular);
        if(I2C_WriteByte(modular, (modular->addr << 1) | 0X00, timeout) == 1) {    //发送器件地址+写命令, 将数据按位拆分后写入
            return 1;
        }
        if(I2C_WriteByte(modular, address, timeout) == 1) {
            return 1;
        }
    }
    I2C_Start(modular);
    if(I2C_WriteByte(modular, (modular->addr << 1) | 0X01, timeout) == 1) {    //发送器件地址+读命令, 将数据按位拆分后写入
        return 1;
    }
    I2C_Read(modular, pdata, size, timeout);
    I2C_Stop(modular);
    return 0;
}
#endif


/**
static inline int8_t MODULAR_I2C_Write(I2C_ModuleHandleTypeDef *modular, uint8_t address, uint8_t *pdata, uint16_t size, int8_t skip, uint32_t timeout) {
    int8_t i = 0;
    uint16_t j = 0;
    if(modular->speed == I2CHIGH) {
        //初始化总线
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(LOW);
        //在时钟线置高时, 数据线上的一个下降沿表示开始传输数据
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(OUT);     //数据线设置为写模式
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);    //拉高数据线, 进行准备
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(HIGH);    //拉高时钟线
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(LOW);     //拉低数据线, 产生一个下降沿
        if(longtime!=0) {((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(longtime);}
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(LOW);    //拉低时钟线
        //发送器件地址+写命令, 将数据按位拆分后写入
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(OUT);    //数据线设置为写模式
        for(i = 0; i < 8; i++) {
            if(((modular->addr << 1) | 0X00) & (0x80 >> i)) {    //写数据至数据线
                ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);
            } else {
                ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(LOW);
            }
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(HIGH);    //拉高时钟线并保持一段时间, 等待从机读取
            if(longtime!=0) {((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(longtime);}
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(LOW);    //拉低时钟线, 准备下一次写数据
        }
        //每写入1字节数据后等待从机应答
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(IN);      //数据线设置为读取模式
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);    //释放数据总线, 交由从机写数据
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(HIGH);    //拉高时钟线并保持一段时间, 待数据稳定后读入
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(timeout);
        if(((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_In() == HIGH) {    //从数据线读数据
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(LOW);         //拉低时钟线
#if WAITING_EXCEPTION == 1
            //在时钟线置高时, 数据线上的一个上升沿表示停止传输数据
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(OUT);     //数据线设置为写模式
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(LOW);     //拉低数据线, 进行准备
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(HIGH);    //拉高时钟线
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);    //拉高数据线, 产生一个上升沿
            if(longtime!=0) {((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(longtime);}
            return 1;
#endif
        } else {
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(LOW);    //拉低时钟线
        }
        //写寄存器地址, 将数据按位拆分后写入
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(OUT);    //数据线设置为写模式
        for(i = 0; i < 8; i++) {
            if(address & (0x80 >> i)) {    //写数据至数据线
                ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);
            } else {
                ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(LOW);
            }
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(HIGH);    //拉高时钟线并保持一段时间, 等待从机读取
            if(longtime!=0) {((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(longtime);}
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(LOW);    //拉低时钟线, 准备下一次写数据
        }
        //每写入1字节数据后等待从机应答
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(IN);      //数据线设置为读取模式
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);    //释放数据总线, 交由从机写数据
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(HIGH);    //拉高时钟线并保持一段时间, 待数据稳定后读入
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(timeout);
        if(((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_In() == HIGH) {    //从数据线读数据
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(LOW);         //拉低时钟线
#if WAITING_EXCEPTION == 1
            //在时钟线置高时, 数据线上的一个上升沿表示停止传输数据
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(OUT);     //数据线设置为写模式
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(LOW);     //拉低数据线, 进行准备
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(HIGH);    //拉高时钟线
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);    //拉高数据线, 产生一个上升沿
            if(longtime!=0) {((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(longtime);}
            return 1;
#endif
        } else {
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(LOW);    //拉低时钟线
        }
        for(j = 0; j < size; j++) {
            //写1字节数据, 将数据按位拆分后写入
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(OUT);    //数据线设置为写模式
            for(i = 0; i < 8; i++) {
                if(*pdata & (0x80 >> i)) {    //写数据至数据线
                    ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);
                } else {
                    ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(LOW);
                }
                ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(HIGH);    //拉高时钟线并保持一段时间, 等待从机读取
                if(longtime!=0) {((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(longtime);}
                ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(LOW);    //拉低时钟线, 准备下一次写数据
            }
            //每写入1字节数据后等待从机应答
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(IN);      //数据线设置为读取模式
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);    //释放数据总线, 交由从机写数据
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(HIGH);    //拉高时钟线并保持一段时间, 待数据稳定后读入
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(timeout);
            if(((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_In() == HIGH) {    //从数据线读数据
                ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(LOW);         //拉低时钟线
#if WAITING_EXCEPTION == 1
                //在时钟线置高时, 数据线上的一个上升沿表示停止传输数据
                ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(OUT);     //数据线设置为写模式
                ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(LOW);     //拉低数据线, 进行准备
                ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(HIGH);    //拉高时钟线
                ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);    //拉高数据线, 产生一个上升沿
                if(longtime!=0) {((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(longtime);}
                return 1;
#endif
            } else {
                ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(LOW);    //拉低时钟线
            }
            pdata++;
        }
        //在时钟线置高时, 数据线上的一个上升沿表示停止传输数据
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(OUT);     //数据线设置为写模式
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(LOW);     //拉低数据线, 进行准备
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(HIGH);    //拉高时钟线
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);    //拉高数据线, 产生一个上升沿
        if(longtime!=0) {((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(longtime);}
    } else if(modular->speed == I2CMEDIUM) {
        //初始化总线
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(LOW);
        //在时钟线置高时, 数据线上的一个下降沿表示开始传输数据
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(OUT);     //数据线设置为写模式
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);    //拉高数据线, 进行准备
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(HIGH);    //拉高时钟线
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(LOW);     //拉低数据线, 产生一个下降沿
        if(longtime!=0) {((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(longtime);}
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(LOW);    //拉低时钟线
        //发送器件地址+写命令, 将数据按位拆分后写入
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(OUT);    //数据线设置为写模式
        for(i = 0; i < 8; i++) {
            if(((modular->addr << 1) | 0X00) & (0x80 >> i)) {    //写数据至数据线
                ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);
            } else {
                ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(LOW);
            }
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(HIGH);    //拉高时钟线并保持一段时间, 等待从机读取
            if(longtime!=0) {((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(longtime);}
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(LOW);    //拉低时钟线, 准备下一次写数据
        }
        //每写入1字节数据后等待从机应答
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(IN);      //数据线设置为读取模式
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);    //释放数据总线, 交由从机写数据
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(HIGH);    //拉高时钟线并保持一段时间, 待数据稳定后读入
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(timeout);
        if(((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_In() == HIGH) {    //从数据线读数据
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(LOW);         //拉低时钟线
#if WAITING_EXCEPTION == 1
            //在时钟线置高时, 数据线上的一个上升沿表示停止传输数据
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(OUT);     //数据线设置为写模式
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(LOW);     //拉低数据线, 进行准备
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(HIGH);    //拉高时钟线
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);    //拉高数据线, 产生一个上升沿
            if(longtime!=0) {((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(longtime);}
            return 1;
#endif
        } else {
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(LOW);    //拉低时钟线
        }
        //写寄存器地址, 将数据按位拆分后写入
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(OUT);    //数据线设置为写模式
        for(i = 0; i < 8; i++) {
            if(address & (0x80 >> i)) {    //写数据至数据线
                ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);
            } else {
                ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(LOW);
            }
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(HIGH);    //拉高时钟线并保持一段时间, 等待从机读取
            if(longtime!=0) {((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(longtime);}
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(LOW);    //拉低时钟线, 准备下一次写数据
        }
        //每写入1字节数据后等待从机应答
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(IN);      //数据线设置为读取模式
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);    //释放数据总线, 交由从机写数据
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(HIGH);    //拉高时钟线并保持一段时间, 待数据稳定后读入
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(timeout);
        if(((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_In() == HIGH) {    //从数据线读数据
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(LOW);         //拉低时钟线
#if WAITING_EXCEPTION == 1
            //在时钟线置高时, 数据线上的一个上升沿表示停止传输数据
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(OUT);     //数据线设置为写模式
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(LOW);     //拉低数据线, 进行准备
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(HIGH);    //拉高时钟线
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);    //拉高数据线, 产生一个上升沿
            if(longtime!=0) {((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(longtime);}
            return 1;
#endif
        } else {
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(LOW);    //拉低时钟线
        }
        for(j = 0; j < size; j++) {
            //写1字节数据, 将数据按位拆分后写入
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(OUT);    //数据线设置为写模式
            for(i = 0; i < 8; i++) {
                if(*pdata & (0x80 >> i)) {    //写数据至数据线
                    ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);
                } else {
                    ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(LOW);
                }
                ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(HIGH);    //拉高时钟线并保持一段时间, 等待从机读取
                if(longtime!=0) {((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(longtime);}
                ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(LOW);    //拉低时钟线, 准备下一次写数据
            }
            //每写入1字节数据后等待从机应答
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(IN);      //数据线设置为读取模式
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);    //释放数据总线, 交由从机写数据
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(HIGH);    //拉高时钟线并保持一段时间, 待数据稳定后读入
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(timeout);
            if(((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_In() == HIGH) {    //从数据线读数据
                ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(LOW);         //拉低时钟线
#if WAITING_EXCEPTION == 1
                //在时钟线置高时, 数据线上的一个上升沿表示停止传输数据
                ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(OUT);     //数据线设置为写模式
                ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(LOW);     //拉低数据线, 进行准备
                ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(HIGH);    //拉高时钟线
                ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);    //拉高数据线, 产生一个上升沿
                if(longtime!=0) {((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(longtime);}
                return 1;
#endif
            } else {
                ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(LOW);    //拉低时钟线
            }
            pdata++;
        }
        //在时钟线置高时, 数据线上的一个上升沿表示停止传输数据
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(OUT);     //数据线设置为写模式
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(LOW);     //拉低数据线, 进行准备
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(HIGH);    //拉高时钟线
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);    //拉高数据线, 产生一个上升沿
        if(longtime!=0) {((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(longtime);}
    } else if(modular->speed == I2CLOW) {
        //初始化总线
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(LOW);
        //在时钟线置高时, 数据线上的一个下降沿表示开始传输数据
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(OUT);     //数据线设置为写模式
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);    //拉高数据线, 进行准备
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(HIGH);    //拉高时钟线
        //((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(shorttime);
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(LOW);    //拉低数据线, 产生一个下降沿
        if(longtime!=0) {((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(longtime);}
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(LOW);    //拉低时钟线
        //发送器件地址+写命令, 将数据按位拆分后写入
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(OUT);    //数据线设置为写模式
        for(i = 0; i < 8; i++) {
            if(((modular->addr << 1) | 0X00) & (0x80 >> i)) {    //写数据至数据线
                ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);
            } else {
                ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(LOW);
            }
            //((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(shorttime);
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(HIGH);    //拉高时钟线并保持一段时间, 等待从机读取
            if(longtime!=0) {((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(longtime);}
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(LOW);    //拉低时钟线, 准备下一次写数据
        }
        //每写入1字节数据后等待从机应答
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(IN);      //数据线设置为读取模式
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);    //释放数据总线, 交由从机写数据
        //((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(shorttime);
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(HIGH);    //拉高时钟线并保持一段时间, 待数据稳定后读入
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(timeout);
        if(((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_In() == HIGH) {    //从数据线读数据
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(LOW);         //拉低时钟线
#if WAITING_EXCEPTION == 1
            //在时钟线置高时, 数据线上的一个上升沿表示停止传输数据
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(OUT);     //数据线设置为写模式
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(LOW);     //拉低数据线, 进行准备
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(HIGH);    //拉高时钟线
            //((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(shorttime);
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);    //拉高数据线, 产生一个上升沿
            if(longtime!=0) {((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(longtime);}
            return 1;
#endif
        } else {
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(LOW);    //拉低时钟线
        }
        //写寄存器地址, 将数据按位拆分后写入
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(OUT);    //数据线设置为写模式
        for(i = 0; i < 8; i++) {
            if(address & (0x80 >> i)) {    //写数据至数据线
                ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);
            } else {
                ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(LOW);
            }
            //((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(shorttime);
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(HIGH);    //拉高时钟线并保持一段时间, 等待从机读取
            if(longtime!=0) {((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(longtime);}
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(LOW);    //拉低时钟线, 准备下一次写数据
        }
        //每写入1字节数据后等待从机应答
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(IN);      //数据线设置为读取模式
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);    //释放数据总线, 交由从机写数据
        //((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(shorttime);
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(HIGH);    //拉高时钟线并保持一段时间, 待数据稳定后读入
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(timeout);
        if(((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_In() == HIGH) {    //从数据线读数据
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(LOW);         //拉低时钟线
#if WAITING_EXCEPTION == 1
            //在时钟线置高时, 数据线上的一个上升沿表示停止传输数据
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(OUT);     //数据线设置为写模式
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(LOW);     //拉低数据线, 进行准备
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(HIGH);    //拉高时钟线
            //((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(shorttime);
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);    //拉高数据线, 产生一个上升沿
            if(longtime!=0) {((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(longtime);}
            return 1;
#endif
        } else {
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(LOW);    //拉低时钟线
        }
        for(j = 0; j < size; j++) {
            //写1字节数据, 将数据按位拆分后写入
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(OUT);    //数据线设置为写模式
            for(i = 0; i < 8; i++) {
                if(*pdata & (0x80 >> i)) {    //写数据至数据线
                    ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);
                } else {
                    ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(LOW);
                }
                //((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(shorttime);
                ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(HIGH);    //拉高时钟线并保持一段时间, 等待从机读取
                if(longtime!=0) {((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(longtime);}
                ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(LOW);    //拉低时钟线, 准备下一次写数据
            }
            //每写入1字节数据后等待从机应答
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(IN);      //数据线设置为读取模式
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);    //释放数据总线, 交由从机写数据
            //((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(shorttime);
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(HIGH);    //拉高时钟线并保持一段时间, 待数据稳定后读入
            ((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(timeout);
            if(((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_In() == HIGH) {    //从数据线读数据
                ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(LOW);         //拉低时钟线
#if WAITING_EXCEPTION == 1
                //在时钟线置高时, 数据线上的一个上升沿表示停止传输数据
                ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(OUT);     //数据线设置为写模式
                ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(LOW);     //拉低数据线, 进行准备
                ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(HIGH);    //拉高时钟线
                //((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(shorttime);
                ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);    //拉高数据线, 产生一个上升沿
                if(longtime!=0) {((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(longtime);}
                return 1;
#endif
            } else {
                ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(LOW);    //拉低时钟线
            }
            pdata++;
        }
        //在时钟线置高时, 数据线上的一个上升沿表示停止传输数据
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Set(OUT);     //数据线设置为写模式
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(LOW);     //拉低数据线, 进行准备
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SCL_Out(HIGH);    //拉高时钟线
        //((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(shorttime);
        ((I2C_AnalogHandleTypeDef *)modular->bushandle)->SDA_Out(HIGH);    //拉高数据线, 产生一个上升沿
        if(longtime!=0) {((I2C_AnalogHandleTypeDef *)modular->bushandle)->delayus(longtime);}
    }
    return 0;
}**/
