#ifndef __PROTOCOL_H
#define __PROTOCOL_H
#include <stdint.h>
#include <stdbool.h>

#define I2CBUS_DELAY 1           //延时自身耗时1us
#define I2CHS_SCL_HIGH_TIME 0    //I2C高速模式
#define I2CHS_SCL_LOW_TIME 0
#define I2CFP_SCL_HIGH_TIME 2 - I2CBUS_DELAY    //I2C快速模式plus
#define I2CFP_SCL_LOW_TIME 0
#define I2CFM_SCL_HIGH_TIME 2 - I2CBUS_DELAY    //I2C快速模式
#define I2CFM_SCL_LOW_TIME 2 - I2CBUS_DELAY
#define I2CSM_SCL_HIGH_TIME 8 - I2CBUS_DELAY    //I2C标准模式
#define I2CSM_SCL_LOW_TIME 2 - I2CBUS_DELAY
#define I2CLS_SCL_HIGH_TIME 98 - I2CBUS_DELAY    //I2C低速模式
#define I2CLS_SCL_LOW_TIME 2 - I2CBUS_DELAY
// #define I2CBUS_TIMEOUT        //是否启用时钟拉伸
// #define I2CBUS_ARBITRATION    //是否启用总线仲裁
// #define I2CBUS_EXCEPTION    //读取从机应答信号后, 是否进行处理

#define SPIBUS_SCLK_HIGH_TIME 0
#define SPIBUS_SCLK_LOW_TIME 0

typedef enum {
    LOW,
    HIGH
} Potential_TypeDef;
typedef enum {
    IN,
    OUT
} Direct_TypeDef;
////////////////////////////////////////////////////////////////////////////
typedef enum {
    SCL,
    SDA
} I2CBUS_WireTypeDef;
typedef enum {
    I2CBUS_LOWSPEED,     //10kbps
    I2CBUS_STANDARD,     //100kbps
    I2CBUS_FAST,         //400kbps
    I2CBUS_FASTPLUS,     //1Mbps
    I2CBUS_HIGHSPEED,    //3.4Mbps
    // I2CBUS_ULTRAFAST,    //5Mbps
} I2CBUS_SpeedTypeDef;
typedef struct {    //I2C模拟总线结构体
#ifdef I2CBUS_USEPOINTER
    void (*SCL_Set)(bool);
    void (*SDA_Set)(bool);
    void (*SCL_Out)(bool);
    void (*SDA_Out)(bool);
    bool (*SCL_In)(void);
    bool (*SDA_In)(void);
    void (*error)(int8_t err);
    void (*delayus)(uint16_t us);
    void (*delayms)(uint16_t ms);
    int8_t (*delayus_paral)(uint16_t us);
#endif    // I2CBUS_USEPOINTER
} I2C_SoftHandleTypeDef;
typedef struct {                  //I2C通信模块结构体
    uint8_t addr;                 //模块I2C地址
    int8_t wait;                  //模块是否必须响应
    I2CBUS_SpeedTypeDef speed;    //模块I2C速率
    void *bushandle;              //I2C模拟/硬件总线句柄
} I2C_ModuleHandleTypeDef;
////////////////////////////////////////////////////////////////////////////
typedef enum {
    SPIBUS_READ_WRITE,
    SPIBUS_WRITE,
    SPIBUS_READ,
} SPIBUS_RWTypeTypeDef;
typedef struct {    //SPI模拟总线结构体
#ifdef SPIBUS_USEPOINTER
    void (*SCK_Out)(bool);
    void (*SDI_Out)(bool);
    bool (*SDO_In)();
    void (*CS_Out)(bool);
    void (*error)(int8_t err);
    void (*delayus)(uint16_t us);
    void (*delayms)(uint16_t ms);
    int8_t (*delayus_paral)(uint16_t us);
#endif    //SPIBUS_USEPOINTER
} SPI_SoftHandleTypeDef;
typedef struct {        //SPI通信模块结构体
    uint8_t rwtype;     //设备读写类型
    void *bushandle;    //SPI模拟/硬件总线句柄
} SPI_ModuleHandleTypeDef;
////////////////////////////////////////////////////////////////////////////
typedef struct {           //ONEWIRE模拟总线结构体
    uint64_t num;          //总线上的设备数量
    int8_t flag_search;    //总线在最近一段时间内是否进行过搜索以更新设备数量
#ifdef OWREBUS_USEPOINTER
    void (*OWIO_Set)(bool);
    void (*OWIO_Out)(bool);
    bool (*OWIO_In)(void);
    void (*error)(int8_t err);
    void (*delayus)(uint16_t us);
    void (*delayms)(uint16_t ms);
    int8_t (*delayus_paral)(uint16_t us);
#endif    // OWREBUS_USEPOINTER
} ONEWIRE_SoftHandleTypeDef;
typedef struct {        //ONEWIRE通信模块结构体
    uint64_t rom;       //模块64位ROM编码
    void *bushandle;    //ONEWIRE总线句柄
} ONEWIRE_ModuleHandleTypeDef;

