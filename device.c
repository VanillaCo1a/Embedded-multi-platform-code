#include "device.h"
#include "timer.h"

/////////////////////////    设备控制部分    /////////////////////////
devpool_size _devsize = 0;
DEVS_TypeDef *_devsstream = NULL;
DEV_TypeDef devpool[DEVPOOL_MAXNUM];
uint64_t devbusylist[DEVBUSYLIST_MAXNUM];    //忙设备表, 0为空, 非0为设备置忙时刻(us)
void DEVBUSYLIST_Init(void) {
    for(busypool_size i = 0; i < DEVBUSYLIST_MAXNUM; i++) {
        devbusylist[i] = 0;
    }
}
/**
 * @description: 初始化某类设备, 从设备池中分配空闲空间, 输入设备类结构体,输出初始化结果
 * @param {DEVS_TypeDef} *devs
 * @param {void} *parameter
 * @param {void} *devio
 * @param {DEVCMNI_TypeDef} *cmni
 * @return {*}
 */
int8_t DEV_Init(DEVS_TypeDef *devs, void *parameter, void *io, DEVCMNI_TypeDef *cmni) {
    if(_devsize + devs->size >= DEVPOOL_MAXNUM) {
        return 1;    //申请失败
    }
    devs->pool = _devsize;
    _devsize += devs->size;
    for(devpool_size i = 0; i < devs->size; i++) {
        //tofix: 空指针无法进行加减
        devpool[devs->pool + i].parameter = parameter + i;
        devpool[devs->pool + i].io = io + i;
        devpool[devs->pool + i].cmni = cmni;
        devpool[devs->pool + i].state = 0;
    }
    return 0;
}
/**
 * @description: DEV_Error
 * @param {*}
 * @return {*}
 */
void DEV_Error(void) {
    while(1)
        ;
}
/**
 * @description: 设置活动类设备, 并设置活动设备序号(0~最大值-1)
 * @param {DEVS_TypeDef} *self
 * @param {devpool_size} stream
 * @return {*}
 */
int8_t DEV_setistm(DEVS_TypeDef *self, devpool_size stream) {
    if(self == NULL || stream > _devsize || stream > self->size) {
        return 1;
    }
    self->stream = stream;
    _devsstream = self;
    return 0;
}
/**
 * @description: 获取某类设备的活动设备序号
 * @param {DEVS_TypeDef} *self
 * @return {*}
 */
devpool_size DEV_getistm(DEVS_TypeDef *self) {
    if(self == NULL) {
        return -1;
    }
    return self->stream;
}
/**
 * @description: 获取某类设备的活动设备的配置
 * @param {DEVS_TypeDef} *self
 * @return {*}
 */
DEV_TypeDef *DEV_getdev(DEVS_TypeDef *self) {
    if(self == NULL) {
        return NULL;
    }
    _devsstream = self;
    return &devpool[self->pool + self->stream];
}
/**
 * @description: 对某类设备批量进行某一操作
 * @param {DEVS_TypeDef} *devs
 * @param {void} *action
 * @return {*}
 */
void DEV_Do(DEVS_TypeDef *devs, void (*action)(void)) {
    for(devpool_size i = 0; i < devs->size; i++) {
        if(DEV_setistm(devs, i) == 1) {
            DEV_Error();
        }
        action();
    }
}
/**
 * @description: 设置某类设备的活动设备状态, us>0设置设备忙时长(单位为10us),us=0设置设备闲
 * @param {DEVS_TypeDef} *self
 * @param {uint16_t} tus
 * @return {*}
 */
