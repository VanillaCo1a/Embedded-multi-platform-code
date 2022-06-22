#ifndef __PROTOCOL_H
#define __PROTOCOL_H
#include <stdint.h>
#include <stdbool.h>

#define TIMER_DELAY 1    //延时自身耗时1us

#define I2CLS_SCL_HIGH_TIME  3     //I2C低速模式
#define I2CLS_SCL_LOW_TIME   97    //
#define I2CSM_SCL_HIGH_TIME  3     //I2C标准模式
#define I2CSM_SCL_LOW_TIME   7     //
#define I2CFM_SCL_HIGH_TIME  3     //I2C快速模式
#define I2CFM_SCL_LOW_TIME   1     //
#define I2CFMP_SCL_HIGH_TIME 1     //I2C快速模式plus
#define I2CFMP_SCL_LOW_TIME  1     //FMP以上需要保证SCL低电平期间SDA有足够的时间(或更大的电流)上拉, 实测1k上拉基本稳定
#define I2CHS_SCL_HIGH_TIME  0     //I2C高速模式
#define I2CHS_SCL_LOW_TIME   0     //仍需要0.5us左右的延时
#define I2CUFM_SCL_HIGH_TIME 0     //I2C超快模式
#define I2CUFM_SCL_LOW_TIME  0     //仍需要0.3us左右的延时

#define I2CBUS_EXCEPTION 0    //读取从机应答信号异常后的处理方式