void DEVCMNI_SCL_Set(bool dir);
void DEVCMNI_SDA_OWRE_Set(bool dir);
void DEVCMNI_SCL_SCK_Out(bool pot);
void DEVCMNI_SDA_SDI_OWRE_Out(bool pot);
bool DEVCMNI_SCL_In(void);
bool DEVCMNI_SDA_OWRE_In(void);
bool DEVCMNI_SDO_In(void);
void DEVCMNI_CS_Out(bool pot);
void DEVCMNI_Error(int8_t err);
void DEVCMNI_Delayus(uint16_t us);
void DEVCMNI_Delayms(uint16_t ms);
int8_t DEVCMNI_Delayus_paral(uint16_t us);
////////////////////////////////////////////////////////////////////////////
#ifdef DEVICE_I2C_SOFTWARE_ENABLED
#if defined(I2CBUS_USEPOINTER)
#define I2CBUS_SCL_Set(dir) ((I2C_SoftHandleTypeDef *)modular->bushandle)->SCL_Set(dir)
#define I2CBUS_SDA_Set(dir) ((I2C_SoftHandleTypeDef *)modular->bushandle)->SDA_Set(dir)
#define I2CBUS_SCL_Out(pot) ((I2C_SoftHandleTypeDef *)modular->bushandle)->SCL_Out(pot)
#define I2CBUS_SDA_Out(pot) ((I2C_SoftHandleTypeDef *)modular->bushandle)->SDA_Out(pot)
#define I2CBUS_SCL_In() ((I2C_SoftHandleTypeDef *)modular->bushandle)->SCL_In()
#define I2CBUS_SDA_In() ((I2C_SoftHandleTypeDef *)modular->bushandle)->SDA_In()
#define I2CBUS_ERROR(err) ((I2C_SoftHandleTypeDef *)modular->bushandle)->error(err)
#define I2CBUS_Delayus(us) ({if(us) {((I2C_SoftHandleTypeDef *)modular->bushandle)->delayus(us);} })
#define I2CBUS_Delayms(ms) ({if(ms) {((I2C_SoftHandleTypeDef *)modular->bushandle)->delayms(ms);} })
#define I2CBUS_Delayus_paral(us) ((I2C_SoftHandleTypeDef *)modular->bushandle)->delayus_paral(us)
#else
#define I2CBUS_SCL_Set(dir) DEVCMNI_SCL_Set(dir)
#define I2CBUS_SDA_Set(dir) DEVCMNI_SDA_OWRE_Set(dir)
#define I2CBUS_SCL_Out(pot) DEVCMNI_SCL_SCK_Out(pot)
#define I2CBUS_SDA_Out(pot) DEVCMNI_SDA_SDI_OWRE_Out(pot)
#define I2CBUS_SCL_In() DEVCMNI_SCL_In()
#define I2CBUS_SDA_In() DEVCMNI_SDA_OWRE_In()
#define I2CBUS_ERROR(err) DEVCMNI_Error(err)
#define I2CBUS_Delayus(us) ({if(us) {DEVCMNI_Delayus(us);} })
#define I2CBUS_Delayms(ms) ({if(ms) {DEVCMNI_Delayms(ms);} })
#define I2CBUS_Delayus_paral(us) DEVCMNI_Delayus_paral(us)
#endif    // I2CBUS_USEPOINTER
//todo: 根据不同平台设置延时时间/根据输入频率计算延迟时间
static uint8_t I2CBUS_SCL_LOW_TIME = 0, I2CBUS_SCL_HIGH_TIME = 0, I2CBUS_SCL_TIMEOUT = 0;
//todo: 当发送一字节数据后从机未响应的处理机制
static inline int8_t I2CBUS_Init(I2C_ModuleHandleTypeDef *modular, uint32_t timeout) {
    if(modular->speed == I2CBUS_HIGHSPEED) {
        I2CBUS_SCL_HIGH_TIME = I2CHS_SCL_HIGH_TIME;
        I2CBUS_SCL_LOW_TIME = I2CHS_SCL_LOW_TIME;
    } else if(modular->speed == I2CBUS_FASTPLUS) {
        I2CBUS_SCL_HIGH_TIME = I2CFP_SCL_HIGH_TIME;
        I2CBUS_SCL_LOW_TIME = I2CFP_SCL_LOW_TIME;
    } else if(modular->speed == I2CBUS_FAST) {
        I2CBUS_SCL_HIGH_TIME = I2CFM_SCL_HIGH_TIME;
        I2CBUS_SCL_LOW_TIME = I2CFM_SCL_LOW_TIME;
    } else if(modular->speed == I2CBUS_STANDARD) {
        I2CBUS_SCL_HIGH_TIME = I2CSM_SCL_HIGH_TIME;
        I2CBUS_SCL_LOW_TIME = I2CSM_SCL_LOW_TIME;
    } else if(modular->speed == I2CBUS_LOWSPEED) {
        I2CBUS_SCL_HIGH_TIME = I2CLS_SCL_HIGH_TIME;
        I2CBUS_SCL_LOW_TIME = I2CLS_SCL_LOW_TIME;
    }
    I2CBUS_SCL_TIMEOUT = timeout > I2CBUS_SCL_HIGH_TIME ? timeout : I2CBUS_SCL_HIGH_TIME;
    //初始化总线
    I2CBUS_SCL_Out(HIGH);
    I2CBUS_SDA_Out(HIGH);
    I2CBUS_SCL_Set(IN);
    I2CBUS_SDA_Set(IN);
    if(I2CBUS_SCL_In() != HIGH || I2CBUS_SDA_In() != HIGH) {
        return 1;    //若总线没有被释放, 返回错误值
    }
    I2CBUS_SCL_Set(OUT);
    I2CBUS_SDA_Set(OUT);
    I2CBUS_SCL_Out(LOW);
    return 0;
}
static inline void I2CBUS_Arbitration(I2C_ModuleHandleTypeDef *modular) {
    I2CBUS_ERROR(1);
}
static inline void I2CBUS_Start(I2C_ModuleHandleTypeDef *modular) {
    //在时钟线置高时, 数据线上的一个下降沿表示开始传输数据
    I2CBUS_SDA_Out(HIGH);    //拉高数据线, 进行准备
    I2CBUS_Delayus(I2CBUS_SCL_LOW_TIME);
    I2CBUS_SCL_Out(HIGH);    //拉高时钟线
#ifdef I2CBUS_TIMEOUT
    I2CBUS_SCL_Set(IN);
    while(!I2CBUS_Delayus_paral(I2CBUS_SCL_TIMEOUT))
        if(I2CBUS_SCL_In())
            break;
    I2CBUS_SCL_Set(OUT);
#endif    // I2CBUS_TIMEOUT
#ifdef I2CBUS_ARBITRATION
    I2CBUS_SDA_Set(IN);
    if(!I2CBUS_SDA_In()) {
        I2CBUS_Arbitration(modular);
    }
    I2CBUS_SDA_Set(OUT);
#endif                      // I2CBUS_ARBITRATION
    I2CBUS_SDA_Out(LOW);    //拉低数据线, 产生一个下降沿
    I2CBUS_Delayus(I2CBUS_SCL_HIGH_TIME);
    I2CBUS_SCL_Out(LOW);    //拉低时钟线
}
static inline void I2CBUS_Stop(I2C_ModuleHandleTypeDef *modular) {
    //在时钟线置高时, 数据线上的一个上升沿表示停止传输数据
    I2CBUS_SDA_Out(LOW);    //拉低数据线, 进行准备
    I2CBUS_Delayus(I2CBUS_SCL_LOW_TIME);
    I2CBUS_SCL_Out(HIGH);    //拉高时钟线
#ifdef I2CBUS_TIMEOUT
    I2CBUS_SCL_Set(IN);
    while(!I2CBUS_Delayus_paral(I2CBUS_SCL_TIMEOUT))
        if(I2CBUS_SCL_In())
            break;
    I2CBUS_SCL_Set(OUT);
#endif                       // I2CBUS_TIMEOUT
    I2CBUS_SDA_Out(HIGH);    //拉高数据线, 产生一个上升沿
    I2CBUS_Delayus(I2CBUS_SCL_HIGH_TIME);
#ifdef I2CBUS_ARBITRATION
    I2CBUS_SDA_Set(IN);
    if(!I2CBUS_SDA_In()) {
        I2CBUS_Arbitration(modular);
    }
    I2CBUS_SDA_Set(OUT);
#endif    // I2CBUS_ARBITRATION
}
static inline void I2CBUS_WriteBit(I2C_ModuleHandleTypeDef *modular, bool bit) {
    //写1位数据
    //I2C通信建立后, 在时钟线拉低期间, 数据线可进行操作, 主机写数据; 时钟线拉高期间, 数据不可进行操作, 从机读数据
    I2CBUS_SDA_Out(bit);    //写数据至数据线
    I2CBUS_Delayus(I2CBUS_SCL_LOW_TIME);
    I2CBUS_SCL_Out(HIGH);    //拉高时钟线并保持一段时间, 等待从机读取
    I2CBUS_Delayus(I2CBUS_SCL_HIGH_TIME);
#ifdef I2CBUS_TIMEOUT
    I2CBUS_SCL_Set(IN);
    while(!I2CBUS_Delayus_paral(I2CBUS_SCL_TIMEOUT))
        if(I2CBUS_SCL_In())
            break;
    I2CBUS_SCL_Set(OUT);
#endif    // I2CBUS_TIMEOUT
#ifdef I2CBUS_ARBITRATION
    I2CBUS_SDA_Set(IN);
    if(bit && !I2CBUS_SDA_In()) {
        I2CBUS_Arbitration(modular);
    }
    I2CBUS_SDA_Set(OUT);
#endif                      // I2CBUS_ARBITRATION
    I2CBUS_SCL_Out(LOW);    //拉低时钟线
}
static inline bool I2CBUS_ReadBit(I2C_ModuleHandleTypeDef *modular) {
    //读1位数据
    //I2C通信建立后, 在时钟线拉低期间, 数据线可进行操作, 从机写数据; 时钟线拉高期间, 数据不可进行操作, 主机读数据
    bool bit = 0;
    // I2CBUS_SDA_Out(HIGH);    //释放数据总线
    // I2CBUS_SDA_Set(IN);      //数据线设置为读取模式
    I2CBUS_Delayus(I2CBUS_SCL_LOW_TIME);
    I2CBUS_SCL_Out(HIGH);    //拉高时钟线并保持一段时间, 待数据稳定后读入
#ifdef I2CBUS_TIMEOUT
    I2CBUS_SCL_Set(IN);
    while(!I2CBUS_Delayus_paral(I2CBUS_SCL_TIMEOUT))
        if(I2CBUS_SCL_In())
            break;
    I2CBUS_SCL_Set(OUT);
#endif    // I2CBUS_TIMEOUT
    I2CBUS_Delayus(I2CBUS_SCL_HIGH_TIME);
    bit = I2CBUS_SDA_In();    //从数据线读数据
    I2CBUS_SCL_Out(LOW);      //拉低时钟线
    // I2CBUS_SDA_Set(OUT);      //数据线设置为写入模式
    return bit;
}
////////////////////////////////////////////////////////////////////////////
static inline int8_t I2CBUS_WriteByte(I2C_ModuleHandleTypeDef *modular, uint8_t byte) {
    //写1字节数据, 将数据按位拆分后写入
    for(uint8_t i = 0x80; i; i >>= 1) {
        I2CBUS_WriteBit(modular, byte & i ? 1 : 0);
    }
    I2CBUS_SDA_Out(HIGH);    //释放数据总线
    I2CBUS_SDA_Set(IN);      //数据线设置为读取模式
    //每写入1字节数据后等待从机应答, 写入完毕, 主机释放(即拉高)时钟线和数据线.
    //一定时间内若能检测到从机拉低数据线, 说明从机应答正常, 可以继续进行通信; 否则说明从机存在问题, 需要重新建立通信
    if(I2CBUS_ReadBit(modular) == 1) {
        I2CBUS_SDA_Set(OUT);    //数据线设置为写入模式
        I2CBUS_Stop(modular);
        return 1;
    }
    return 0;
}
static inline uint8_t I2CBUS_ReadByte(I2C_ModuleHandleTypeDef *modular) {
    //读1字节数据, 将数据按位读取后合并
    uint8_t byte = 0;
    I2CBUS_SDA_Out(HIGH);    //释放数据总线
    I2CBUS_SDA_Set(IN);      //数据线设置为读取模式
    for(uint8_t i = 0x80; i; i >>= 1) {
        byte |= I2CBUS_ReadBit(modular) ? i : 0x00;
    }
    I2CBUS_SDA_Set(OUT);    //数据线设置为写入模式
    //每读取1字节数据后主机进行应答, 读取完毕, 从机会释放(即拉高)数据线.
    //主机写入1位低电平信号, 表示应答正常, 从机继续进行通信; 主机写入1位高电平信号, 表示不再接收数据, 从机停止通信
    I2CBUS_WriteBit(modular, 0);
    return byte;
}
static inline int8_t I2CBUS_Write(I2C_ModuleHandleTypeDef *modular, uint8_t *pdata, uint16_t size) {
    for(uint16_t j = 0; j < size; j++) {
        for(uint8_t i = 0x80; i; i >>= 1) {
            I2CBUS_WriteBit(modular, pdata[j] & i ? 1 : 0);
        }
        I2CBUS_SDA_Out(HIGH);
        I2CBUS_SDA_Set(IN);
        if(I2CBUS_ReadBit(modular) == 1) {
            I2CBUS_SDA_Set(OUT);
            I2CBUS_Stop(modular);
            return 1;
        }
    }
    return 0;
}
static inline void I2CBUS_Read(I2C_ModuleHandleTypeDef *modular, uint8_t *pdata, uint16_t size) {
    for(uint16_t j = 0; j < size; j++) {
        I2CBUS_SDA_Out(HIGH);
        I2CBUS_SDA_Set(IN);
        for(uint8_t i = 0x80; i; i >>= 1) {
            pdata[j] |= I2CBUS_ReadBit(modular) ? i : 0x00;
        }
        I2CBUS_SDA_Set(OUT);
        if(j + 1 < size) {
            I2CBUS_WriteBit(modular, 1);
        } else {
            I2CBUS_WriteBit(modular, 0);
        }
    }
}
////////////////////////////////////////////////////////////////////////////
__attribute__((unused)) static int8_t DEV_I2C_WriteByte(I2C_ModuleHandleTypeDef *modular, uint8_t address, uint8_t byte, int8_t skip, uint32_t timeout) {
    I2CBUS_Init(modular, timeout);
    I2CBUS_Start(modular);
    if(I2CBUS_WriteByte(modular, (modular->addr << 1) | 0X00) == 1) {    //发送器件地址+写命令, 将数据按位拆分后写入
#ifdef I2CBUS_EXCEPTION
        I2CBUS_ERROR(2);
#else
        return 1;
#endif
    }
    if(!skip) {
        if(I2CBUS_WriteByte(modular, address) == 1) {
#ifdef I2CBUS_EXCEPTION
            I2CBUS_ERROR(3);
#else
            return 1;
#endif
        }
    }
    if(I2CBUS_WriteByte(modular, byte) == 1) {    //发送数据
#ifdef I2CBUS_EXCEPTION
        I2CBUS_ERROR(3);
#else
        return 1;
#endif
    }
    I2CBUS_Stop(modular);
    return 0;
}
__attribute__((unused)) static uint8_t DEV_I2C_ReadByte(I2C_ModuleHandleTypeDef *modular, uint8_t address, int8_t skip, uint32_t timeout) {
    uint8_t byte = 0;
    I2CBUS_Init(modular, timeout);
    if(!skip) {
        I2CBUS_Start(modular);
        if(I2CBUS_WriteByte(modular, (modular->addr << 1) | 0X00) == 1) {    //发送器件地址+写命令, 将数据按位拆分后写入
#ifdef I2CBUS_EXCEPTION
            I2CBUS_ERROR(2);
#endif
        }
        if(I2CBUS_WriteByte(modular, address) == 1) {
#ifdef I2CBUS_EXCEPTION
            I2CBUS_ERROR(3);
#endif
        }
    }
    I2CBUS_Start(modular);
    if(I2CBUS_WriteByte(modular, (modular->addr << 1) | 0X01) == 1) {    //发送器件地址+读命令, 将数据按位拆分后写入
#ifdef I2CBUS_EXCEPTION
        I2CBUS_ERROR(3);
#endif
    }
    byte = I2CBUS_ReadByte(modular);
    I2CBUS_Stop(modular);
    return byte;
}
//多字节写入函数, timeout应答超时,speed速度模式
__attribute__((unused)) static int8_t DEV_I2C_Write(I2C_ModuleHandleTypeDef *modular, uint8_t address, uint8_t *pdata, uint16_t size, int8_t skip, uint32_t timeout) {
    I2CBUS_Init(modular, timeout);
    I2CBUS_Start(modular);
    if(I2CBUS_WriteByte(modular, (modular->addr << 1) | 0X00) == 1) {    //发送器件地址+写命令, 将数据按位拆分后写入
#ifdef I2CBUS_EXCEPTION
        I2CBUS_ERROR(2);
#else
        return 1;
#endif
    }
    if(!skip) {
        if(I2CBUS_WriteByte(modular, address) == 1) {
#ifdef I2CBUS_EXCEPTION
            I2CBUS_ERROR(3);
#else
            return 1;
#endif
        }
    }
    if(I2CBUS_Write(modular, pdata, size) == 1) {
#ifdef I2CBUS_EXCEPTION
        I2CBUS_ERROR(3);
#else
        return 1;
#endif
    }
    I2CBUS_Stop(modular);
    return 0;
}
//多字节读取函数, timeout应答超时,speed速度模式
__attribute__((unused)) static int8_t DEV_I2C_Read(I2C_ModuleHandleTypeDef *modular, uint8_t address, uint8_t *pdata, uint16_t size, int8_t skip, uint32_t timeout) {
    I2CBUS_Init(modular, timeout);
    if(!skip) {
        I2CBUS_Start(modular);
        if(I2CBUS_WriteByte(modular, (modular->addr << 1) | 0X00) == 1) {    //发送器件地址+写命令, 将数据按位拆分后写入
#ifdef I2CBUS_EXCEPTION
            I2CBUS_ERROR(2);
#else
            return 1;
#endif
        }
        if(I2CBUS_WriteByte(modular, address) == 1) {
#ifdef I2CBUS_EXCEPTION
            I2CBUS_ERROR(3);
#else
            return 1;
#endif
        }
    }
    I2CBUS_Start(modular);
    if(I2CBUS_WriteByte(modular, (modular->addr << 1) | 0X01) == 1) {    //发送器件地址+读命令, 将数据按位拆分后写入
#ifdef I2CBUS_EXCEPTION
        I2CBUS_ERROR(3);
#else
        return 1;
#endif
    }
    I2CBUS_Read(modular, pdata, size);
    I2CBUS_Stop(modular);
    return 0;
}
#endif    // DEVICE_I2C_SOFTWARE_ENABLED