int8_t DEV_setState(DEVS_TypeDef *self, uint16_t tus) {
    if(tus > 0 && devpool[self->pool + self->stream].state == 0) {
        for(busypool_size i = 0; i < DEVBUSYLIST_MAXNUM; i++) {
            if(devbusylist[i] == 0) {
                devpool[self->pool + self->stream].state = i + 1;    //设置成功, 保存当前设备忙时间记录在忙设备列表的第几个
                devbusylist[i] = TIMER_getRunTimeus() + tus * 10;
                break;
            }
        }
        if(devpool[self->pool + self->stream].state == 0) {
            return 1;    //设备状态表已满, 设置设备忙失败
        }
    } else if(tus > 0 && devpool[self->pool + self->stream].state > 0) {
        devbusylist[devpool[self->pool + self->stream].state - 1] = TIMER_getRunTimeus() + tus * 10;
    } else if(tus == 0 && devpool[self->pool + self->stream].state > 0) {
        devbusylist[devpool[self->pool + self->stream].state - 1] = 0;
        devpool[self->pool + self->stream].state = 0;
    }
    return 0;
}
/**
 * @description: 获取某类设备的活动设备状态, 先查询忙设备是否满足空闲条件, 然后更新状态并返回
 * @param {DEVS_TypeDef} *self
 * @return {*}
 */
DEV_StateTypeDef DEV_getState(DEVS_TypeDef *self) {
    if(devpool[self->pool + self->stream].state > 0 && TIMER_getRunTimeus() > devbusylist[devpool[self->pool + self->stream].state - 1]) {
        devbusylist[devpool[self->pool + self->stream].state - 1] = 0;    //设备忙指定时间后转为空闲状态
        devpool[self->pool + self->stream].state = 0;
    }
    //(devpool[self->pool+self->stream].state == 0) ? idle : busy;
    return (DEV_StateTypeDef)(devpool[self->pool + self->stream].state != idle);
}


