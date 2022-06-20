#include "i2c.h"
#define HDELAY_LongTime() ;
#define HDELAY_ShortTime() ;
#define MDELAY_LongTime() modular.delayus(2)
#define MDELAY_ShortTime() ;
#define LDELAY_LongTime() modular.delayus(5)
#define LDELAY_ShortTime() modular.delayus(1)

#define WAITING_EXCEPTION 1   //读取从机应答信号后, 是否根据异常信号停止读写操作
////////////////////////////////////////////////////////////////////////////
//高/中/低速I2C通信, 理论速率高速3.4Mbit/s,中速400kbit/s,低速100kbit/s
//高速I2C
void I2CHWIRE_Init(I2C_AnalogTypedef modular) {
    //初始化总线
    modular.i2cSDAOut(HIGH);
    modular.i2cSCLOut(LOW);
}
void I2CHWIRE_Start(I2C_AnalogTypedef modular) {
    //在时钟线置高时, 数据线上的一个下降沿表示开始传输数据
    modular.i2cSDASet(OUT);             //数据线设置为写模式
    modular.i2cSDAOut(HIGH);            //拉高数据线, 进行准备
    modular.i2cSCLOut(HIGH);            //拉高时钟线
    HDELAY_ShortTime();
    modular.i2cSDAOut(LOW);             //拉低数据线, 产生一个下降沿
    HDELAY_LongTime();
    modular.i2cSCLOut(LOW);             //拉低时钟线
}
void I2CHWIRE_Stop(I2C_AnalogTypedef modular) {
    //在时钟线置高时, 数据线上的一个上升沿表示停止传输数据
    modular.i2cSDASet(OUT);             //数据线设置为写模式
    modular.i2cSDAOut(LOW);             //拉低数据线, 进行准备
    modular.i2cSCLOut(HIGH);            //拉高时钟线
    HDELAY_ShortTime();
    modular.i2cSDAOut(HIGH);            //拉高数据线, 产生一个上升沿
    HDELAY_LongTime();
}
void I2CHWIRE_WriteBit(I2C_AnalogTypedef modular, uint8_t bit) {
    //写1位数据, 根据i2c连续按字节读写的特性, 函数一般不会单独使用
    //I2C通信建立后, 在时钟线拉低期间, 数据线可进行操作, 主机写数据; 时钟线拉高期间, 数据不可进行操作, 从机读数据
    if(bit) {                           //写数据至数据线
        modular.i2cSDAOut(HIGH);
    }else {
        modular.i2cSDAOut(LOW);
    }
    HDELAY_ShortTime();
    modular.i2cSCLOut(HIGH);            //拉高时钟线并保持一段时间, 等待从机读取
    HDELAY_LongTime();
    modular.i2cSCLOut(LOW);             //拉低时钟线
}
uint8_t I2CHWIRE_ReadBit(I2C_AnalogTypedef modular) {
    //读1位数据, 根据i2c连续按字节读写的特性, 函数一般不会单独使用
    //I2C通信建立后, 在时钟线拉低期间, 数据线可进行操作, 从机写数据; 时钟线拉高期间, 数据不可进行操作, 主机读数据
    uint8_t bit = 0;
    modular.i2cSDAOut(HIGH);            //释放数据总线, 交由从机写数据
    HDELAY_ShortTime();
    modular.i2cSCLOut(HIGH);            //拉高时钟线并保持一段时间, 待数据稳定后读入
    HDELAY_LongTime();
    if(modular.i2cSDAIn() == HIGH) {    //从数据线读数据
        bit = 1;
    }else {
        bit = 0;
    }
    modular.i2cSCLOut(LOW);             //拉低时钟线
    return bit;
}
void I2CHWIRE_WriteByte(I2C_AnalogTypedef modular, uint8_t byte) {
    //写1字节数据, 将数据按位拆分后写入
    int8_t i = 0;
    modular.i2cSDASet(OUT);             //数据线设置为写模式
    for(i=0; i<8; i++) {
        if(byte&(0x80>>i)) {            //写数据至数据线
            modular.i2cSDAOut(HIGH);
        }else {
            modular.i2cSDAOut(LOW);
        }
        HDELAY_ShortTime();
        modular.i2cSCLOut(HIGH);        //拉高时钟线并保持一段时间, 等待从机读取
        HDELAY_LongTime();
        modular.i2cSCLOut(LOW);         //拉低时钟线, 准备下一次写数据
    }
}
uint8_t I2CHWIRE_ReadByte(I2C_AnalogTypedef modular) {
    //读1字节数据, 将数据按位读取后合并
    uint8_t byte = 0;
    int8_t i = 0;
    modular.i2cSDASet(IN);              //数据线设置为读取模式
    for(i=0; i<8; i++) {
        modular.i2cSDAOut(HIGH);        //释放数据总线, 交由从机写数据
        HDELAY_ShortTime();
        modular.i2cSCLOut(HIGH);        //拉高时钟线并保持一段时间, 待数据稳定后读入
        HDELAY_LongTime();
        if(modular.i2cSDAIn() == HIGH) {    //从数据线读数据
            byte = (byte<<1)|1;
        }else {
            byte = (byte<<1)|0;
        }
        modular.i2cSCLOut(LOW);         //拉低时钟线, 准备下一次读数据
    }
    return byte;
}
uint8_t I2CHWIRE_WriteWait(I2C_AnalogTypedef modular, uint32_t timeout) {
    //每写入1字节数据后等待从机应答
    //写入完毕, 主机释放(即拉高)时钟线和数据线. 
    //一定时间内若能检测到从机拉低数据线, 说明从机应答正常, 可以继续进行通信; 否则说明从机存在问题, 需要重新建立通信
    uint8_t wait = 0;
    modular.i2cSDASet(IN);              //数据线设置为读取模式
    modular.i2cSDAOut(HIGH);            //释放数据总线, 交由从机写数据
    HDELAY_ShortTime();
    modular.i2cSCLOut(HIGH);            //拉高时钟线并保持一段时间, 待数据稳定后读入
    modular.delayus(timeout);
    if(modular.i2cSDAIn() == HIGH) {    //从数据线读数据
        wait = 1;
    }else {
        wait = 0;
    }
    modular.i2cSCLOut(LOW);             //拉低时钟线
    return wait;
}
void I2CHWIRE_ReadRespond(I2C_AnalogTypedef modular, uint8_t respond) {
    //每读取1字节数据后主机进行应答
    //读取完毕, 从机会释放(即拉高)数据线.
    //主机写入1位低电平信号, 表示应答正常, 从机继续进行通信; 主机写入1位高电平信号, 表示不再接收数据, 从机停止通信
    modular.i2cSDASet(OUT);             //数据线设置为写模式
    if(!respond) {                      //写数据至数据线
        modular.i2cSDAOut(HIGH);
    }else {
        modular.i2cSDAOut(LOW);
    }
    HDELAY_ShortTime();
    modular.i2cSCLOut(HIGH);            //拉高时钟线并保持一段时间, 等待从机读取
    HDELAY_LongTime();
    modular.i2cSCLOut(LOW);             //拉低时钟线
}
////////////////////////////////////////////////////////////////////////////
//中速I2C
void I2CMWIRE_Init(I2C_AnalogTypedef modular) {
    //初始化总线
    modular.i2cSDAOut(HIGH);
    modular.i2cSCLOut(LOW);
}
void I2CMWIRE_Start(I2C_AnalogTypedef modular) {
    //在时钟线置高时, 数据线上的一个下降沿表示开始传输数据
    modular.i2cSDASet(OUT);             //数据线设置为写模式
    modular.i2cSDAOut(HIGH);            //拉高数据线, 进行准备
    modular.i2cSCLOut(HIGH);            //拉高时钟线
    MDELAY_ShortTime();
    modular.i2cSDAOut(LOW);             //拉低数据线, 产生一个下降沿
    MDELAY_LongTime();
    modular.i2cSCLOut(LOW);             //拉低时钟线
}
void I2CMWIRE_Stop(I2C_AnalogTypedef modular) {
    //在时钟线置高时, 数据线上的一个上升沿表示停止传输数据
    modular.i2cSDASet(OUT);             //数据线设置为写模式
    modular.i2cSDAOut(LOW);             //拉低数据线, 进行准备
    modular.i2cSCLOut(HIGH);            //拉高时钟线
    MDELAY_ShortTime();
    modular.i2cSDAOut(HIGH);            //拉高数据线, 产生一个上升沿
    MDELAY_LongTime();
}
void I2CMWIRE_WriteBit(I2C_AnalogTypedef modular, uint8_t bit) {
    //写1位数据, 根据i2c连续按字节读写的特性, 函数一般不会单独使用
    //I2C通信建立后, 在时钟线拉低期间, 数据线可进行操作, 主机写数据; 时钟线拉高期间, 数据不可进行操作, 从机读数据
    if(bit) {                           //写数据至数据线
        modular.i2cSDAOut(HIGH);
    }else {
        modular.i2cSDAOut(LOW);
    }
    MDELAY_ShortTime();
    modular.i2cSCLOut(HIGH);            //拉高时钟线并保持一段时间, 等待从机读取
    MDELAY_LongTime();
    modular.i2cSCLOut(LOW);             //拉低时钟线
}
uint8_t I2CMWIRE_ReadBit(I2C_AnalogTypedef modular) {
    //读1位数据, 根据i2c连续按字节读写的特性, 函数一般不会单独使用
    //I2C通信建立后, 在时钟线拉低期间, 数据线可进行操作, 从机写数据; 时钟线拉高期间, 数据不可进行操作, 主机读数据
    uint8_t bit = 0;
    modular.i2cSDAOut(HIGH);            //释放数据总线, 交由从机写数据
    MDELAY_ShortTime();
    modular.i2cSCLOut(HIGH);            //拉高时钟线并保持一段时间, 待数据稳定后读入
    MDELAY_LongTime();
    if(modular.i2cSDAIn() == HIGH) {    //从数据线读数据
        bit = 1;
    }else {
        bit = 0;
    }
    modular.i2cSCLOut(LOW);             //拉低时钟线
    return bit;
}
void I2CMWIRE_WriteByte(I2C_AnalogTypedef modular, uint8_t byte) {
    //写1字节数据, 将数据按位拆分后写入
    int8_t i = 0;
    modular.i2cSDASet(OUT);             //数据线设置为写模式
    for(i=0; i<8; i++) {
        if(byte&(0x80>>i)) {            //写数据至数据线
            modular.i2cSDAOut(HIGH);
        }else {
            modular.i2cSDAOut(LOW);
        }
        MDELAY_ShortTime();
        modular.i2cSCLOut(HIGH);        //拉高时钟线并保持一段时间, 等待从机读取
        MDELAY_LongTime();
        modular.i2cSCLOut(LOW);         //拉低时钟线, 准备下一次写数据
    }
}
uint8_t I2CMWIRE_ReadByte(I2C_AnalogTypedef modular) {
    //读1字节数据, 将数据按位读取后合并
    uint8_t byte = 0;
    int8_t i = 0;
    modular.i2cSDASet(IN);              //数据线设置为读取模式
    for(i=0; i<8; i++) {
        modular.i2cSDAOut(HIGH);        //释放数据总线, 交由从机写数据
        MDELAY_ShortTime();
        modular.i2cSCLOut(HIGH);        //拉高时钟线并保持一段时间, 待数据稳定后读入
        MDELAY_LongTime();
        if(modular.i2cSDAIn() == HIGH) {    //从数据线读数据
            byte = (byte<<1)|1;
        }else {
            byte = (byte<<1)|0;
        }
        modular.i2cSCLOut(LOW);         //拉低时钟线, 准备下一次读数据
    }
    return byte;
}
uint8_t I2CMWIRE_WriteWait(I2C_AnalogTypedef modular, uint32_t timeout) {
    //每写入1字节数据后等待从机应答
    //写入完毕, 主机释放(即拉高)时钟线和数据线. 
    //一定时间内若能检测到从机拉低数据线, 说明从机应答正常, 可以继续进行通信; 否则说明从机存在问题, 需要重新建立通信
    uint8_t wait = 0;
    modular.i2cSDASet(IN);              //数据线设置为读取模式
    modular.i2cSDAOut(HIGH);            //释放数据总线, 交由从机写数据
    MDELAY_ShortTime();
    modular.i2cSCLOut(HIGH);            //拉高时钟线并保持一段时间, 待数据稳定后读入
    modular.delayus(timeout);
    if(modular.i2cSDAIn() == HIGH) {    //从数据线读数据
        wait = 1;
    }else {
        wait = 0;
    }
    modular.i2cSCLOut(LOW);             //拉低时钟线
    return wait;
}
void I2CMWIRE_ReadRespond(I2C_AnalogTypedef modular, uint8_t respond) {
    //每读取1字节数据后主机进行应答
    //读取完毕, 从机会释放(即拉高)数据线.
    //主机写入1位低电平信号, 表示应答正常, 从机继续进行通信; 主机写入1位高电平信号, 表示不再接收数据, 从机停止通信
    modular.i2cSDASet(OUT);             //数据线设置为写模式
    if(!respond) {                      //写数据至数据线
        modular.i2cSDAOut(HIGH);
    }else {
        modular.i2cSDAOut(LOW);
    }
    MDELAY_ShortTime();
    modular.i2cSCLOut(HIGH);            //拉高时钟线并保持一段时间, 等待从机读取
    MDELAY_LongTime();
    modular.i2cSCLOut(LOW);             //拉低时钟线
}
////////////////////////////////////////////////////////////////////////////
//低速I2C
void I2CLWIRE_Init(I2C_AnalogTypedef modular) {
    //初始化总线
    modular.i2cSDAOut(HIGH);
    modular.i2cSCLOut(LOW);
}
void I2CLWIRE_Start(I2C_AnalogTypedef modular) {
    //在时钟线置高时, 数据线上的一个下降沿表示开始传输数据
    modular.i2cSDASet(OUT);             //数据线设置为写模式
    modular.i2cSDAOut(HIGH);            //拉高数据线, 进行准备
    modular.i2cSCLOut(HIGH);            //拉高时钟线
    LDELAY_ShortTime();
    modular.i2cSDAOut(LOW);             //拉低数据线, 产生一个下降沿
    LDELAY_LongTime();
    modular.i2cSCLOut(LOW);             //拉低时钟线
}
void I2CLWIRE_Stop(I2C_AnalogTypedef modular) {
    //在时钟线置高时, 数据线上的一个上升沿表示停止传输数据
    modular.i2cSDASet(OUT);             //数据线设置为写模式
    modular.i2cSDAOut(LOW);             //拉低数据线, 进行准备
    modular.i2cSCLOut(HIGH);            //拉高时钟线
    LDELAY_ShortTime();
    modular.i2cSDAOut(HIGH);            //拉高数据线, 产生一个上升沿
    LDELAY_LongTime();
}
void I2CLWIRE_WriteBit(I2C_AnalogTypedef modular, uint8_t bit) {
    //写1位数据, 根据i2c连续按字节读写的特性, 函数一般不会单独使用
    //I2C通信建立后, 在时钟线拉低期间, 数据线可进行操作, 主机写数据; 时钟线拉高期间, 数据不可进行操作, 从机读数据
    if(bit) {                           //写数据至数据线
        modular.i2cSDAOut(HIGH);
    }else {
        modular.i2cSDAOut(LOW);
    }
    LDELAY_ShortTime();
    modular.i2cSCLOut(HIGH);            //拉高时钟线并保持一段时间, 等待从机读取
    LDELAY_LongTime();
    modular.i2cSCLOut(LOW);             //拉低时钟线
}
uint8_t I2CLWIRE_ReadBit(I2C_AnalogTypedef modular) {
    //读1位数据, 根据i2c连续按字节读写的特性, 函数一般不会单独使用
    //I2C通信建立后, 在时钟线拉低期间, 数据线可进行操作, 从机写数据; 时钟线拉高期间, 数据不可进行操作, 主机读数据
    uint8_t bit = 0;
    modular.i2cSDAOut(HIGH);            //释放数据总线, 交由从机写数据
    LDELAY_ShortTime();
    modular.i2cSCLOut(HIGH);            //拉高时钟线并保持一段时间, 待数据稳定后读入
    LDELAY_LongTime();
    if(modular.i2cSDAIn() == HIGH) {    //从数据线读数据
        bit = 1;
    }else {
        bit = 0;
    }
    modular.i2cSCLOut(LOW);             //拉低时钟线
    return bit;
}
void I2CLWIRE_WriteByte(I2C_AnalogTypedef modular, uint8_t byte) {
    //写1字节数据, 将数据按位拆分后写入
    int8_t i = 0;
    modular.i2cSDASet(OUT);             //数据线设置为写模式
    for(i=0; i<8; i++) {
        if(byte&(0x80>>i)) {            //写数据至数据线
            modular.i2cSDAOut(HIGH);
        }else {
            modular.i2cSDAOut(LOW);
        }
        LDELAY_ShortTime();
        modular.i2cSCLOut(HIGH);        //拉高时钟线并保持一段时间, 等待从机读取
        LDELAY_LongTime();
        modular.i2cSCLOut(LOW);         //拉低时钟线, 准备下一次写数据
    }
}
uint8_t I2CLWIRE_ReadByte(I2C_AnalogTypedef modular) {
    //读1字节数据, 将数据按位读取后合并
    uint8_t byte = 0;
    int8_t i = 0;
    modular.i2cSDASet(IN);              //数据线设置为读取模式
    for(i=0; i<8; i++) {
        modular.i2cSDAOut(HIGH);        //释放数据总线, 交由从机写数据
        LDELAY_ShortTime();
        modular.i2cSCLOut(HIGH);        //拉高时钟线并保持一段时间, 待数据稳定后读入
        LDELAY_LongTime();
        if(modular.i2cSDAIn() == HIGH) {    //从数据线读数据
            byte = (byte<<1)|1;
        }else {
            byte = (byte<<1)|0;
        }
        modular.i2cSCLOut(LOW);         //拉低时钟线, 准备下一次读数据
    }
    return byte;
}
uint8_t I2CLWIRE_WriteWait(I2C_AnalogTypedef modular, uint32_t timeout) {
    //每写入1字节数据后等待从机应答
    //写入完毕, 主机释放(即拉高)时钟线和数据线. 
    //一定时间内若能检测到从机拉低数据线, 说明从机应答正常, 可以继续进行通信; 否则说明从机存在问题, 需要重新建立通信
    uint8_t wait = 0;
    modular.i2cSDASet(IN);              //数据线设置为读取模式
    modular.i2cSDAOut(HIGH);            //释放数据总线, 交由从机写数据
    LDELAY_ShortTime();
    modular.i2cSCLOut(HIGH);            //拉高时钟线并保持一段时间, 待数据稳定后读入
    modular.delayus(timeout);
    if(modular.i2cSDAIn() == HIGH) {    //从数据线读数据
        wait = 1;
    }else {
        wait = 0;
    }
    modular.i2cSCLOut(LOW);             //拉低时钟线
    return wait;
}
void I2CLWIRE_ReadRespond(I2C_AnalogTypedef modular, uint8_t respond) {
    //每读取1字节数据后主机进行应答
    //读取完毕, 从机会释放(即拉高)数据线.
    //主机写入1位低电平信号, 表示应答正常, 从机继续进行通信; 主机写入1位高电平信号, 表示不再接收数据, 从机停止通信
    modular.i2cSDASet(OUT);             //数据线设置为写模式
    if(!respond) {                      //写数据至数据线
        modular.i2cSDAOut(HIGH);
    }else {
        modular.i2cSDAOut(LOW);
    }
    LDELAY_ShortTime();
    modular.i2cSCLOut(HIGH);            //拉高时钟线并保持一段时间, 等待从机读取
    LDELAY_LongTime();
    modular.i2cSCLOut(LOW);             //拉低时钟线
}