////////////////////////////////////////////////////////////////////////////
#ifdef DEVICE_SPI_SOFTWARE_ENABLED
#if defined(SPIBUS_USEPOINTER)
#define SPIBUS_SCK_Out(pot) ((SPI_SoftHandleTypeDef *)modular->bushandle)->SCK_Out(pot)
#define SPIBUS_SDI_Out(pot) ((SPI_SoftHandleTypeDef *)modular->bushandle)->SDI_Out(pot)
#define SPIBUS_SDO_In() ((SPI_SoftHandleTypeDef *)modular->bushandle)->SDO_In()
#define SPIBUS_CS_Out(pot) ((SPI_SoftHandleTypeDef *)modular->bushandle)->CS_Out(pot)
#define SPIBUS_ERROR(err) ((SPI_SoftHandleTypeDef *)modular->bushandle)->error(err)
#define SPIBUS_delayus(us) ({if(us) {((SPI_SoftHandleTypeDef *)modular->bushandle)->delayus(us);} })
#define SPIBUS_delayms(ms) ({if(ms) {((SPI_SoftHandleTypeDef *)modular->bushandle)->delayms(ms);} })
#define SPIBUS_Delayus_paral(us) ((SPI_SoftHandleTypeDef *)modular->bushandle)->delayus_paral(us)
#else
#define SPIBUS_SCK_Out(pot) DEVCMNI_SCL_SCK_Out(pot)
#define SPIBUS_SDI_Out(pot) DEVCMNI_SDA_SDI_OWRE_Out(pot)
#define SPIBUS_SDO_In() DEVCMNI_SDO_In()
#define SPIBUS_CS_Out(pot) DEVCMNI_CS_Out(pot)
#define SPIBUS_ERROR(err) DEVCMNI_Error(err)
#define SPIBUS_delayus(us) ({if(us) {DEVCMNI_Delayus(us);} })
#define SPIBUS_delayms(ms) ({if(ms) {DEVCMNI_Delayms(ms);} })
#define SPIBUS_Delayus_paral(us) DEVCMNI_Delayus_paral(us)
#endif    // SPIBUS_USEPOINTER
static inline void SPIBUS_Init(SPI_ModuleHandleTypeDef *modular) {
    SPIBUS_CS_Out(HIGH);
    SPIBUS_SCK_Out(HIGH);
    SPIBUS_SDI_Out(HIGH);
    SPIBUS_CS_Out(LOW);
}
static inline void SPIBUS_WriteBit(SPI_ModuleHandleTypeDef *modular, bool bit) {
    SPIBUS_SCK_Out(LOW);
    SPIBUS_SDI_Out(bit);
    SPIBUS_delayus(SPIBUS_SCLK_LOW_TIME);
    SPIBUS_SCK_Out(HIGH);
    SPIBUS_delayus(SPIBUS_SCLK_HIGH_TIME);
}
static inline bool SPIBUS_ReadBit(SPI_ModuleHandleTypeDef *modular) {
    bool bit;
    SPIBUS_SCK_Out(LOW);
    SPIBUS_delayus(SPIBUS_SCLK_LOW_TIME);
    SPIBUS_SCK_Out(HIGH);
    bit = SPIBUS_SDO_In();    //tofix: 这里应该先等后读还是先读后等, 有待测试
    SPIBUS_delayus(SPIBUS_SCLK_HIGH_TIME);
    return bit;
}
static inline bool SPIBUS_TransmitBit(SPI_ModuleHandleTypeDef *modular, bool bit) {
    SPIBUS_SCK_Out(LOW);
    SPIBUS_SDI_Out(bit);
    SPIBUS_delayus(SPIBUS_SCLK_LOW_TIME);
    SPIBUS_SCK_Out(HIGH);
    bit = SPIBUS_SDO_In();    //tofix: 这里应该先等后读还是先读后等, 有待测试
    SPIBUS_delayus(SPIBUS_SCLK_HIGH_TIME);
    return bit;
}
////////////////////////////////////////////////////////////////////////////
//单字节写入函数
__attribute__((unused)) static uint8_t DEV_SPI_TransmitByte(SPI_ModuleHandleTypeDef *modular, uint8_t byte, int8_t skip, uint32_t timeout) {
    SPIBUS_Init(modular);
    if(!skip) {
        SPIBUS_CS_Out(LOW);    //拉低片选
    }
    if(modular->rwtype == SPIBUS_READ_WRITE) {
        for(uint8_t i = 0x80; i; i >>= 1) {
            byte |= SPIBUS_TransmitBit(modular, byte & i ? 1 : 0) ? i : 0x00;
        }
    } else if(modular->rwtype == SPIBUS_WRITE) {
        for(uint8_t i = 0x80; i; i >>= 1) {
            SPIBUS_WriteBit(modular, byte & i ? 1 : 0);
        }
    } else if(modular->rwtype == SPIBUS_READ) {
        for(uint8_t i = 0x80; i; i >>= 1) {
            byte |= SPIBUS_ReadBit(modular) ? i : 0x00;
        }
    }
    if(!skip) {
        SPIBUS_CS_Out(HIGH);    //拉高片选
    }
    return byte;
}
//多字节写入函数
__attribute__((unused)) static void DEV_SPI_Transmit(SPI_ModuleHandleTypeDef *modular, uint8_t *pdata, uint16_t size, int8_t skip, uint32_t timeout) {
    SPIBUS_Init(modular);
    if(!skip) {
        SPIBUS_CS_Out(LOW);    //拉低片选
    }
    if(modular->rwtype == SPIBUS_READ_WRITE) {
        for(uint16_t j = 0; j < size; j++) {
            pdata[j] |= SPIBUS_TransmitBit(modular, pdata[j] & 0x80 ? 1 : 0) ? 0x80 : 0x00;
            pdata[j] |= SPIBUS_TransmitBit(modular, pdata[j] & 0x40 ? 1 : 0) ? 0x40 : 0x00;
            pdata[j] |= SPIBUS_TransmitBit(modular, pdata[j] & 0x20 ? 1 : 0) ? 0x20 : 0x00;
            pdata[j] |= SPIBUS_TransmitBit(modular, pdata[j] & 0x10 ? 1 : 0) ? 0x10 : 0x00;
            pdata[j] |= SPIBUS_TransmitBit(modular, pdata[j] & 0x08 ? 1 : 0) ? 0x08 : 0x00;
            pdata[j] |= SPIBUS_TransmitBit(modular, pdata[j] & 0x04 ? 1 : 0) ? 0x04 : 0x00;
            pdata[j] |= SPIBUS_TransmitBit(modular, pdata[j] & 0x02 ? 1 : 0) ? 0x02 : 0x00;
            pdata[j] |= SPIBUS_TransmitBit(modular, pdata[j] & 0x01 ? 1 : 0) ? 0x01 : 0x00;
        }
    } else if(modular->rwtype == SPIBUS_WRITE) {
        for(uint16_t j = 0; j < size; j++) {
            SPIBUS_WriteBit(modular, pdata[j] & 0x80);
            SPIBUS_WriteBit(modular, pdata[j] & 0x40);
            SPIBUS_WriteBit(modular, pdata[j] & 0x20);
            SPIBUS_WriteBit(modular, pdata[j] & 0x10);
            SPIBUS_WriteBit(modular, pdata[j] & 0x08);
            SPIBUS_WriteBit(modular, pdata[j] & 0x04);
            SPIBUS_WriteBit(modular, pdata[j] & 0x02);
            SPIBUS_WriteBit(modular, pdata[j] & 0x01);
        }
    } else if(modular->rwtype == SPIBUS_READ) {
        for(uint16_t j = 0; j < size; j++) {
            pdata[j] |= SPIBUS_ReadBit(modular) ? 0x80 : 0x00;
            pdata[j] |= SPIBUS_ReadBit(modular) ? 0x40 : 0x00;
            pdata[j] |= SPIBUS_ReadBit(modular) ? 0x20 : 0x00;
            pdata[j] |= SPIBUS_ReadBit(modular) ? 0x10 : 0x00;
            pdata[j] |= SPIBUS_ReadBit(modular) ? 0x08 : 0x00;
            pdata[j] |= SPIBUS_ReadBit(modular) ? 0x04 : 0x00;
            pdata[j] |= SPIBUS_ReadBit(modular) ? 0x02 : 0x00;
            pdata[j] |= SPIBUS_ReadBit(modular) ? 0x01 : 0x00;
        }
    }
    if(!skip) {
        SPIBUS_CS_Out(HIGH);    //拉高片选
    }
}
#endif    // DEVICE_SPI_SOFTWARE_ENABLED

