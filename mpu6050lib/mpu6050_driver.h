#ifndef __MPU6050_DRIVER_H
#define __MPU6050_DRIVER_H
#include "mpu6050lib.h"
//////////////////////////////////////////////////////////////////////////////////     
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK MiniSTM32F103开发板 
//MPU6050 驱动代码       
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/4/18
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved                                      
////////////////////////////////////////////////////////////////////////////////// 
#define MPU6050_SCL_CLK RCC_APB2Periph_GPIOA
#define MPU6050_SCL_GPIO GPIOA
#define MPU6050_SCL_PIN GPIO_Pin_15
#define MPU6050_SDA_CLK RCC_APB2Periph_GPIOB
#define MPU6050_SDA_GPIO GPIOB
#define MPU6050_SDA_PIN GPIO_Pin_5
#define MPU6050_INT_CLK RCC_APB2Periph_GPIOB
#define MPU6050_INT_GPIO GPIOB
#define MPU6050_INT_PIN GPIO_Pin_9
#define MPU6050_INT_EXTILINE EXTI_Line9
#define MPU6050_INT_EXTISTATE EXTI9_5_IRQn
#define MPU6050_INT_EXTIGPIO GPIO_PortSourceGPIOB
#define MPU6050_INT_EXTIPIN GPIO_PinSource9
//ADO脚(9脚)接VCC/GND时, I2C地址为0X69/0X68(不包含最低位)
#define MPU6050_I2CADDR1 0X68
#define MPU6050_I2CADDR2 0X69
uint8_t MPU_Write_Len(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf);//I2C连续写
uint8_t MPU_Read_Len(uint8_t addr,uint8_t reg,uint8_t len,uint8_t *buf); //I2C连续读 
uint8_t MPU_Write_Byte(uint8_t reg,uint8_t data);                //I2C写一个字节
uint8_t MPU_Read_Byte(uint8_t reg);                        //I2C读一个字节


#ifndef MCU_COMPILER
#define MCU_STM32FWLIB 0
#define MCU_STM32HAL 1
#define MCU_TIBOARD 2
#define MCU_ESP32 3
#define MCU_HC32 4
#define MCU_COMPILER    MCU_STM32HAL        //主控芯片编译环境
#endif
#define MPU6050_NUM 1
typedef enum {
    MPU6050_I2C, MPU6050_ONEWIRE
}Mpu6050Prop_Type;
typedef enum {
    MPU6050_ANALOG, MPU6050_HARDWARE
}Mpu6050Comi_Type;
typedef struct {
    #if (MCU_COMPILER == MCU_STM32FWLIB)    //固件库IO结构
    uint32_t CLK_scl_sclk;
    GPIO_TypeDef *GPIO_scl_sclk;
    uint16_t PIN_scl_sclk;
    uint32_t CLK_sda_sdo;
    GPIO_TypeDef *GPIO_sda_sdo;
    uint16_t PIN_sda_sdo;
    uint32_t CLK_rst;
    GPIO_TypeDef *GPIO_rst;
    uint16_t PIN_rst;
    uint32_t CLK_dc;
    GPIO_TypeDef *GPIO_dc;
    uint16_t PIN_dc;
    uint32_t CLK_cs;
    GPIO_TypeDef *GPIO_cs;
    uint16_t PIN_cs;
    #elif (MCU_COMPILER == MCU_STM32HAL)    //HAL库IO结构
    GPIO_TypeDef *GPIO_scl_sclk;
    uint16_t PIN_scl_sclk;
    GPIO_TypeDef *GPIO_sda_sdo;
    uint16_t PIN_sda_sdo;
    GPIO_TypeDef *GPIO_rst;
    uint16_t PIN_rst;
    GPIO_TypeDef *GPIO_dc;
    uint16_t PIN_dc;
    GPIO_TypeDef *GPIO_cs;
    uint16_t PIN_cs;
    #endif
}Mpu6050io_Typedef;
typedef struct {
    Mpu6050Prop_Type mpu6050prop;
    Mpu6050Comi_Type mpu6050comi;
    uint8_t mpu6050addr;
    void *communication_handle; //通信句柄
    Mpu6050io_Typedef *mpu6050io;
}Mpu6050_Typedef;

void MPU6050_readConti(uint8_t *pdata, uint16_t size, uint8_t address);
uint8_t MPU6050_readByte(uint8_t address);
void MPU6050_writeConti(uint8_t *pdata, uint16_t size, uint8_t address);
void MPU6050_writeByte(uint8_t data, uint8_t address);