////////////////////////////////////////////////////////////////////////////
//单字节写入函数, timeout应答超时,speed速度模式
//不适用于OLED等需要写入内部寄存器的I2C器件
int8_t MODULAR_I2CWriteByte(I2C_AnalogTypedef modular, uint8_t data, uint32_t timeout, int8_t speed) {
    if(speed == I2CHIGH) {
        I2CHWIRE_Init(modular);
        I2CHWIRE_Start(modular);
        I2CHWIRE_WriteByte(modular, (modular.ADDRESS<<1)|0X00);     //发送器件地址+写命令
        if(I2CHWIRE_WriteWait(modular, timeout) == 1) {             //等待应答
            I2CHWIRE_Stop(modular);
            return -1;
        }
        I2CHWIRE_WriteByte(modular, data);                          //发送数据
        if(I2CHWIRE_WriteWait(modular, timeout) == 1) {             //等待应答
            I2CHWIRE_Stop(modular);
            return -1;         
        }
        I2CHWIRE_Stop(modular);
    }
    else if(speed == I2CMEDIUM) {
        I2CMWIRE_Init(modular);
        I2CMWIRE_Start(modular);
        I2CMWIRE_WriteByte(modular, (modular.ADDRESS<<1)|0X00);     //发送器件地址+写命令
        if(I2CMWIRE_WriteWait(modular, timeout) == 1) {             //等待应答
            I2CMWIRE_Stop(modular);
            return -1;
        }
        I2CMWIRE_WriteByte(modular, data);                          //发送数据
        if(I2CMWIRE_WriteWait(modular, timeout) == 1) {             //等待应答
            I2CMWIRE_Stop(modular);
            return -1;         
        }
        I2CMWIRE_Stop(modular);    
    }else {
        I2CLWIRE_Init(modular);
        I2CLWIRE_Start(modular);
        I2CLWIRE_WriteByte(modular, (modular.ADDRESS<<1)|0X00);     //发送器件地址+写命令
        if(I2CLWIRE_WriteWait(modular, timeout) == 1) {             //等待应答
            I2CLWIRE_Stop(modular);
            return -1;
        }
        I2CLWIRE_WriteByte(modular, data);                          //发送数据
        if(I2CLWIRE_WriteWait(modular, timeout) == 1) {             //等待应答
            I2CLWIRE_Stop(modular);
            return -1;         
        }
        I2CLWIRE_Stop(modular);    
    }
    return 1;
}
//多字节写入函数, timeout应答超时,speed速度模式
//为了使速度接近理论值, 尽可能地减少调用函数入栈出栈等步骤, 并且对数据进行连续写入处理
int8_t MODULAR_I2CWrite(I2C_AnalogTypedef modular, uint8_t address, uint8_t *pdata, uint16_t size, uint32_t timeout, int8_t speed) {
    int8_t i = 0;
    uint16_t j = 0;
    if(speed == I2CHIGH) {
        //初始化总线
        modular.i2cSDAOut(HIGH);
        modular.i2cSCLOut(LOW);
        //在时钟线置高时, 数据线上的一个下降沿表示开始传输数据
        modular.i2cSDASet(OUT);             //数据线设置为写模式
        modular.i2cSDAOut(HIGH);            //拉高数据线, 进行准备
        modular.i2cSCLOut(HIGH);            //拉高时钟线
        HDELAY_ShortTime();
        modular.i2cSDAOut(LOW);             //拉低数据线, 产生一个下降沿
        HDELAY_LongTime();
        modular.i2cSCLOut(LOW);             //拉低时钟线
        //发送器件地址+写命令, 将数据按位拆分后写入
        modular.i2cSDASet(OUT);             //数据线设置为写模式
        for(i=0; i<8; i++) {
            if(((modular.ADDRESS<<1)|0X00)&(0x80>>i)) { //写数据至数据线
                modular.i2cSDAOut(HIGH);
            }else {
                modular.i2cSDAOut(LOW);
            }
            HDELAY_ShortTime();
            modular.i2cSCLOut(HIGH);        //拉高时钟线并保持一段时间, 等待从机读取
            HDELAY_LongTime();
            modular.i2cSCLOut(LOW);         //拉低时钟线, 准备下一次写数据
        }  
        //每写入1字节数据后等待从机应答
        modular.i2cSDASet(IN);              //数据线设置为读取模式
        modular.i2cSDAOut(HIGH);            //释放数据总线, 交由从机写数据
        HDELAY_ShortTime();
        modular.i2cSCLOut(HIGH);            //拉高时钟线并保持一段时间, 待数据稳定后读入
        modular.delayus(timeout);
        if(modular.i2cSDAIn() == HIGH) {    //从数据线读数据
            modular.i2cSCLOut(LOW);         //拉低时钟线
            #if WAITING_EXCEPTION == 1
            //在时钟线置高时, 数据线上的一个上升沿表示停止传输数据
            modular.i2cSDASet(OUT);         //数据线设置为写模式
            modular.i2cSDAOut(LOW);         //拉低数据线, 进行准备
            modular.i2cSCLOut(HIGH);        //拉高时钟线
            HDELAY_ShortTime();
            modular.i2cSDAOut(HIGH);        //拉高数据线, 产生一个上升沿
            HDELAY_LongTime();
            return -1;
            #endif
        }else {
            modular.i2cSCLOut(LOW);         //拉低时钟线 
        }
        //写寄存器地址, 将数据按位拆分后写入
        modular.i2cSDASet(OUT);             //数据线设置为写模式
        for(i=0; i<8; i++) {
            if(address&(0x80>>i)) {         //写数据至数据线
                modular.i2cSDAOut(HIGH);
            }else {
                modular.i2cSDAOut(LOW);
            }
            HDELAY_ShortTime();
            modular.i2cSCLOut(HIGH);        //拉高时钟线并保持一段时间, 等待从机读取
            HDELAY_LongTime();
            modular.i2cSCLOut(LOW);         //拉低时钟线, 准备下一次写数据
        }
        //每写入1字节数据后等待从机应答
        modular.i2cSDASet(IN);              //数据线设置为读取模式
        modular.i2cSDAOut(HIGH);            //释放数据总线, 交由从机写数据
        HDELAY_ShortTime();
        modular.i2cSCLOut(HIGH);            //拉高时钟线并保持一段时间, 待数据稳定后读入
        modular.delayus(timeout);
        if(modular.i2cSDAIn() == HIGH) {    //从数据线读数据
            modular.i2cSCLOut(LOW);         //拉低时钟线
            #if WAITING_EXCEPTION == 1
            //在时钟线置高时, 数据线上的一个上升沿表示停止传输数据
            modular.i2cSDASet(OUT);         //数据线设置为写模式
            modular.i2cSDAOut(LOW);         //拉低数据线, 进行准备
            modular.i2cSCLOut(HIGH);        //拉高时钟线
            HDELAY_ShortTime();
            modular.i2cSDAOut(HIGH);        //拉高数据线, 产生一个上升沿
            HDELAY_LongTime();
            return -1;
            #endif
        }else {
            modular.i2cSCLOut(LOW);         //拉低时钟线 
        }
        for(j=0; j<size; j++) {   
            //写1字节数据, 将数据按位拆分后写入
            modular.i2cSDASet(OUT);         //数据线设置为写模式
            for(i=0; i<8; i++) {
                if(*pdata&(0x80>>i)) {      //写数据至数据线
                    modular.i2cSDAOut(HIGH);
                }else {
                    modular.i2cSDAOut(LOW);
                }
                HDELAY_ShortTime();
                modular.i2cSCLOut(HIGH);    //拉高时钟线并保持一段时间, 等待从机读取
                HDELAY_LongTime();
                modular.i2cSCLOut(LOW);     //拉低时钟线, 准备下一次写数据
            }
            //每写入1字节数据后等待从机应答
            modular.i2cSDASet(IN);          //数据线设置为读取模式
            modular.i2cSDAOut(HIGH);        //释放数据总线, 交由从机写数据
            HDELAY_ShortTime();
            modular.i2cSCLOut(HIGH);        //拉高时钟线并保持一段时间, 待数据稳定后读入
            modular.delayus(timeout);
            if(modular.i2cSDAIn() == HIGH) {    //从数据线读数据
                modular.i2cSCLOut(LOW);     //拉低时钟线 
                #if WAITING_EXCEPTION == 1
                //在时钟线置高时, 数据线上的一个上升沿表示停止传输数据
                modular.i2cSDASet(OUT);     //数据线设置为写模式
                modular.i2cSDAOut(LOW);     //拉低数据线, 进行准备
                modular.i2cSCLOut(HIGH);    //拉高时钟线
                HDELAY_ShortTime();
                modular.i2cSDAOut(HIGH);    //拉高数据线, 产生一个上升沿
                HDELAY_LongTime();
                return -1;
                #endif
            }else {
                modular.i2cSCLOut(LOW);     //拉低时钟线 
            }
            pdata++;
        }
        //在时钟线置高时, 数据线上的一个上升沿表示停止传输数据
        modular.i2cSDASet(OUT);             //数据线设置为写模式
        modular.i2cSDAOut(LOW);             //拉低数据线, 进行准备
        modular.i2cSCLOut(HIGH);            //拉高时钟线
        HDELAY_ShortTime();
        modular.i2cSDAOut(HIGH);            //拉高数据线, 产生一个上升沿
        HDELAY_LongTime();
    }else if(speed == I2CMEDIUM) {
        //初始化总线
        modular.i2cSDAOut(HIGH);
        modular.i2cSCLOut(LOW);
        //在时钟线置高时, 数据线上的一个下降沿表示开始传输数据
        modular.i2cSDASet(OUT);             //数据线设置为写模式
        modular.i2cSDAOut(HIGH);            //拉高数据线, 进行准备
        modular.i2cSCLOut(HIGH);            //拉高时钟线
        MDELAY_ShortTime();
        modular.i2cSDAOut(LOW);             //拉低数据线, 产生一个下降沿
        MDELAY_LongTime();
        modular.i2cSCLOut(LOW);             //拉低时钟线
        //发送器件地址+写命令, 将数据按位拆分后写入
        modular.i2cSDASet(OUT);             //数据线设置为写模式
        for(i=0; i<8; i++) {
            if(((modular.ADDRESS<<1)|0X00)&(0x80>>i)) { //写数据至数据线
                modular.i2cSDAOut(HIGH);
            }else {
                modular.i2cSDAOut(LOW);
            }
            MDELAY_ShortTime();
            modular.i2cSCLOut(HIGH);        //拉高时钟线并保持一段时间, 等待从机读取
            MDELAY_LongTime();
            modular.i2cSCLOut(LOW);         //拉低时钟线, 准备下一次写数据
        }  
        //每写入1字节数据后等待从机应答
        modular.i2cSDASet(IN);              //数据线设置为读取模式
        modular.i2cSDAOut(HIGH);            //释放数据总线, 交由从机写数据
        MDELAY_ShortTime();
        modular.i2cSCLOut(HIGH);            //拉高时钟线并保持一段时间, 待数据稳定后读入
        modular.delayus(timeout);
        if(modular.i2cSDAIn() == HIGH) {    //从数据线读数据
            modular.i2cSCLOut(LOW);         //拉低时钟线
            #if WAITING_EXCEPTION == 1
            //在时钟线置高时, 数据线上的一个上升沿表示停止传输数据
            modular.i2cSDASet(OUT);         //数据线设置为写模式
            modular.i2cSDAOut(LOW);         //拉低数据线, 进行准备
            modular.i2cSCLOut(HIGH);        //拉高时钟线
            MDELAY_ShortTime();
            modular.i2cSDAOut(HIGH);        //拉高数据线, 产生一个上升沿
            MDELAY_LongTime();
            return -1;
            #endif
        }else {
            modular.i2cSCLOut(LOW);         //拉低时钟线 
        }
        //写寄存器地址, 将数据按位拆分后写入
        modular.i2cSDASet(OUT);             //数据线设置为写模式
        for(i=0; i<8; i++) {
            if(address&(0x80>>i)) {         //写数据至数据线
                modular.i2cSDAOut(HIGH);
            }else {
                modular.i2cSDAOut(LOW);
            }
            MDELAY_ShortTime();
            modular.i2cSCLOut(HIGH);        //拉高时钟线并保持一段时间, 等待从机读取
            MDELAY_LongTime();
            modular.i2cSCLOut(LOW);         //拉低时钟线, 准备下一次写数据
        }
        //每写入1字节数据后等待从机应答
        modular.i2cSDASet(IN);              //数据线设置为读取模式
        modular.i2cSDAOut(HIGH);            //释放数据总线, 交由从机写数据
        MDELAY_ShortTime();
        modular.i2cSCLOut(HIGH);            //拉高时钟线并保持一段时间, 待数据稳定后读入
        modular.delayus(timeout);
        if(modular.i2cSDAIn() == HIGH) {    //从数据线读数据
            modular.i2cSCLOut(LOW);         //拉低时钟线
            #if WAITING_EXCEPTION == 1
            //在时钟线置高时, 数据线上的一个上升沿表示停止传输数据
            modular.i2cSDASet(OUT);         //数据线设置为写模式
            modular.i2cSDAOut(LOW);         //拉低数据线, 进行准备
            modular.i2cSCLOut(HIGH);        //拉高时钟线
            MDELAY_ShortTime();
            modular.i2cSDAOut(HIGH);        //拉高数据线, 产生一个上升沿
            MDELAY_LongTime();
            return -1;
            #endif
        }else {
            modular.i2cSCLOut(LOW);         //拉低时钟线 
        }
        for(j=0; j<size; j++) {   
            //写1字节数据, 将数据按位拆分后写入
            modular.i2cSDASet(OUT);         //数据线设置为写模式
            for(i=0; i<8; i++) {
                if(*pdata&(0x80>>i)) {      //写数据至数据线
                    modular.i2cSDAOut(HIGH);
                }else {
                    modular.i2cSDAOut(LOW);
                }
                MDELAY_ShortTime();
                modular.i2cSCLOut(HIGH);    //拉高时钟线并保持一段时间, 等待从机读取
                MDELAY_LongTime();
                modular.i2cSCLOut(LOW);     //拉低时钟线, 准备下一次写数据
            }
            //每写入1字节数据后等待从机应答
            modular.i2cSDASet(IN);          //数据线设置为读取模式
            modular.i2cSDAOut(HIGH);        //释放数据总线, 交由从机写数据
            MDELAY_ShortTime();
            modular.i2cSCLOut(HIGH);        //拉高时钟线并保持一段时间, 待数据稳定后读入
            modular.delayus(timeout);
            if(modular.i2cSDAIn() == HIGH) {    //从数据线读数据
                modular.i2cSCLOut(LOW);     //拉低时钟线 
                #if WAITING_EXCEPTION == 1
                //在时钟线置高时, 数据线上的一个上升沿表示停止传输数据
                modular.i2cSDASet(OUT);     //数据线设置为写模式
                modular.i2cSDAOut(LOW);     //拉低数据线, 进行准备
                modular.i2cSCLOut(HIGH);    //拉高时钟线
                MDELAY_ShortTime();
                modular.i2cSDAOut(HIGH);    //拉高数据线, 产生一个上升沿
                MDELAY_LongTime();
                return -1;
                #endif
            }else {
                modular.i2cSCLOut(LOW);     //拉低时钟线 
            }
            pdata++;
        }
        //在时钟线置高时, 数据线上的一个上升沿表示停止传输数据
        modular.i2cSDASet(OUT);             //数据线设置为写模式
        modular.i2cSDAOut(LOW);             //拉低数据线, 进行准备
        modular.i2cSCLOut(HIGH);            //拉高时钟线
        MDELAY_ShortTime();
        modular.i2cSDAOut(HIGH);            //拉高数据线, 产生一个上升沿
        MDELAY_LongTime();
    }else {
        //初始化总线
        modular.i2cSDAOut(HIGH);
        modular.i2cSCLOut(LOW);
        //在时钟线置高时, 数据线上的一个下降沿表示开始传输数据
        modular.i2cSDASet(OUT);             //数据线设置为写模式
        modular.i2cSDAOut(HIGH);            //拉高数据线, 进行准备
        modular.i2cSCLOut(HIGH);            //拉高时钟线
        LDELAY_ShortTime();
        modular.i2cSDAOut(LOW);             //拉低数据线, 产生一个下降沿
        LDELAY_LongTime();
        modular.i2cSCLOut(LOW);             //拉低时钟线
        //发送器件地址+写命令, 将数据按位拆分后写入
        modular.i2cSDASet(OUT);             //数据线设置为写模式
        for(i=0; i<8; i++) {
            if(((modular.ADDRESS<<1)|0X00)&(0x80>>i)) { //写数据至数据线
                modular.i2cSDAOut(HIGH);
            }else {
                modular.i2cSDAOut(LOW);
            }
            LDELAY_ShortTime();
            modular.i2cSCLOut(HIGH);        //拉高时钟线并保持一段时间, 等待从机读取
            LDELAY_LongTime();
            modular.i2cSCLOut(LOW);         //拉低时钟线, 准备下一次写数据
        }  
        //每写入1字节数据后等待从机应答
        modular.i2cSDASet(IN);              //数据线设置为读取模式
        modular.i2cSDAOut(HIGH);            //释放数据总线, 交由从机写数据
        LDELAY_ShortTime();
        modular.i2cSCLOut(HIGH);            //拉高时钟线并保持一段时间, 待数据稳定后读入
        modular.delayus(timeout);
        if(modular.i2cSDAIn() == HIGH) {    //从数据线读数据
            modular.i2cSCLOut(LOW);         //拉低时钟线
            #if WAITING_EXCEPTION == 1
            //在时钟线置高时, 数据线上的一个上升沿表示停止传输数据
            modular.i2cSDASet(OUT);         //数据线设置为写模式
            modular.i2cSDAOut(LOW);         //拉低数据线, 进行准备
            modular.i2cSCLOut(HIGH);        //拉高时钟线
            LDELAY_ShortTime();
            modular.i2cSDAOut(HIGH);        //拉高数据线, 产生一个上升沿
            LDELAY_LongTime();
            return -1;
            #endif
        }else {
            modular.i2cSCLOut(LOW);         //拉低时钟线 
        }
        //写寄存器地址, 将数据按位拆分后写入
        modular.i2cSDASet(OUT);             //数据线设置为写模式
        for(i=0; i<8; i++) {
            if(address&(0x80>>i)) {         //写数据至数据线
                modular.i2cSDAOut(HIGH);
            }else {
                modular.i2cSDAOut(LOW);
            }
            LDELAY_ShortTime();
            modular.i2cSCLOut(HIGH);        //拉高时钟线并保持一段时间, 等待从机读取
            LDELAY_LongTime();
            modular.i2cSCLOut(LOW);         //拉低时钟线, 准备下一次写数据
        }
        //每写入1字节数据后等待从机应答
        modular.i2cSDASet(IN);              //数据线设置为读取模式
        modular.i2cSDAOut(HIGH);            //释放数据总线, 交由从机写数据
        LDELAY_ShortTime();
        modular.i2cSCLOut(HIGH);            //拉高时钟线并保持一段时间, 待数据稳定后读入
        modular.delayus(timeout);
        if(modular.i2cSDAIn() == HIGH) {    //从数据线读数据
            modular.i2cSCLOut(LOW);         //拉低时钟线
            #if WAITING_EXCEPTION == 1
            //在时钟线置高时, 数据线上的一个上升沿表示停止传输数据
            modular.i2cSDASet(OUT);         //数据线设置为写模式
            modular.i2cSDAOut(LOW);         //拉低数据线, 进行准备
            modular.i2cSCLOut(HIGH);        //拉高时钟线
            LDELAY_ShortTime();
            modular.i2cSDAOut(HIGH);        //拉高数据线, 产生一个上升沿
            LDELAY_LongTime();
            return -1;
            #endif
        }else {
            modular.i2cSCLOut(LOW);         //拉低时钟线 
        }
        for(j=0; j<size; j++) {   
            //写1字节数据, 将数据按位拆分后写入
            modular.i2cSDASet(OUT);         //数据线设置为写模式
            for(i=0; i<8; i++) {
                if(*pdata&(0x80>>i)) {      //写数据至数据线
                    modular.i2cSDAOut(HIGH);
                }else {
                    modular.i2cSDAOut(LOW);
                }
                LDELAY_ShortTime();
                modular.i2cSCLOut(HIGH);    //拉高时钟线并保持一段时间, 等待从机读取
                LDELAY_LongTime();
                modular.i2cSCLOut(LOW);     //拉低时钟线, 准备下一次写数据
            }
            //每写入1字节数据后等待从机应答
            modular.i2cSDASet(IN);          //数据线设置为读取模式
            modular.i2cSDAOut(HIGH);        //释放数据总线, 交由从机写数据
            LDELAY_ShortTime();
            modular.i2cSCLOut(HIGH);        //拉高时钟线并保持一段时间, 待数据稳定后读入
            modular.delayus(timeout);
            if(modular.i2cSDAIn() == HIGH) {    //从数据线读数据
                modular.i2cSCLOut(LOW);     //拉低时钟线 
                #if WAITING_EXCEPTION == 1
                //在时钟线置高时, 数据线上的一个上升沿表示停止传输数据
                modular.i2cSDASet(OUT);     //数据线设置为写模式
                modular.i2cSDAOut(LOW);     //拉低数据线, 进行准备
                modular.i2cSCLOut(HIGH);    //拉高时钟线
                LDELAY_ShortTime();
                modular.i2cSDAOut(HIGH);    //拉高数据线, 产生一个上升沿
                LDELAY_LongTime();
                return -1;
                #endif
            }else {
                modular.i2cSCLOut(LOW);     //拉低时钟线 
            }
            pdata++;
        }
        //在时钟线置高时, 数据线上的一个上升沿表示停止传输数据
        modular.i2cSDASet(OUT);             //数据线设置为写模式
        modular.i2cSDAOut(LOW);             //拉低数据线, 进行准备
        modular.i2cSCLOut(HIGH);            //拉高时钟线
        LDELAY_ShortTime();
        modular.i2cSDAOut(HIGH);            //拉高数据线, 产生一个上升沿
        LDELAY_LongTime();
    }
    return 1;
}