/////////////////////////    IO操作部分    /////////////////////////
#if MCU_COMPILER == MCU_STM32HAL
#if DEV_USEMACRO == 0
void DEVIO_WritePin_SET(DEVIO_TypeDef *DEVIO) {
    if(DEVIO == NULL) {
        DEV_Error();
    }
    HAL_GPIO_WritePin((DEVIO)->GPIOx, (DEVIO)->GPIO_Pin, GPIO_PIN_SET);
}
void DEVIO_WritePin_RESET(DEVIO_TypeDef *DEVIO) {
    if(DEVIO == NULL) {
        DEV_Error();
    }
    HAL_GPIO_WritePin((DEVIO)->GPIOx, (DEVIO)->GPIO_Pin, GPIO_PIN_RESET);
}
DEVIO_PinState DEVIO_ReadPin(DEVIO_TypeDef *DEVIO) {
    if(DEVIO == NULL) {
        DEV_Error();
    }
    return (DEVIO_PinState)HAL_GPIO_ReadPin((DEVIO)->GPIOx, (DEVIO)->GPIO_Pin);
}
#endif
#elif MCU_COMPILER == MCU_STM32FWLIBF1
#if DEV_USEMACRO == 0
void DEVIO_WritePin(DEVIO_TypeDef *DEVIO, DEVIO_PinState PinState) {
    if(DEVIO == NULL) {
        DEV_Error();
    }
    if(PinState == DEVIO_PIN_SET) {
        GPIO_SetBits((DEVIO)->GPIOx, (DEVIO)->GPIO_Pin);
    } else {
        GPIO_ResetBits((DEVIO)->GPIOx, (DEVIO)->GPIO_Pin);
    }
}
void DEVIO_WritePin_SET(DEVIO_TypeDef *DEVIO) {
    if(DEVIO == NULL) {
        DEV_Error();
    }
    GPIO_SetBits((DEVIO)->GPIOx, (DEVIO)->GPIO_Pin);
}
void DEVIO_WritePin_RESET(DEVIO_TypeDef *DEVIO) {
    if(DEVIO == NULL) {
        DEV_Error();
    }
    GPIO_ResetBits((DEVIO)->GPIOx, (DEVIO)->GPIO_Pin);
}
DEVIO_PinState DEVIO_ReadPin(DEVIO_TypeDef *DEVIO) {
    if(DEVIO == NULL) {
        DEV_Error();
    }
    return (DEVIO_PinState)GPIO_ReadInputDataBit((DEVIO)->GPIOx, (DEVIO)->GPIO_Pin);
}
void DEV_CLK_GPIO_PIN_CONFI(DEVIO_TypeDef *DEVIO) {
    if(DEVIO == NULL) {
        DEV_Error();
    }
    RCC_APB2PeriphClockCmd((DEVIO)->CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin = (DEVIO)->GPIO_Pin;
    GPIO_Init((DEVIO)->GPIOx, &GPIO_InitStructure);
}
#endif
#endif


/////////////////////////    通信协议实现部分    /////////////////////////
//    I2C/SPI模拟通信的IO操作绑定
void DEVCMNI_SCL_Set(Direct_TypeDef dir) {}
void DEVCMNI_SDA_Set(Direct_TypeDef dir) {}
void DEVCMNI_SCL_Out(Potential_TypeDef bit) {
    if(bit == LOW)
        DEVIO_WritePin_RESET(&((DEVCMNI_IOTypeDef *)DEV_getdev(_devsstream)->io)->SCL_SCLK);
    else
        DEVIO_WritePin_SET(&((DEVCMNI_IOTypeDef *)DEV_getdev(_devsstream)->io)->SCL_SCLK);
}
void DEVCMNI_SDA_Out(Potential_TypeDef bit) {
    if(bit == LOW)
        DEVIO_WritePin_RESET(&((DEVCMNI_IOTypeDef *)DEV_getdev(_devsstream)->io)->SDA_SDO_OWIO);
    else
        DEVIO_WritePin_SET(&((DEVCMNI_IOTypeDef *)DEV_getdev(_devsstream)->io)->SDA_SDO_OWIO);
}
Potential_TypeDef DEVCMNI_SDA_In(void) {
    DEVIO_PinState bit = DEVIO_ReadPin(&((DEVCMNI_IOTypeDef *)DEV_getdev(_devsstream)->io)->SDA_SDO_OWIO);
    if(bit == DEVIO_PIN_RESET)
        return LOW;
    else
        return HIGH;
}
void DEVCMNI_SCLK_Out(Potential_TypeDef bit) {
    if(bit == LOW)
        DEVIO_WritePin_RESET(&((DEVCMNI_IOTypeDef *)DEV_getdev(_devsstream)->io)->SCL_SCLK);
    else
        DEVIO_WritePin_SET(&((DEVCMNI_IOTypeDef *)DEV_getdev(_devsstream)->io)->SCL_SCLK);
}
void DEVCMNI_SDO_Out(Potential_TypeDef bit) {
    if(bit == LOW)
        DEVIO_WritePin_RESET(&((DEVCMNI_IOTypeDef *)DEV_getdev(_devsstream)->io)->SDA_SDO_OWIO);
    else
        DEVIO_WritePin_SET(&((DEVCMNI_IOTypeDef *)DEV_getdev(_devsstream)->io)->SDA_SDO_OWIO);
}
void DEVCMNI_CS_Out(Potential_TypeDef bit) {
    if(((DEVCMNI_IOTypeDef *)DEV_getdev(_devsstream)->io)->CS.GPIOx != NULL) {
        if(bit == LOW)
            DEVIO_WritePin_RESET(&((DEVCMNI_IOTypeDef *)DEV_getdev(_devsstream)->io)->CS);
        else
            DEVIO_WritePin_SET(&((DEVCMNI_IOTypeDef *)DEV_getdev(_devsstream)->io)->CS);
    }
}
void DEVCMNI_OWIO_Set(Direct_TypeDef dir) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = ((DEVCMNI_IOTypeDef *)DEV_getdev(_devsstream)->io)->SDA_SDO_OWIO.GPIO_Pin;
    if(dir == OUT) {
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    } else {
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    }
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(((DEVCMNI_IOTypeDef *)DEV_getdev(_devsstream)->io)->SDA_SDO_OWIO.GPIOx, &GPIO_InitStruct);
}
void DEVCMNI_OWIO_Out(Potential_TypeDef bit) {
    if(bit == LOW)
        DEVIO_WritePin_RESET(&((DEVCMNI_IOTypeDef *)DEV_getdev(_devsstream)->io)->SDA_SDO_OWIO);
    else
        DEVIO_WritePin_SET(&((DEVCMNI_IOTypeDef *)DEV_getdev(_devsstream)->io)->SDA_SDO_OWIO);
}
Potential_TypeDef DEVCMNI_OWIO_In(void) {
    DEVIO_PinState bit = DEVIO_ReadPin(&((DEVCMNI_IOTypeDef *)DEV_getdev(_devsstream)->io)->SDA_SDO_OWIO);
    if(bit == DEVIO_PIN_RESET)
        return LOW;
    else
        return HIGH;
}
void DEVCMNI_Delayus(uint16_t us) {
#if MCU_COMPILER == MCU_STM32HAL
    delayus_timer(us);
#elif MCU_COMPILER == MCU_STM32FWLIBF1
    delayus_timer(us);
#endif
}
void DEVCMNI_Delayms(uint16_t ms) {
#if MCU_COMPILER == MCU_STM32HAL
#ifdef __TIMER_H
    delayms_timer(ms);
#else
    HAL_Delay(ms);
#endif
#elif MCU_COMPILER == MCU_STM32FWLIBF1
    delayms_timer(ms);
#endif
}
//模拟通信基于IO操作的实例
I2C_AnalogTypeDef ai2c;
SPI_AnalogTypeDef aspi;
OWRE_AnalogTypedef aowre;
void DEVCMNI_Init(void) {
    ai2c.SCL_Set = DEVCMNI_SCL_Set;
    ai2c.SDA_Set = DEVCMNI_SDA_Set;
    ai2c.SCL_Out = DEVCMNI_SCL_Out;
    ai2c.SDA_Out = DEVCMNI_SDA_Out;
    ai2c.SDA_In = DEVCMNI_SDA_In;
    ai2c.delayus = DEVCMNI_Delayus;
    ai2c.delayms = DEVCMNI_Delayms;
    aspi.SCLK_Out = DEVCMNI_SCLK_Out;
    aspi.SDO_Out = DEVCMNI_SDO_Out;
    aspi.CS_Out = DEVCMNI_CS_Out;
    aspi.delayus = DEVCMNI_Delayus;
    aspi.delayms = DEVCMNI_Delayms;
    aowre.OWIO_Set = DEVCMNI_OWIO_Set;
    aowre.OWIO_Out = DEVCMNI_OWIO_Out;
    aowre.OWIO_In = DEVCMNI_OWIO_In;
    aowre.delayus = DEVCMNI_Delayus;
    aowre.delayms = DEVCMNI_Delayms;
}
//    I2C/SPI/ONEWIRE通信IO初始化配置
void DEVCMNI_IOInit(void) {
    DEV_TypeDef *dev = DEV_getdev(_devsstream);
    DEVCMNI_IOTypeDef *devio = (DEVCMNI_IOTypeDef *)dev->io;
    if(dev->cmni == NULL || dev->io == NULL) {    //若设备没有通信配置, 则直接返回
        return;
    }
    if(dev->cmni->type == 0 || dev->cmni->dev == 0) {    //若设备通信配置未配置正确, 则报错
        DEV_Error();
    }
    if(dev->cmni->handle.hai2c.method == NULL && dev->cmni->handle.haspi.method == NULL && dev->cmni->handle.hi2c == NULL && dev->cmni->handle.hspi == NULL && dev->cmni->handle.howre == NULL && dev->cmni->handle.haowre.method == NULL) {
        DEV_Error();
    }
    //不同设备实际使用的通信引脚不同, 未定义的引脚不会被初始化
#if MCU_COMPILER == MCU_STM32HAL
    //HAL库的IO初始化在main函数中完成
#elif MCU_COMPILER == MCU_STM32FWLIBF1
    if(dev->cmni->type == I2C) {
        GPIO_InitTypeDef GPIO_InitStructure;
        if(dev->cmni->dev == SOFTWARE) {
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;    //初始化SCL
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            DEV_CLK_GPIO_PIN_CONFI(&devio->SCL_SCLK);
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;    //初始化SDA
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            DEV_CLK_GPIO_PIN_CONFI(&devio->SDA_SDO_OWIO);
        } else if(dev->cmni->dev == HARDWARE) {
            //固件库的硬件I2C初始化,待补充
        }
    } else if(dev->cmni->type == SPI) {
        GPIO_InitTypeDef GPIO_InitStructure;
        if(dev->cmni->dev == SOFTWARE) {
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;    //初始化SCLK
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            DEV_CLK_GPIO_PIN_CONFI(&devio->SCL_SCLK);
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;    //初始化SDO
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            DEV_CLK_GPIO_PIN_CONFI(&devio->SDA_SDO_OWIO);
            if(devio->CS.GPIOx != NULL) {
                GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //初始化CS
                GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
                DEV_CLK_GPIO_PIN_CONFI(&devio->CS);
            }
        } else if(dev->cmni->dev == HARDWARE) {
            //固件库的硬件SPI初始化,待补充
        }
    } else if(dev->cmni->type == OWRE) {
        if(dev->cmni->dev == SOFTWARE) {
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;    //初始化OWIO
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            DEV_CLK_GPIO_PIN_CONFI(&devio->SDA_SDO_OWIO);
        }
    }
#endif
    //IO电平配置
    if(dev->cmni->type == I2C) {
        if(dev->cmni->dev == SOFTWARE) {
            DEVIO_WritePin_SET(&devio->SCL_SCLK);
            DEVIO_WritePin_SET(&devio->SDA_SDO_OWIO);
        }
    } else if(dev->cmni->type == SPI) {
        if(dev->cmni->dev == SOFTWARE) {
            DEVIO_WritePin_SET(&devio->SCL_SCLK);
            DEVIO_WritePin_SET(&devio->SDA_SDO_OWIO);
        }
        if(devio->CS.GPIOx != NULL) {
            DEVIO_WritePin_SET(&devio->CS);
        }
    } else if(dev->cmni->type == OWRE) {
        if(dev->cmni->dev == SOFTWARE) {
            DEVIO_WritePin_SET(&devio->SDA_SDO_OWIO);
        }
    }
}
//    I2C/SPI通信驱动函数
void DEVCMNI_WriteByte(uint8_t data, uint8_t address) {
    DEV_TypeDef *dev = DEV_getdev(_devsstream);
    DEVCMNI_IOTypeDef *devio = (DEVCMNI_IOTypeDef *)dev->io;
    if(dev->cmni->type == I2C) {
        if(dev->cmni->dev == SOFTWARE) {
            MODULAR_I2C_Write((I2C_AnalogHandleTypeDef *)&dev->cmni->handle.hai2c, address, &data, 1, 0, I2CHIGH);
        } else if(dev->cmni->dev == HARDWARE) {
#if MCU_COMPILER == MCU_STM32FWLIBF1
            //固件库的硬件I2C驱动函数,待补充
#elif MCU_COMPILER == MCU_STM32HAL
#if defined(HAL_I2C_MODULE_ENABLED)
            HAL_I2C_Mem_Write((I2C_HandleTypeDef *)dev->cmni->handle.hi2c, (((Oled_TypeDef *)dev->parameter)->oledaddr << 1) | 0X00, address, I2C_MEMADD_SIZE_8BIT, &data, 1, 0x100);
#elif defined(HAL_FMPI2C_MODULE_ENABLED)
            HAL_FMPI2C_Mem_Write(((FMPI2C_HandleTypeDef *)dev->handle), (((Oled_TypeDef *)dev->parameter)->oledaddr << 1) | 0X00, address, FMPI2C_MEMADD_SIZE_8BIT, &data, 1, 0x100);
#endif
#endif
        }
    } else if(dev->cmni->type == SPI) {
        if(dev->cmni->dev == SOFTWARE) {
            MODULAR_SPI_Write((SPI_AnalogHandleTypeDef *)&dev->cmni->handle.haspi, &data, 1);
        } else if(dev->cmni->dev == HARDWARE) {
            if(devio->CS.GPIOx != NULL) {
                DEVIO_WritePin_RESET(&devio->CS);
            }
#if MCU_COMPILER == MCU_STM32FWLIBF1
            //固件库的硬件SPI驱动函数,待补充
#elif MCU_COMPILER == MCU_STM32HAL
#if defined(HAL_SPI_MODULE_ENABLED)
            //todo: 检查硬件句柄是否为null
            HAL_SPI_Transmit((SPI_HandleTypeDef *)dev->cmni->handle.hspi, &data, 1, 0x100);
#elif defined(HAL_QSPI_MODULE_ENABLED)
            //todo: SPI_Write for QSPI
#endif
#endif
            if(devio->CS.GPIOx != NULL) {
                DEVIO_WritePin_SET(&devio->CS);
            }
        }
    }
}
void DEVCMNI_WriteConti(uint8_t *pdata, uint16_t size, uint8_t address) {
    DEV_TypeDef *dev = DEV_getdev(_devsstream);
    DEVCMNI_IOTypeDef *devio = (DEVCMNI_IOTypeDef *)dev->io;
    if(dev->cmni->type == I2C) {
        if(dev->cmni->dev == SOFTWARE) {
            MODULAR_I2C_Write((I2C_AnalogHandleTypeDef *)&dev->cmni->handle.hai2c, address, pdata, size, 0, I2CHIGH);
        } else if(dev->cmni->dev == HARDWARE) {
#if MCU_COMPILER == MCU_STM32FWLIBF1
            //固件库的硬件I2C驱动函数,待补充
#elif MCU_COMPILER == MCU_STM32HAL
#if defined(HAL_I2C_MODULE_ENABLED)
            HAL_I2C_Mem_Write((I2C_HandleTypeDef *)dev->cmni->handle.hi2c, (((Oled_TypeDef *)dev->parameter)->oledaddr << 1) | 0X00, address, I2C_MEMADD_SIZE_8BIT, pdata, size, 0x100);
#elif defined(HAL_FMPI2C_MODULE_ENABLED)
            HAL_FMPI2C_Mem_Write(((FMPI2C_HandleTypeDef *)dev->handle), (((Oled_TypeDef *)dev->parameter)->oledaddr << 1) | 0X00, address, FMPI2C_MEMADD_SIZE_8BIT, pdata, size, 0x100);
#endif
#endif
        }
    } else if(dev->cmni->type == SPI) {
        if(dev->cmni->dev == SOFTWARE) {
            MODULAR_SPI_Write((SPI_AnalogHandleTypeDef *)&dev->cmni->handle.haspi, pdata, size);
        } else if(dev->cmni->dev == HARDWARE) {
            if(devio->CS.GPIOx != NULL) {
                DEVIO_WritePin_RESET(&devio->CS);
            }
#if MCU_COMPILER == MCU_STM32FWLIBF1
            //固件库的硬件SPI驱动函数,待补充
#elif MCU_COMPILER == MCU_STM32HAL
#if defined(HAL_SPI_MODULE_ENABLED)
            HAL_SPI_Transmit((SPI_HandleTypeDef *)dev->cmni->handle.hspi, pdata, size, 0x100);
#elif defined(HAL_QSPI_MODULE_ENABLED)
            //todo: SPI_Write for QSPI
#endif
#endif
            if(devio->CS.GPIOx != NULL) {
                DEVIO_WritePin_SET(&devio->CS);
            }
        }
    }
}

/////////////////////////    设备初始化配置    /////////////////////////
void DEV_Confi(DEVS_TypeDef *devs, void *parameter, void *io, DEVCMNI_TypeDef *cmni) {
    static int8_t first_init = 1;
    if(first_init == 1) {
        first_init = 0;
        DEVBUSYLIST_Init();    //程序首次初始化设备实例时初始化忙设备表
        DEVCMNI_Init();        //程序首次初始化设备实例时初始化通信实例
    }
    if(DEV_Init(devs, parameter, io, cmni) == 1) {    //初始化一类设备实例到设备池中
        DEV_Error();
    }
    for(devpool_size i = 0; i < devs->size; i++) {    //若设备实例有通信配置, 则初始化所有通信引脚
        if(DEV_setistm(devs, i) == 1) {
            DEV_Error();
        }
        DEVCMNI_IOInit();
    }
    return;
}