//#define MPU_ACCEL_OFFS_REG    0X06        //accel_offs寄存器,可读取版本号,寄存器手册未提到
//#define MPU_PROD_ID_REG       0X0C        //prod id寄存器,在寄存器手册未提到
#define MPU_SELF_TESTX_REG      0X0D        //自检寄存器X
#define MPU_SELF_TESTY_REG      0X0E        //自检寄存器Y
#define MPU_SELF_TESTZ_REG      0X0F        //自检寄存器Z
#define MPU_SELF_TESTA_REG      0X10        //自检寄存器A
#define MPU_SAMPLE_RATE_REG     0X19        //采样频率分频器
#define MPU_CFG_REG             0X1A        //配置寄存器
#define MPU_GYRO_CFG_REG        0X1B        //陀螺仪配置寄存器
#define MPU_ACCEL_CFG_REG       0X1C        //加速度计配置寄存器
#define MPU_MOTION_DET_REG      0X1F        //运动检测阀值设置寄存器
#define MPU_FIFO_EN_REG         0X23        //FIFO使能寄存器
#define MPU_I2CMST_CTRL_REG     0X24        //I2C主机控制寄存器
#define MPU_I2CSLV0_ADDR_REG    0X25        //I2C从机0器件地址寄存器
#define MPU_I2CSLV0_REG         0X26        //I2C从机0数据地址寄存器
#define MPU_I2CSLV0_CTRL_REG    0X27        //I2C从机0控制寄存器
#define MPU_I2CSLV1_ADDR_REG    0X28        //I2C从机1器件地址寄存器
#define MPU_I2CSLV1_REG         0X29        //I2C从机1数据地址寄存器
#define MPU_I2CSLV1_CTRL_REG    0X2A        //I2C从机1控制寄存器
#define MPU_I2CSLV2_ADDR_REG    0X2B        //I2C从机2器件地址寄存器
#define MPU_I2CSLV2_REG         0X2C        //I2C从机2数据地址寄存器
#define MPU_I2CSLV2_CTRL_REG    0X2D        //I2C从机2控制寄存器
#define MPU_I2CSLV3_ADDR_REG    0X2E        //I2C从机3器件地址寄存器
#define MPU_I2CSLV3_REG         0X2F        //I2C从机3数据地址寄存器
#define MPU_I2CSLV3_CTRL_REG    0X30        //I2C从机3控制寄存器
#define MPU_I2CSLV4_ADDR_REG    0X31        //I2C从机4器件地址寄存器
#define MPU_I2CSLV4_REG         0X32        //I2C从机4数据地址寄存器
#define MPU_I2CSLV4_DO_REG      0X33        //I2C从机4写数据寄存器
#define MPU_I2CSLV4_CTRL_REG    0X34        //I2C从机4控制寄存器
#define MPU_I2CSLV4_DI_REG      0X35        //I2C从机4读数据寄存器

#define MPU_I2CMST_STA_REG      0X36        //I2C主机状态寄存器
#define MPU_INTBP_CFG_REG       0X37        //中断/旁路设置寄存器
#define MPU_INT_EN_REG          0X38        //中断使能寄存器
#define MPU_INT_STA_REG         0X3A        //中断状态寄存器

#define MPU_ACCEL_XOUTH_REG     0X3B        //加速度值,X轴高8位寄存器
#define MPU_ACCEL_XOUTL_REG     0X3C        //加速度值,X轴低8位寄存器
#define MPU_ACCEL_YOUTH_REG     0X3D        //加速度值,Y轴高8位寄存器
#define MPU_ACCEL_YOUTL_REG     0X3E        //加速度值,Y轴低8位寄存器
#define MPU_ACCEL_ZOUTH_REG     0X3F        //加速度值,Z轴高8位寄存器
#define MPU_ACCEL_ZOUTL_REG     0X40        //加速度值,Z轴低8位寄存器

#define MPU_TEMP_OUTH_REG       0X41        //温度值高八位寄存器
#define MPU_TEMP_OUTL_REG       0X42        //温度值低8位寄存器

#define MPU_GYRO_XOUTH_REG      0X43        //陀螺仪值,X轴高8位寄存器
#define MPU_GYRO_XOUTL_REG      0X44        //陀螺仪值,X轴低8位寄存器
#define MPU_GYRO_YOUTH_REG      0X45        //陀螺仪值,Y轴高8位寄存器
#define MPU_GYRO_YOUTL_REG      0X46        //陀螺仪值,Y轴低8位寄存器
#define MPU_GYRO_ZOUTH_REG      0X47        //陀螺仪值,Z轴高8位寄存器
#define MPU_GYRO_ZOUTL_REG      0X48        //陀螺仪值,Z轴低8位寄存器

#define MPU_I2CSLV0_DO_REG      0X63        //I2C从机0数据寄存器
#define MPU_I2CSLV1_DO_REG      0X64        //I2C从机1数据寄存器
#define MPU_I2CSLV2_DO_REG      0X65        //I2C从机2数据寄存器
#define MPU_I2CSLV3_DO_REG      0X66        //I2C从机3数据寄存器

#define MPU_I2CMST_DELAY_REG    0X67        //I2C主机延时管理寄存器
#define MPU_SIGPATH_RST_REG     0X68        //信号通道复位寄存器
#define MPU_MDETECT_CTRL_REG    0X69        //运动检测控制寄存器
#define MPU_USER_CTRL_REG       0X6A        //用户控制寄存器
#define MPU_PWR_MGMT1_REG       0X6B        //电源管理寄存器1
#define MPU_PWR_MGMT2_REG       0X6C        //电源管理寄存器2 
#define MPU_FIFO_CNTH_REG       0X72        //FIFO计数寄存器高八位
#define MPU_FIFO_CNTL_REG       0X73        //FIFO计数寄存器低八位
#define MPU_FIFO_RW_REG         0X74        //FIFO读写寄存器
#define MPU_DEVICE_ID_REG       0X75        //器件ID寄存器

extern float pitch,roll,yaw;         //欧拉角
extern short aacx,aacy,aacz;        //加速度传感器原始数据
extern short gyrox,gyroy,gyroz;    //陀螺仪原始数据
extern short temperature;                //温度
extern uint8_t flag_mpu6050;

void MPU6050_Read(void);
void MPU6050_Confi(void);

uint8_t MPU_Set_Gyro_Fsr(uint8_t fsr);
uint8_t MPU_Set_Accel_Fsr(uint8_t fsr);
uint8_t MPU_Set_LPF(uint16_t lpf);
uint8_t MPU_Set_Rate(uint16_t rate);
uint8_t MPU_Set_Fifo(uint8_t sens);
short MPU_Get_Temperature(void);
uint8_t MPU_Get_Gyroscope(short *gx,short *gy,short *gz);
uint8_t MPU_Get_Accelerometer(short *ax,short *ay,short *az);

#endif