////////////////////////////////////////////////////////////////////////////
#ifdef DEVICE_ONEWIRE_SOFTWARE_ENABLED
#if defined(OWREBUS_USEPOINTER)
#define OWREBUS_OWIO_Set(dir) ((ONEWIRE_SoftHandleTypeDef *)modular->bushandle)->OWIO_Set(dir)
#define OWREBUS_OWIO_Out(pot) ((ONEWIRE_SoftHandleTypeDef *)modular->bushandle)->OWIO_Out(pot)
#define OWREBUS_OWIO_In() ((ONEWIRE_SoftHandleTypeDef *)modular->bushandle)->OWIO_In()
#define OWREBUS_ERROR(err) ((ONEWIRE_SoftHandleTypeDef *)modular->bushandle)->error(err)
#define OWREBUS_delayus(us) ({if(us) {((ONEWIRE_SoftHandleTypeDef *)modular->bushandle)->delayus(us);} })
#define OWREBUS_delayms(ms) ({if(ms) {((ONEWIRE_SoftHandleTypeDef *)modular->bushandle)->delayms(ms);} })
#define OWREBUS_Delayus_paral(us) ((ONEWIRE_SoftHandleTypeDef *)modular->bushandle)->delayus_paral(us)
#else
#define OWREBUS_OWIO_Set(pot) DEVCMNI_SDA_OWRE_Set(pot)
#define OWREBUS_OWIO_Out(pot) DEVCMNI_SDA_SDI_OWRE_Out(pot)
#define OWREBUS_OWIO_In(pot) DEVCMNI_SDA_OWRE_In(pot)
#define OWREBUS_ERROR(err) DEVCMNI_Error(err)
#define OWREBUS_delayus(us) ({if(us) {DEVCMNI_Delayus(us);} })
#define OWREBUS_delayms(ms) ({if(ms) {DEVCMNI_Delayms(ms);} })
#define OWREBUS_Delayus_paral(us) DEVCMNI_Delayus_paral(us)
#endif    // OWREBUS_USEPOINTER
static inline int8_t OWREBUS_Init(ONEWIRE_ModuleHandleTypeDef *modular) {
    OWREBUS_OWIO_Out(HIGH);
    OWREBUS_OWIO_Set(IN);
    if(OWREBUS_OWIO_In() != HIGH) {
        return 1;    //若总线没有被释放, 返回错误值
    }
    OWREBUS_OWIO_Set(OUT);
    return 0;
}
static inline uint8_t OWREBUS_Reset(ONEWIRE_ModuleHandleTypeDef *modular) {
    uint8_t result = 0;
    OWREBUS_OWIO_Out(LOW);    //拉低总线480us, 发出复位信号
    OWREBUS_delayus(480);
    //todo: 关中断      //应答信号最好在60-120us期间读取, 应关闭中断
    OWREBUS_OWIO_Out(HIGH);    //释放总线, 等待60us
    OWREBUS_delayus(60);
    OWREBUS_OWIO_Set(IN);              //读取总线信息
    if(OWREBUS_OWIO_In() == HIGH) {    //若总线被拉低, 返回0, 否则返回1
        result = 1;
    } else {
        result = 0;
    }
    //todo: 开中断
    OWREBUS_delayus(420);
    OWREBUS_OWIO_Set(OUT);
    return result;
}
static inline void OWREBUS_WriteBit(ONEWIRE_ModuleHandleTypeDef *modular, bool bit) {
    if(bit) {
        //todo: 关中断
        OWREBUS_OWIO_Out(LOW);
        OWREBUS_delayus(5);
        OWREBUS_OWIO_Out(HIGH);
        //todo: 开中断
        OWREBUS_delayus(55);
    } else {
        OWREBUS_OWIO_Out(LOW);
        OWREBUS_delayus(60);
        OWREBUS_OWIO_Out(HIGH);
        OWREBUS_delayus(1);
    }
}
static inline uint8_t OWREBUS_ReadBit(ONEWIRE_ModuleHandleTypeDef *modular) {
    bool bit;
    //todo: 关中断
    OWREBUS_OWIO_Out(LOW);     //拉低总线1us, 开始读时隙
    OWREBUS_delayus(1);        //若主控的主频较低, 可跳过1us的延时
    OWREBUS_OWIO_Out(HIGH);    //释放总线
    OWREBUS_OWIO_Set(IN);
    OWREBUS_delayus(10);    //在15us内的最后时刻读取总线, 考虑到误差只延时10us
    if(OWREBUS_OWIO_In() == HIGH) {
        bit = 1;
    } else {
        bit = 0;
    }
    //todo: 开中断
    if(bit) {
        OWREBUS_delayus(50);
    } else {
        OWREBUS_delayus(51);
    }
    OWREBUS_OWIO_Set(OUT);
    return bit;
}
static inline void OWREBUS_WriteByte(ONEWIRE_ModuleHandleTypeDef *modular, uint8_t byte) {
    for(uint8_t i = 0x01; i; i <<= 1) {
        OWREBUS_WriteBit(modular, byte & i ? 1 : 0);
    }
}
static inline uint8_t OWREBUS_ReadByte(ONEWIRE_ModuleHandleTypeDef *modular) {
    uint8_t byte = 0;
    for(uint8_t i = 0x01; i; i <<= 1) {
        byte |= OWREBUS_ReadBit(modular) ? i : 0x00;
    }
    return byte;
}
static inline void OWREBUS_Write(ONEWIRE_ModuleHandleTypeDef *modular, uint8_t *pdata, uint16_t size) {
    for(uint16_t j = 0; j < size; j++) {
        for(uint8_t i = 0x01; i; i <<= 1) {
            OWREBUS_WriteBit(modular, pdata[j] & i ? 1 : 0);
        }
    }
    return;
}
static inline void OWREBUS_Read(ONEWIRE_ModuleHandleTypeDef *modular, uint8_t *pdata, uint16_t size) {
    for(uint16_t j = 0; j < size; j++) {
        for(uint8_t i = 0x01; i != 0; i <<= 1) {
            pdata[j] |= OWREBUS_ReadBit(modular) ? i : 0x00;
        }
    }
    return;
}
////////////////////////////////////////////////////////////////////////////
static inline void OWREBUS_Skip(ONEWIRE_ModuleHandleTypeDef *modular) {    //ROM跳过匹配指令[限单个设备/接多个设备能同时进行的操作]
    OWREBUS_WriteByte(modular, 0xCC);
}
static inline void OWREBUS_Match(ONEWIRE_ModuleHandleTypeDef *modular) {    //ROM匹配指令
    OWREBUS_WriteByte(modular, 0x55);
    OWREBUS_Write(modular, (uint8_t *)&modular->rom, 8);
}
static inline void OWREBUS_Query(ONEWIRE_ModuleHandleTypeDef *modular) {    //ROM读取指令[限单个设备]
    OWREBUS_WriteByte(modular, 0x33);
}
static inline void OWREBUS_Search(ONEWIRE_ModuleHandleTypeDef *modular) {    //ROM搜索指令
    OWREBUS_WriteByte(modular, 0xF0);
}
static inline void OWREBUS_AlarmSearch(ONEWIRE_ModuleHandleTypeDef *modular) {    //ROM报警搜索指令
    OWREBUS_WriteByte(modular, 0xEC);
}
////////////////////////////////////////////////////////////////////////////
/**
 * @description: 单总线协议ROM搜索/报警搜索操作
 * 在发起一个复位信号后, 根据通信模块数量, 发出ROM读取/搜索指令, 或发出ROM报警搜索指令
 * @param {ONEWIRE_ModuleHandleTypeDef} *modular
 * @param {int8_t} searchtype
 * @return {*}
 */