#define SPIBUS_SCLK_HIGH_TIME 0
#define SPIBUS_SCLK_LOW_TIME  0

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
    I2CBUS_LOWSPEED,        //MAX10kbps
    I2CBUS_STANDARD,        //MAX100kbps
    I2CBUS_FASTMODE,        //MAX400kbps
    I2CBUS_FASTMODEPLUS,    //MAX1Mbps
    I2CBUS_HIGHSPEED,       //MAX3.4Mbps, 由于电阻上拉需要约1us, IO浮空输出在这一频率下工作不稳定且速度存在瓶颈
    I2CBUS_ULTRAFAST,       //MAX5Mbps
} I2CBUS_SpeedTypeDef;
typedef enum {
    I2CBUS_NOERROR = 0,
    I2CBUS_TIMEOUT = 1,
    I2CBUS_ARBITRATION = 2,
    I2CBUS_NOACK = 3,
    I2CBUS_NOFOUND = 4,
    I2CBUS_NOANSWER = 5,
} I2CBUS_ErrorTypeDef;
typedef enum {
    I2CBUS_LEVER1 = 0,    //当从机未响应超时, 时钟拉伸超时, 发生总线仲裁时均进入错误处理函数
    I2CBUS_LEVER0 = 1,    //无需从机应答, 在时钟拉伸超时后继续读写下一位, 无视发生的总线仲裁
} I2CBUS_ErrhandTypeDef;
typedef struct {          //I2C模拟总线结构体
    bool clockstretch;    //是否启用时钟拉伸
    bool arbitration;     //是否启用总线仲裁
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
typedef struct {                      //I2C总线设备结构体
    uint8_t addr;                     //模块I2C地址
    I2CBUS_SpeedTypeDef speed;        //模块I2C速率
    I2CBUS_ErrhandTypeDef errhand;    //模块的错误处理方式
    void *bushandle;                  //I2C模拟/硬件总线句柄
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
typedef struct {        //SPI总线模块结构体
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
typedef struct {
    uint64_t rom;       //模块64位ROM编码
    void *bushandle;    //ONEWIRE总线句柄
} ONEWIRE_ModuleHandleTypeDef;


////////////////////////////////////////////////////////////////////////////
#ifdef DEVICE_I2C_SOFTWARE_ENABLED
#if defined(I2CBUS_USEPOINTER)
#define I2CBUS_SCL_Set(dir)      i2cbus.SCL_Set(dir)
#define I2CBUS_SDA_Set(dir)      i2cbus.SDA_Set(dir)
#define I2CBUS_SCL_Out(pot)      i2cbus.SCL_Out(pot)
#define I2CBUS_SDA_Out(pot)      i2cbus.SDA_Out(pot)
#define I2CBUS_SCL_In()          i2cbus.SCL_In()
#define I2CBUS_SDA_In()          i2cbus.SDA_In()
#define I2CBUS_Error(err)        i2cbus.error(err)
#define I2CBUS_Delayus(us)       ({if(us) {i2cbus.delayus(us-TIMER_DELAY);} })
#define I2CBUS_Delayms(ms)       ({if(ms) {i2cbus.delayms(ms);} })
#define I2CBUS_Delayus_paral(us) i2cbus.delayus_paral(us)
#else
#define I2CBUS_SCL_Set(dir)      DEVCMNI_SCL_Set(dir)
#define I2CBUS_SDA_Set(dir)      DEVCMNI_SDA_OWRE_Set(dir)
#define I2CBUS_SCL_Out(pot)      DEVCMNI_SCL_SCK_Out(pot)
#define I2CBUS_SDA_Out(pot)      DEVCMNI_SDA_SDI_OWRE_Out(pot)
#define I2CBUS_SCL_In()          DEVCMNI_SCL_In()
#define I2CBUS_SDA_In()          DEVCMNI_SDA_OWRE_In()
#define I2CBUS_Error(err)        DEVCMNI_Error(err)
#define I2CBUS_Delayus(us)       ({if(us) {DEVCMNI_Delayus(us-TIMER_DELAY);} })
#define I2CBUS_Delayms(ms)       ({if(ms) {DEVCMNI_Delayms(ms);} })
#define I2CBUS_Delayus_paral(us) DEVCMNI_Delayus_paral(us)
#endif    // I2CBUS_USEPOINTER
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
static uint32_t I2CBUS_scl_lowtime = 0, I2CBUS_scl_hightime = 0, I2CBUS_timeout = 0;
static I2CBUS_ErrorTypeDef i2cerror = I2CBUS_NOERROR;
static I2C_SoftHandleTypeDef i2cbus = {.clockstretch = true, .arbitration = true};
static I2C_ModuleHandleTypeDef i2cmodular = {.addr = 0x00, .speed = I2CBUS_STANDARD, .errhand = I2CBUS_LEVER1, .bushandle = &i2cbus};
static int8_t I2CBUS_Init(I2C_ModuleHandleTypeDef *modular, uint32_t timeout) {
    i2cmodular = *modular;
    i2cbus = *((I2C_SoftHandleTypeDef *)modular->bushandle);
    i2cerror = I2CBUS_NOERROR;
    if(i2cmodular.speed >= I2CBUS_ULTRAFAST) {
        i2cmodular.errhand = I2CBUS_LEVER0;
        i2cbus.clockstretch = false;
        i2cbus.arbitration = false;
    }
    //todo: 根据不同平台设置延时时间/根据输入频率计算延迟时间
    if(modular->speed == I2CBUS_ULTRAFAST) {
        I2CBUS_scl_hightime = I2CUFM_SCL_HIGH_TIME;
        I2CBUS_scl_lowtime = I2CUFM_SCL_LOW_TIME;
    } else if(modular->speed == I2CBUS_HIGHSPEED) {
        I2CBUS_scl_hightime = I2CHS_SCL_HIGH_TIME;
        I2CBUS_scl_lowtime = I2CHS_SCL_LOW_TIME;
    } else if(modular->speed == I2CBUS_FASTMODEPLUS) {
        I2CBUS_scl_hightime = I2CFMP_SCL_HIGH_TIME;
        I2CBUS_scl_lowtime = I2CFMP_SCL_LOW_TIME;
    } else if(modular->speed == I2CBUS_FASTMODE) {
        I2CBUS_scl_hightime = I2CFM_SCL_HIGH_TIME;
        I2CBUS_scl_lowtime = I2CFM_SCL_LOW_TIME;
    } else if(modular->speed == I2CBUS_STANDARD) {
        I2CBUS_scl_hightime = I2CSM_SCL_HIGH_TIME;
        I2CBUS_scl_lowtime = I2CSM_SCL_LOW_TIME;
    } else if(modular->speed == I2CBUS_LOWSPEED) {
        I2CBUS_scl_hightime = I2CLS_SCL_HIGH_TIME;
        I2CBUS_scl_lowtime = I2CLS_SCL_LOW_TIME;
    }
    I2CBUS_timeout = timeout;

    return 0;
}
/* 以下的整个持续读写过程为原子操作, 每个函数(除Start())开始前与(除Stop())结束后总是有: SCL,SDA为IO输出模式, SCL为未释放状态(置低) */
/* 对于进一步切割, 使同一条总线上的读写操作可以并行运行的工作, 还有待探究 */
__STATIC_INLINE bool I2CBUS_ClockStetch(void) {    //时钟拉伸判断函数
    if(i2cmodular.errhand == I2CBUS_LEVER0) {      //直接写下一位
        return 0;
    } else if(i2cmodular.errhand == I2CBUS_LEVER1) {    //等待时钟线释放直至超时
        I2CBUS_SCL_Set(IN);
        while(!I2CBUS_SCL_In()) {
            if(I2CBUS_Delayus_paral(I2CBUS_timeout)) {
                I2CBUS_SCL_Set(OUT);
                I2CBUS_SDA_Out(HIGH);    //退出进行错误处理前, 释放数据线
                i2cerror = I2CBUS_TIMEOUT;
                return 1;
            }
        }
        I2CBUS_SCL_Set(OUT);
    }
    return 0;
}
__STATIC_INLINE bool I2CBUS_BusArbitration(void) {    //总线仲裁判断函数
    if(i2cmodular.errhand == I2CBUS_LEVER0) {         //直接写下一位
        return 0;
    } else if(i2cmodular.errhand == I2CBUS_LEVER1) {    //等待数据线释放直至超时
        I2CBUS_SDA_Set(IN);
        while(!I2CBUS_SDA_In()) {
            if(I2CBUS_Delayus_paral(I2CBUS_timeout)) {
                I2CBUS_SDA_Set(OUT);
                I2CBUS_SCL_Out(HIGH);    //退出进行错误处理前, 释放时钟线
                i2cerror = I2CBUS_ARBITRATION;
                return 1;
            }
        }
        I2CBUS_SDA_Set(OUT);
    }
    return 0;
}
static void I2CBUS_Start_(void);
static void I2CBUS_Stop_(void);
static bool I2CBUS_SlaveWaiting(void) {    //从机响应判断函数
    if(i2cmodular.errhand == I2CBUS_LEVER0) {       //直接写下一字节
        return 0;
    } else if(i2cmodular.errhand == I2CBUS_LEVER1) {    //等待从机响应直至超时
        while(I2CBUS_SDA_In()) {
            if(I2CBUS_Delayus_paral(I2CBUS_timeout)) {
                I2CBUS_SDA_Set(OUT);
                /* 可能导致从机未响应的原因:
                1.时钟太快, 从机跟不上未及时下拉数据线, 导致未响应
                2.时钟线被强上拉, 从机察觉不到时序, 间接导致未响应
                3.数据线被强上拉, 从机无法下拉数据线, 直接导致未响应
                尝试使用释放时钟线以结束至错误处理 */
                I2CBUS_SCL_Out(HIGH);    //退出进行错误处理前, 释放时钟线
                i2cerror = I2CBUS_NOACK;
                return 1;
            }
        }
    }
    return 0;
}
static void I2CBUS_Start(void) {
    /* 在时钟线置高时, 数据线上的一个下降沿表示开始传输数据 */
    I2CBUS_SCL_Out(LOW);
    I2CBUS_SDA_Out(HIGH);
    I2CBUS_Delayus(I2CBUS_scl_lowtime);        //时钟线置低期间等待数据线电位稳定
    __NOP();                                   //tofix:将短暂的暂停优化到延时函数内
    __NOP();                                   //
    __NOP();                                   //
    __NOP();                                   //
    __NOP();                                   //
    if(I2CBUS_BusArbitration()) { return; }    //总线仲裁判断, 若发现数据线没有被如期置高, 则进行仲裁处理
    I2CBUS_SCL_Out(HIGH);                      //
    I2CBUS_Delayus(I2CBUS_scl_hightime);       //时钟线拉高后等待电位稳定
    __NOP();                                   //tofix:将短暂的暂停优化到延时函数内
    __NOP();                                   //
    __NOP();                                   //
    __NOP();                                   //
    __NOP();                                   //
    if(I2CBUS_ClockStetch()) { return; }       //时钟拉伸判断, 若发现时钟线没有被如期置高, 则等待直至超时进行超时处理
    I2CBUS_SDA_Out(LOW);                       //拉低数据线, 产生一个下降沿
    I2CBUS_Delayus(I2CBUS_scl_hightime);       //时钟线置高期间等待数据线电位稳定
    __NOP();                                   //tofix:将短暂的暂停优化到延时函数内
    __NOP();                                   //
    __NOP();                                   //
    __NOP();                                   //
    __NOP();                                   //
    I2CBUS_SCL_Out(LOW);                       //拉低时钟线
}
static void I2CBUS_Stop(void) {
    /* 在时钟线置高时, 数据线上的一个上升沿表示停止传输数据 */
    I2CBUS_SDA_Out(LOW);                       //拉低数据线, 进行准备
    I2CBUS_Delayus(I2CBUS_scl_lowtime);        //时钟线置低期间等待数据线电位稳定
    __NOP();                                   //tofix:将短暂的暂停优化到延时函数内
    __NOP();                                   //
    __NOP();                                   //
    __NOP();                                   //
    __NOP();                                   //
    I2CBUS_SCL_Out(HIGH);                      //拉高时钟线
    I2CBUS_Delayus(I2CBUS_scl_hightime);       //时钟线拉高后等待电位稳定
    __NOP();                                   //tofix:将短暂的暂停优化到延时函数内
    __NOP();                                   //
    __NOP();                                   //
    __NOP();                                   //
    __NOP();                                   //
    if(I2CBUS_ClockStetch()) { return; }       //时钟拉伸判断, 若发现时钟线没有被如期置高, 则等待直至超时进行超时处理
    I2CBUS_SDA_Out(HIGH);                      //拉高数据线, 产生一个上升沿
    I2CBUS_Delayus(I2CBUS_scl_hightime);       //时钟线置高期间等待数据线电位稳定
    __NOP();                                   //tofix:将短暂的暂停优化到延时函数内
    __NOP();                                   //
    __NOP();                                   //
    __NOP();                                   //
    __NOP();                                   //
    if(I2CBUS_BusArbitration()) { return; }    //总线仲裁判断, 若发现数据线没有被如期置高, 则进行仲裁处理
}
static void I2CBUS_Start_(void) {
    /* 在时钟线置高时, 数据线上的一个下降沿表示开始传输数据 */
    I2CBUS_SCL_Out(LOW);
    I2CBUS_SDA_Out(HIGH);
    I2CBUS_Delayus(I2CBUS_scl_lowtime);     //时钟线置低期间等待数据线电位稳定
    __NOP();                                //tofix:将短暂的暂停优化到延时函数内
    __NOP();                                //
    __NOP();                                //
    __NOP();                                //
    __NOP();                                //
    I2CBUS_SCL_Out(HIGH);                   //
    I2CBUS_Delayus(I2CBUS_scl_hightime);    //时钟线拉高后等待电位稳定
    __NOP();                                //tofix:将短暂的暂停优化到延时函数内
    __NOP();                                //
    __NOP();                                //
    __NOP();                                //
    __NOP();                                //
    I2CBUS_SDA_Out(LOW);                    //拉低数据线, 产生一个下降沿
    I2CBUS_Delayus(I2CBUS_scl_hightime);    //时钟线置高期间等待数据线电位稳定
    __NOP();                                //tofix:将短暂的暂停优化到延时函数内
    __NOP();                                //
    __NOP();                                //
    __NOP();                                //
    __NOP();                                //
    I2CBUS_SCL_Out(LOW);                    //拉低时钟线
}
static void I2CBUS_Stop_(void) {
    /* 在时钟线置高时, 数据线上的一个上升沿表示停止传输数据 */
    I2CBUS_SDA_Out(LOW);                    //拉低数据线, 进行准备
    I2CBUS_Delayus(I2CBUS_scl_lowtime);     //时钟线置低期间等待数据线电位稳定
    __NOP();                                //tofix:将短暂的暂停优化到延时函数内
    __NOP();                                //
    __NOP();                                //
    __NOP();                                //
    __NOP();                                //
    I2CBUS_SCL_Out(HIGH);                   //拉高时钟线
    I2CBUS_Delayus(I2CBUS_scl_hightime);    //时钟线拉高后等待电位稳定
    __NOP();                                //tofix:将短暂的暂停优化到延时函数内
    __NOP();                                //
    __NOP();                                //
    __NOP();                                //
    __NOP();                                //
    I2CBUS_SDA_Out(HIGH);                   //拉高数据线, 产生一个上升沿
    I2CBUS_Delayus(I2CBUS_scl_hightime);    //时钟线置高期间等待数据线电位稳定
    __NOP();                                //tofix:将短暂的暂停优化到延时函数内
    __NOP();                                //
    __NOP();                                //
    __NOP();                                //
    __NOP();
}
__STATIC_INLINE void I2CBUS_WriteBit(bool bit) {    // 写1位数据
    /* I2C通信建立后, 在时钟线拉低期间, 数据线可进行操作, 主机写数据; 时钟线拉高期间, 数据不可进行操作, 从机读数据 */
    I2CBUS_SDA_Out(bit);                              //写数据至数据线
    I2CBUS_Delayus(I2CBUS_scl_lowtime);               //时钟线置低期间等待数据线电位稳定
    __NOP();                                          //tofix:将短暂的暂停优化到延时函数内
    __NOP();                                          //
    __NOP();                                          //
    __NOP();                                          //
    __NOP();                                          //
    if(bit && I2CBUS_BusArbitration()) { return; }    //总线仲裁判断, 若发现数据线没有被如期置高, 则进行仲裁处理
    I2CBUS_SCL_Out(HIGH);                             //拉高时钟线并保持一段时间, 等待从机读取
    I2CBUS_Delayus(I2CBUS_scl_hightime);              //时钟线拉高后等待电位稳定
    __NOP();                                          //tofix:将短暂的暂停优化到延时函数内
    __NOP();                                          //
    __NOP();                                          //
    __NOP();                                          //
    __NOP();                                          //
    if(I2CBUS_ClockStetch()) { return; }              //时钟拉伸判断, 若发现时钟线没有被如期置高, 则等待直至超时进行超时处理
    I2CBUS_SCL_Out(LOW);                              //拉低时钟线
}
__STATIC_INLINE void I2CBUS_WriteBit_(bool bit) {
    I2CBUS_SDA_Out(bit);
    I2CBUS_Delayus(I2CBUS_scl_lowtime);
    __NOP();    //tofix:将短暂的暂停优化到延时函数内
    __NOP();    //
    __NOP();    //
    __NOP();    //
    __NOP();    //
    I2CBUS_SCL_Out(HIGH);
    I2CBUS_Delayus(I2CBUS_scl_hightime);
    __NOP();    //tofix:将短暂的暂停优化到延时函数内
    __NOP();    //
    __NOP();    //
    __NOP();    //
    __NOP();    //
    I2CBUS_SCL_Out(LOW);
}
__STATIC_INLINE bool I2CBUS_ReadBit(void) {    // 读1位数据
    /* I2C通信建立后, 在时钟线拉低期间, 数据线可进行操作, 从机写数据; 时钟线拉高期间, 数据不可进行操作, 主机读数据 */
    bool bit = 0;
    // I2CBUS_SDA_Out(HIGH);                     //释放数据总线
    // I2CBUS_SDA_Set(IN);                       //数据线设置为读取模式
    I2CBUS_Delayus(I2CBUS_scl_lowtime);          //时钟线置低期间等待数据线电位稳定
    __NOP();                                     //tofix:将短暂的暂停优化到延时函数内
    __NOP();                                     //
    __NOP();                                     //
    __NOP();                                     //
    __NOP();                                     //
    I2CBUS_SCL_Out(HIGH);                        //拉高时钟线并保持一段时间, 待数据稳定后读入
    I2CBUS_Delayus(I2CBUS_scl_hightime);         //时钟线拉高后等待电位稳定
    __NOP();                                     //tofix:将短暂的暂停优化到延时函数内
    __NOP();                                     //
    __NOP();                                     //
    __NOP();                                     //
    __NOP();                                     //
    bit = I2CBUS_SDA_In();                       //自数据线读数据
    if(I2CBUS_ClockStetch()) { return true; }    //时钟拉伸判断, 若发现时钟线没有被如期置高, 则等待直至超时进行超时处理
    I2CBUS_SCL_Out(LOW);                         //拉低时钟线
    // I2CBUS_SDA_Set(OUT);                      //数据线设置为写入模式
    return bit;
}
__STATIC_INLINE bool I2CBUS_ReadBit_(void) {
    bool bit = 0;
    // I2CBUS_SDA_Out(HIGH);
    // I2CBUS_SDA_Set(IN);
    I2CBUS_Delayus(I2CBUS_scl_lowtime);
    __NOP();    //tofix:将短暂的暂停优化到延时函数内
    __NOP();    //
    __NOP();    //
    __NOP();    //
    __NOP();    //
    I2CBUS_SCL_Out(HIGH);
    I2CBUS_Delayus(I2CBUS_scl_hightime);
    __NOP();    //tofix:将短暂的暂停优化到延时函数内
    __NOP();    //
    __NOP();    //
    __NOP();    //
    __NOP();    //
    bit = I2CBUS_SDA_In();
    I2CBUS_SCL_Out(LOW);
    // I2CBUS_SDA_Set(OUT);
    return bit;
}
static bool I2CBUS_Write(uint8_t *pdata, uint16_t size) {    // 连续写数据, 将数据按位拆分后写入
    /* 每写入1字节数据后等待从机应答, 写入完毕, 主机释放(即拉高)时钟线和数据线
     一定时间内若能检测到从机拉低数据线, 说明从机应答正常, 可以继续进行通信; 否则说明从机存在问题, 需要重新建立通信 */
    bool bit = 0;
    for(uint16_t j = 0; j < size; j++) {
        for(uint8_t i = 0x80; i; i >>= 1) {
            I2CBUS_WriteBit(pdata[j] & i ? 1 : 0);
            if(i2cerror) { return 1; }    //i2cerror非0, 说明上层因时钟超时/总线仲裁而返回
        }
        I2CBUS_SDA_Out(HIGH);    //释放数据总线
        I2CBUS_SDA_Set(IN);      //数据线设置为读取模式
        if(I2CBUS_ReadBit()) {
            if((bit = i2cerror)) { break; }                 //i2cerror非0, 说明上层因时钟超时而返回
            if((bit = I2CBUS_SlaveWaiting())) { break; }    //从机响应判断, 若发现数据线没有被从机拉低, 则等待直至超时进行未响应处理
        }
        I2CBUS_SDA_Set(OUT);
    }
    I2CBUS_SDA_Set(OUT);
    return bit;
}
static bool I2CBUS_Write_(uint8_t *pdata, uint16_t size) {
    bool bit = 0;
    for(uint16_t j = 0; j < size; j++) {
        for(uint8_t i = 0x80; i; i >>= 1) {
            I2CBUS_WriteBit_(pdata[j] & i ? 1 : 0);
        }
        I2CBUS_SDA_Out(HIGH);
        I2CBUS_SDA_Set(IN);
        if(I2CBUS_ReadBit()) {
            if((bit = I2CBUS_SlaveWaiting())) { break; }    //从机响应判断, 若发现数据线没有被从机拉低, 则等待直至超时进行未响应处理
        }
        I2CBUS_SDA_Set(OUT);
    }
    I2CBUS_SDA_Set(OUT);
    return bit;
}
static void I2CBUS_Read(uint8_t *pdata, uint16_t size) {    // 连续读数据, 将数据按位读取后合并
    /* 每读取1字节数据后主机进行应答, 读取完毕, 从机会释放(即拉高)数据线
    主机写入1位低电平信号, 表示应答正常, 从机继续进行通信; 主机写入1位高电平信号, 表示不再接收数据, 从机停止通信 */
    for(uint16_t j = 0; j < size; j++) {
        I2CBUS_SDA_Out(HIGH);    //释放数据总线
        I2CBUS_SDA_Set(IN);      //数据线设置为读取模式
        for(uint8_t i = 0x80; i; i >>= 1) {
            pdata[j] |= I2CBUS_ReadBit() ? i : 0x00;
            if(i2cerror) { return; }    //i2cerror非0, 说明上层因时钟超时而返回
        }
        I2CBUS_SDA_Set(OUT);    //数据线设置为写入模式
        if(j + 1 < size) {
            I2CBUS_WriteBit(1);
            if(i2cerror) { return; }    //i2cerror非0, 说明上层因时钟超时/总线仲裁而返回
        } else {
            I2CBUS_WriteBit(0);
        }
    }
}
static void I2CBUS_Read_(uint8_t *pdata, uint16_t size) {
    for(uint16_t j = 0; j < size; j++) {
        I2CBUS_SDA_Out(HIGH);
        I2CBUS_SDA_Set(IN);
        for(uint8_t i = 0x80; i; i >>= 1) {
            pdata[j] |= I2CBUS_ReadBit_() ? i : 0x00;
        }
        I2CBUS_SDA_Set(OUT);
        if(j + 1 < size) {
            I2CBUS_WriteBit_(1);
        } else {
            I2CBUS_WriteBit_(0);
        }
    }
}
////////////////////////////////////////////////////////////////////////////
__attribute__((unused)) static int8_t DEV_I2C_TransmitByte(I2C_ModuleHandleTypeDef *modular, uint8_t address, uint8_t *pbyte, int8_t skip, int8_t rw, uint32_t timeout) {
    /* 单字节读写函数, timeout应答超时,speed速度模式,rw1为读0为写 */
    uint8_t byte;
    I2CBUS_Init(modular, timeout);
    if(i2cbus.clockstretch || i2cbus.arbitration) {
        do {
            I2CBUS_Start();
            if(i2cerror) { break; }
            if(skip) {
                byte = (i2cmodular.addr << 1) | rw;
                if(I2CBUS_Write(&byte, 1)) {    //发送器件地址+读/写命令
                    if(i2cerror == I2CBUS_NOACK) { i2cerror = I2CBUS_NOFOUND; }
                    break;
                }
            } else {
                byte = (i2cmodular.addr << 1) | 0X00;
                if(I2CBUS_Write(&byte, 1)) {    //发送器件地址+写命令
                    if(i2cerror == I2CBUS_NOACK) { i2cerror = I2CBUS_NOFOUND; }
                    break;
                }
                if(I2CBUS_Write(&address, 1)) {    //发送寄存器地址
                    if(i2cerror == I2CBUS_NOACK) { i2cerror = I2CBUS_NOFOUND; }
                    break;
                }
                if(rw) {
                    I2CBUS_Start();
                    if(i2cerror) { break; }
                    byte = (i2cmodular.addr << 1) | 0X01;
                    if(I2CBUS_Write(&byte, 1)) {    //发送器件地址+读命令
                        if(i2cerror == I2CBUS_NOACK) { i2cerror = I2CBUS_NOFOUND; }
                        break;
                    }
                }
            }
            if(rw) {
                I2CBUS_Read(pbyte, 1);    //读一个字节
                if(i2cerror) { break; }
            } else {
                if(I2CBUS_Write(pbyte, 1)) {    //写一个字节
                    if(i2cerror == I2CBUS_NOACK) { i2cerror = I2CBUS_NOANSWER; }
                    break;
                }
            }
            I2CBUS_Stop();
            if(i2cerror) { break; }
            return 0;
        } while(0);
        I2CBUS_Error(i2cerror);
        return 1;
    } else {
        I2CBUS_Start_();
        if(skip) {
            byte = (i2cmodular.addr << 1) | rw;
            if(I2CBUS_Write_(&byte, 1)) {    //发送器件地址+读/写命令
            }
        } else {
            byte = (i2cmodular.addr << 1) | 0X00;
            if(I2CBUS_Write_(&byte, 1)) {    //发送器件地址+写命令
            }
            if(I2CBUS_Write_(&address, 1)) {    //发送寄存器地址
            }
            if(rw) {
                I2CBUS_Start_();
                byte = (i2cmodular.addr << 1) | 0X01;
                if(I2CBUS_Write_(&byte, 1)) {    //发送器件地址+读命令
                }
            }
        }
        if(rw) {
            I2CBUS_Read_(pbyte, 1);    //读一个字节
        } else {
            if(I2CBUS_Write_(pbyte, 1)) {    //写一个字节
            }
        }
        I2CBUS_Stop_();
        return 0;
    }
}
__attribute__((unused)) static int8_t DEV_I2C_Transmit(I2C_ModuleHandleTypeDef *modular, uint8_t address, uint8_t *pdata, uint16_t size, int8_t skip, int8_t rw, uint32_t timeout) {
    /* 多字节读写函数, timeout应答超时,speed速度模式 */
    uint8_t byte;
    I2CBUS_Init(modular, timeout);
    if(i2cbus.clockstretch || i2cbus.arbitration) {
        do {
            I2CBUS_Start();
            if(i2cerror) { break; }
            if(skip) {
                byte = (i2cmodular.addr << 1) | rw;
                if(I2CBUS_Write(&byte, 1)) {    //发送器件地址+读/写命令
                    if(i2cerror == I2CBUS_NOACK) { i2cerror = I2CBUS_NOFOUND; }
                    break;
                }
            } else {
                byte = (i2cmodular.addr << 1) | 0X00;
                if(I2CBUS_Write(&byte, 1)) {    //发送器件地址+写命令
                    if(i2cerror == I2CBUS_NOACK) { i2cerror = I2CBUS_NOFOUND; }
                    break;
                }
                if(I2CBUS_Write(&address, 1)) {    //发送寄存器地址
                    if(i2cerror == I2CBUS_NOACK) { i2cerror = I2CBUS_NOFOUND; }
                    break;
                }
                if(rw) {
                    I2CBUS_Start();
                    if(i2cerror) { break; }
                    byte = (i2cmodular.addr << 1) | 0X01;
                    if(I2CBUS_Write(&byte, 1)) {    //发送器件地址+读命令
                        if(i2cerror == I2CBUS_NOACK) { i2cerror = I2CBUS_NOFOUND; }
                        break;
                    }
                }
            }
            if(rw) {
                I2CBUS_Read(pdata, size);    //连续读取
                if(i2cerror) { break; }
            } else {
                if(I2CBUS_Write(pdata, size)) {    //连续写入
                    if(i2cerror == I2CBUS_NOACK) { i2cerror = I2CBUS_NOANSWER; }
                    break;
                }
            }
            I2CBUS_Stop();
            if(i2cerror) { break; }
            return 0;
        } while(0);
        I2CBUS_Error(i2cerror);
        return 1;
    } else {
        do {
            I2CBUS_Start();
            if(skip) {
                byte = (i2cmodular.addr << 1) | rw;
                if(I2CBUS_Write_(&byte, 1)) {    //发送器件地址+读/写命令
                    i2cerror = I2CBUS_NOFOUND;
                    break;
                }
            } else {
                byte = (i2cmodular.addr << 1) | 0X00;
                if(I2CBUS_Write_(&byte, 1)) {    //发送器件地址+写命令
                    i2cerror = I2CBUS_NOFOUND;
                    break;
                }
                if(I2CBUS_Write_(&address, 1)) {    //发送寄存器地址
                    i2cerror = I2CBUS_NOFOUND;
                    break;
                }
                if(rw) {
                    I2CBUS_Start();
                    byte = (i2cmodular.addr << 1) | 0X01;
                    if(I2CBUS_Write_(&byte, 1)) {    //发送器件地址+读命令
                        i2cerror = I2CBUS_NOFOUND;
                        break;
                    }
                }
            }
            if(rw) {
                I2CBUS_Read_(pdata, size);    //连续读取
            } else {
                if(I2CBUS_Write_(pdata, size)) {    //连续写入
                    i2cerror = I2CBUS_NOFOUND;
                    break;
                }
            }
            I2CBUS_Stop();
            return 0;
        } while(0);
        I2CBUS_Error(i2cerror);
        return 1;
    }
}
#undef I2CBUS_SCL_Set
#undef I2CBUS_SDA_Set
#undef I2CBUS_SCL_Out
#undef I2CBUS_SDA_Out
#undef I2CBUS_SCL_In
#undef I2CBUS_SDA_In
#undef I2CBUS_Error
#undef I2CBUS_Delayus
#undef I2CBUS_Delayms
#undef I2CBUS_Delayus_paral
#endif    // DEVICE_I2C_SOFTWARE_ENABLED

////////////////////////////////////////////////////////////////////////////
#ifdef DEVICE_SPI_SOFTWARE_ENABLED
#if defined(SPIBUS_USEPOINTER)
#define SPIBUS_SCK_Out(pot)      ((SPI_SoftHandleTypeDef *)modular->bushandle)->SCK_Out(pot)
#define SPIBUS_SDI_Out(pot)      ((SPI_SoftHandleTypeDef *)modular->bushandle)->SDI_Out(pot)
#define SPIBUS_SDO_In()          ((SPI_SoftHandleTypeDef *)modular->bushandle)->SDO_In()
#define SPIBUS_CS_Out(pot)       ((SPI_SoftHandleTypeDef *)modular->bushandle)->CS_Out(pot)
#define SPIBUS_ERROR(err)        ((SPI_SoftHandleTypeDef *)modular->bushandle)->error(err)
#define SPIBUS_delayus(us)       ({if(us) {((SPI_SoftHandleTypeDef *)modular->bushandle)->delayus(us-TIMER_DELAY);} })
#define SPIBUS_delayms(ms)       ({if(ms) {((SPI_SoftHandleTypeDef *)modular->bushandle)->delayms(ms);} })
#define SPIBUS_Delayus_paral(us) ((SPI_SoftHandleTypeDef *)modular->bushandle)->delayus_paral(us)
#else
#define SPIBUS_SCK_Out(pot)      DEVCMNI_SCL_SCK_Out(pot)
#define SPIBUS_SDI_Out(pot)      DEVCMNI_SDA_SDI_OWRE_Out(pot)
#define SPIBUS_SDO_In()          DEVCMNI_SDO_In()
#define SPIBUS_CS_Out(pot)       DEVCMNI_CS_Out(pot)
#define SPIBUS_ERROR(err)        DEVCMNI_Error(err)
#define SPIBUS_delayus(us)       ({if(us) {DEVCMNI_Delayus(us-TIMER_DELAY);} })
#define SPIBUS_delayms(ms)       ({if(ms) {DEVCMNI_Delayms(ms);} })
#define SPIBUS_Delayus_paral(us) DEVCMNI_Delayus_paral(us)
#endif    // SPIBUS_USEPOINTER
////////////////////////////////////////////////////////////////////////////
static void SPIBUS_Init(SPI_ModuleHandleTypeDef *modular) {}
/* 以下的整个持续读写过程为原子操作, 每个函数(除Start())开始前与(除Stop())结束后总是有: SCK为未释放状态 */
/* 对于进一步切割, 使同一条总线上的读写操作可以并行运行的工作, 还有待探究 */
static void SPIBUS_Start(SPI_ModuleHandleTypeDef *modular, int8_t skip) {
    if(!skip) {    //如果能设置片选, 才初始化总线电位, 否则可能写入多余数据
        SPIBUS_CS_Out(HIGH);
        SPIBUS_SDI_Out(HIGH);
        SPIBUS_delayus(SPIBUS_SCLK_LOW_TIME);
        SPIBUS_SCK_Out(LOW);
        SPIBUS_CS_Out(LOW);    //拉低片选
    }
}
static void SPIBUS_Stop(SPI_ModuleHandleTypeDef *modular, int8_t skip) {
    if(!skip) {
        SPIBUS_CS_Out(HIGH);    //拉高片选
        SPIBUS_SDI_Out(HIGH);
        SPIBUS_delayus(SPIBUS_SCLK_HIGH_TIME);
        SPIBUS_SCK_Out(HIGH);
        SPIBUS_delayus(SPIBUS_SCLK_HIGH_TIME);
    }
}
__STATIC_INLINE void SPIBUS_WriteBit(SPI_ModuleHandleTypeDef *modular, bool bit) {
    SPIBUS_SDI_Out(bit);
    SPIBUS_delayus(SPIBUS_SCLK_LOW_TIME);
    SPIBUS_SCK_Out(HIGH);
    SPIBUS_delayus(SPIBUS_SCLK_HIGH_TIME);
    SPIBUS_SCK_Out(LOW);
}
__STATIC_INLINE bool SPIBUS_ReadBit(SPI_ModuleHandleTypeDef *modular) {
    bool bit = 0;
    SPIBUS_delayus(SPIBUS_SCLK_LOW_TIME);
    SPIBUS_SCK_Out(HIGH);
    SPIBUS_delayus(SPIBUS_SCLK_HIGH_TIME);
    bit = SPIBUS_SDO_In();
    SPIBUS_SCK_Out(LOW);
    return bit;
}
__STATIC_INLINE bool SPIBUS_TransmitBit(SPI_ModuleHandleTypeDef *modular, bool bit) {
    SPIBUS_SDI_Out(bit);
    SPIBUS_delayus(SPIBUS_SCLK_LOW_TIME);
    SPIBUS_SCK_Out(HIGH);
    SPIBUS_delayus(SPIBUS_SCLK_HIGH_TIME);
    bit = SPIBUS_SDO_In();
    SPIBUS_SCK_Out(LOW);
    return bit;
}
////////////////////////////////////////////////////////////////////////////
//单字节写入函数
__attribute__((unused)) static uint8_t DEV_SPI_TransmitByte(SPI_ModuleHandleTypeDef *modular, uint8_t byte, int8_t skip, uint32_t timeout) {
    SPIBUS_Init(modular);
    SPIBUS_Start(modular, skip);
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
    SPIBUS_Stop(modular, skip);
    return byte;
}
//多字节写入函数
__attribute__((unused)) static void DEV_SPI_Transmit(SPI_ModuleHandleTypeDef *modular, uint8_t *pdata, uint16_t size, int8_t skip, uint32_t timeout) {
    SPIBUS_Init(modular);
    SPIBUS_Start(modular, skip);
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
    SPIBUS_Stop(modular, skip);
}
#undef SPIBUS_SCK_Out
#undef SPIBUS_SDI_Out
#undef SPIBUS_SDO_In
#undef SPIBUS_CS_Out
#undef SPIBUS_ERROR
#undef SPIBUS_delayus
#undef SPIBUS_delayms
#undef SPIBUS_Delayus_paral
#endif    // DEVICE_SPI_SOFTWARE_ENABLED

////////////////////////////////////////////////////////////////////////////
#ifdef DEVICE_ONEWIRE_SOFTWARE_ENABLED
#if defined(OWREBUS_USEPOINTER)
#define OWREBUS_OWIO_Set(dir)     ((ONEWIRE_SoftHandleTypeDef *)modular->bushandle)->OWIO_Set(dir)
#define OWREBUS_OWIO_Out(pot)     ((ONEWIRE_SoftHandleTypeDef *)modular->bushandle)->OWIO_Out(pot)
#define OWREBUS_OWIO_In()         ((ONEWIRE_SoftHandleTypeDef *)modular->bushandle)->OWIO_In()
#define OWREBUS_ERROR(err)        ((ONEWIRE_SoftHandleTypeDef *)modular->bushandle)->error(err)
#define OWREBUS_delayus(us)       ({if(us) {((ONEWIRE_SoftHandleTypeDef *)modular->bushandle)->delayus(us);} })
#define OWREBUS_delayms(ms)       ({if(ms) {((ONEWIRE_SoftHandleTypeDef *)modular->bushandle)->delayms(ms);} })
#define OWREBUS_Delayus_paral(us) ((ONEWIRE_SoftHandleTypeDef *)modular->bushandle)->delayus_paral(us)
#else
#define OWREBUS_OWIO_Set(pot)     DEVCMNI_SDA_OWRE_Set(pot)
#define OWREBUS_OWIO_Out(pot)     DEVCMNI_SDA_SDI_OWRE_Out(pot)
#define OWREBUS_OWIO_In(pot)      DEVCMNI_SDA_OWRE_In(pot)
#define OWREBUS_ERROR(err)        DEVCMNI_Error(err)
#define OWREBUS_delayus(us)       ({if(us) {DEVCMNI_Delayus(us);} })
#define OWREBUS_delayms(ms)       ({if(ms) {DEVCMNI_Delayms(ms);} })
#define OWREBUS_Delayus_paral(us) DEVCMNI_Delayus_paral(us)
#endif    // OWREBUS_USEPOINTER
__STATIC_INLINE int8_t OWREBUS_Init(ONEWIRE_ModuleHandleTypeDef *modular) {
    OWREBUS_OWIO_Out(HIGH);
    OWREBUS_OWIO_Set(IN);
    if(OWREBUS_OWIO_In() != HIGH) {
        return 1;    //若总线没有被释放, 返回错误值
    }
    OWREBUS_OWIO_Set(OUT);
    return 0;
}
__STATIC_INLINE uint8_t OWREBUS_Reset(ONEWIRE_ModuleHandleTypeDef *modular) {
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
__STATIC_INLINE void OWREBUS_WriteBit(ONEWIRE_ModuleHandleTypeDef *modular, bool bit) {
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
__STATIC_INLINE uint8_t OWREBUS_ReadBit(ONEWIRE_ModuleHandleTypeDef *modular) {
    bool bit = 0;
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
__STATIC_INLINE void OWREBUS_WriteByte(ONEWIRE_ModuleHandleTypeDef *modular, uint8_t byte) {
    for(uint8_t i = 0x01; i; i <<= 1) {
        OWREBUS_WriteBit(modular, byte & i ? 1 : 0);
    }
}
__STATIC_INLINE uint8_t OWREBUS_ReadByte(ONEWIRE_ModuleHandleTypeDef *modular) {
    uint8_t byte = 0;
    for(uint8_t i = 0x01; i; i <<= 1) {
        byte |= OWREBUS_ReadBit(modular) ? i : 0x00;
    }
    return byte;
}
__STATIC_INLINE void OWREBUS_Write(ONEWIRE_ModuleHandleTypeDef *modular, uint8_t *pdata, uint16_t size) {
    for(uint16_t j = 0; j < size; j++) {
        for(uint8_t i = 0x01; i; i <<= 1) {
            OWREBUS_WriteBit(modular, pdata[j] & i ? 1 : 0);
        }
    }
    return;
}
__STATIC_INLINE void OWREBUS_Read(ONEWIRE_ModuleHandleTypeDef *modular, uint8_t *pdata, uint16_t size) {
    for(uint16_t j = 0; j < size; j++) {
        for(uint8_t i = 0x01; i != 0; i <<= 1) {
            pdata[j] |= OWREBUS_ReadBit(modular) ? i : 0x00;
        }
    }
    return;
}
////////////////////////////////////////////////////////////////////////////
#define _SKIP        0xCC    //ROM跳过匹配指令[限单个设备/接多个设备能同时进行的操作]
#define _MATCH       0x55    //ROM匹配指令
#define _QUERY       0x33    //ROM读取指令[限单个设备]
#define _SEARCH      0xF0    //ROM搜索指令
#define _ALARMSEARCH 0xEC    //ROM报警搜索指令
/**
 * @description: 单总线协议ROM搜索/报警搜索操作
 * 在发起一个复位信号后, 根据总线设备数量, 发出ROM读取/搜索指令, 或发出ROM报警搜索指令
 * @param {ONEWIRE_ModuleHandleTypeDef} *modular
 * @param {int8_t} searchtype
 * @return {*}
 */
__attribute__((unused)) static void DEV_ONEWIRE_Search(ONEWIRE_ModuleHandleTypeDef *modular, int8_t searchtype) {
    OWREBUS_Init(modular);
    OWREBUS_Reset(modular);
    if(searchtype == 0) {
        if(((ONEWIRE_SoftHandleTypeDef *)modular->bushandle)->num == 1) {
            OWREBUS_WriteByte(modular, _QUERY);
        } else if(((ONEWIRE_SoftHandleTypeDef *)modular->bushandle)->num > 1) {
            OWREBUS_WriteByte(modular, _SEARCH);
            //...
        }
    } else {
        OWREBUS_WriteByte(modular, _ALARMSEARCH);
        //...
    }
}
__attribute__((unused)) static void DEV_ONEWIRE_WriteByte(ONEWIRE_ModuleHandleTypeDef *modular, uint8_t byte, int8_t skip, uint32_t timeout) {
    OWREBUS_Init(modular);
    OWREBUS_Reset(modular);
    if(skip || ((ONEWIRE_SoftHandleTypeDef *)modular->bushandle)->num <= 1) {
        OWREBUS_WriteByte(modular, _SKIP);
    } else if(((ONEWIRE_SoftHandleTypeDef *)modular->bushandle)->num > 1) {
        OWREBUS_WriteByte(modular, _MATCH);
        OWREBUS_Write(modular, (uint8_t *)&modular->rom, 8);
    }
    OWREBUS_WriteByte(modular, byte);
}
__attribute__((unused)) static void DEV_ONEWIRE_WriteWord(ONEWIRE_ModuleHandleTypeDef *modular, uint16_t byte, int8_t skip, uint32_t timeout) {
    OWREBUS_Init(modular);
    OWREBUS_Reset(modular);
    if(skip || ((ONEWIRE_SoftHandleTypeDef *)modular->bushandle)->num == 1) {
        OWREBUS_WriteByte(modular, _SKIP);
    } else if(((ONEWIRE_SoftHandleTypeDef *)modular->bushandle)->num > 1) {
        OWREBUS_WriteByte(modular, _MATCH);
        OWREBUS_Write(modular, (uint8_t *)&modular->rom, 8);
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
        OWREBUS_WriteByte(modular, _SKIP);
    } else if(((ONEWIRE_SoftHandleTypeDef *)modular->bushandle)->num > 1) {
        OWREBUS_WriteByte(modular, _MATCH);
        OWREBUS_Write(modular, (uint8_t *)&modular->rom, 8);
    }
    OWREBUS_Write(modular, pdata, size);
    return;
}
__attribute__((unused)) static void DEV_ONEWIRE_Read(ONEWIRE_ModuleHandleTypeDef *modular, uint8_t *pdata, uint16_t size) {
    //tofix: 单总线的读时隙只能跟随在特定的主机写指令之后吗? 当需要并发地与多个设备进行通信时, 怎样进行独立的读操作?
    OWREBUS_Read(modular, pdata, size);
    return;
}
#undef OWREBUS_OWIO_Set
#undef OWREBUS_OWIO_Out
#undef OWREBUS_OWIO_In
#undef OWREBUS_ERROR
#undef OWREBUS_delayus
#undef OWREBUS_delayms
#undef OWREBUS_Delayus_paral
#endif    // DEVICE_ONEWIRE_SOFTWARE_ENABLED

#endif    // !__PROTOCOL_H