__attribute__((unused)) static void DEV_ONEWIRE_Search(ONEWIRE_ModuleHandleTypeDef *modular, int8_t searchtype) {
    OWREBUS_Init(modular);
    OWREBUS_Reset(modular);
    if(searchtype == 0) {
        if(((ONEWIRE_SoftHandleTypeDef *)modular->bushandle)->num == 1) {
            OWREBUS_Query(modular);
        } else if(((ONEWIRE_SoftHandleTypeDef *)modular->bushandle)->num > 1) {
            OWREBUS_Search(modular);
            //...
        }
    } else {
        OWREBUS_AlarmSearch(modular);
        //...
    }
}
__attribute__((unused)) static void DEV_ONEWIRE_WriteByte(ONEWIRE_ModuleHandleTypeDef *modular, uint8_t byte, int8_t skip, uint32_t timeout) {
    OWREBUS_Init(modular);
    OWREBUS_Reset(modular);
    if(skip || ((ONEWIRE_SoftHandleTypeDef *)modular->bushandle)->num <= 1) {
        OWREBUS_Skip(modular);
    } else if(((ONEWIRE_SoftHandleTypeDef *)modular->bushandle)->num > 1) {
        OWREBUS_Match(modular);
    }
    OWREBUS_WriteByte(modular, byte);
}
__attribute__((unused)) static void DEV_ONEWIRE_WriteWord(ONEWIRE_ModuleHandleTypeDef *modular, uint16_t byte, int8_t skip, uint32_t timeout) {
    OWREBUS_Init(modular);
    OWREBUS_Reset(modular);
    if(skip || ((ONEWIRE_SoftHandleTypeDef *)modular->bushandle)->num == 1) {
        OWREBUS_Skip(modular);
    } else if(((ONEWIRE_SoftHandleTypeDef *)modular->bushandle)->num > 1) {
        OWREBUS_Match(modular);
    }
    OWREBUS_WriteByte(modular, (uint8_t)byte);
    OWREBUS_WriteByte(modular, (uint8_t)(byte >> 8));
}
__attribute__((unused)) static uint8_t DEV_ONEWIRE_ReadByte(ONEWIRE_ModuleHandleTypeDef *modular) {
    return OWREBUS_ReadByte(modular);
}
__attribute__((unused)) static uint16_t DEV_ONEWIRE_ReadWord(ONEWIRE_ModuleHandleTypeDef *modular) {
    return (uint16_t)OWREBUS_ReadByte(modular) | ((uint16_t)OWREBUS_ReadByte(modular) << 8);
}
__attribute__((unused)) static uint8_t DEV_ONEWIRE_ReadBit(ONEWIRE_ModuleHandleTypeDef *modular) {
    return OWREBUS_ReadBit(modular);
}
__attribute__((unused)) static void DEV_ONEWIRE_Write(ONEWIRE_ModuleHandleTypeDef *modular, uint8_t *pdata, uint16_t size, int8_t skip, uint32_t timeout) {
    OWREBUS_Init(modular);
    OWREBUS_Reset(modular);
    if(skip || ((ONEWIRE_SoftHandleTypeDef *)modular->bushandle)->num == 1) {
        OWREBUS_Skip(modular);
    } else if(((ONEWIRE_SoftHandleTypeDef *)modular->bushandle)->num > 1) {
        OWREBUS_Match(modular);
    }
    OWREBUS_Write(modular, pdata, size);
    return;
}
__attribute__((unused)) static void DEV_ONEWIRE_Read(ONEWIRE_ModuleHandleTypeDef *modular, uint8_t *pdata, uint16_t size) {
    //tofix: 单总线的读时隙只能跟随在特定的主机写指令之后吗? 当需要并发地与多个设备进行通信时, 怎样进行独立的读操作?
    OWREBUS_Read(modular, pdata, size);
    return;
}
#endif    // DEVICE_ONEWIRE_SOFTWARE_ENABLED

#undef I2CBUS_SCL_Set
#undef I2CBUS_SDA_Set
#undef I2CBUS_SCL_Out
#undef I2CBUS_SDA_Out
#undef I2CBUS_SCL_In
#undef I2CBUS_SDA_In
#undef I2CBUS_ERROR
#undef I2CBUS_Delayus
#undef I2CBUS_Delayms
#undef I2CBUS_Delayus_paral

#undef SPIBUS_SCK_Out
#undef SPIBUS_SDI_Out
#undef SPIBUS_SDO_In
#undef SPIBUS_CS_Out
#undef SPIBUS_ERROR
#undef SPIBUS_delayus
#undef SPIBUS_delayms

#undef OWREBUS_OWIO_Set
#undef OWREBUS_OWIO_Out
#undef OWREBUS_OWIO_In
#undef OWREBUS_ERROR
#undef OWREBUS_delayus
#undef OWREBUS_delayms
#undef OWREBUS_Delayus_paral
#endif    // !__PROTOCOL_H
