#include "device.h"
#include "timer.h"

/////////////////////////    设备控制部分    /////////////////////////
static devpool_size _devsize = 0;
static DEVS_TypeDef *_actdevs = NULL;
static DEV_TypeDef *_actdev = NULL;
static DEV_TypeDef *_devpool[DEVPOOL_MAXNUM] = {0};
static uint64_t _devbusylist[DEVBUSYLIST_MAXNUM] = {0};    //忙设备表, 0为空, 非0为设备置忙时刻(us)
/**
 * @description: 活动设备类设置: 设置活动设备类, 设备流根据活动设备类切换
 * @param {DEVS_TypeDef} *self
 * @return {*}
 */
int8_t DEV_SetActDevs(DEVS_TypeDef *self) {
    if(self == NULL) {
        return 1;
    }
    _actdevs = self;
    _actdev = _devpool[self->pool + self->stream];
    return 0;
}
/**
 * @description: 活动设备类获取: 获取当前活动设备类
 * @param {*}
 * @return {*}
 */
DEVS_TypeDef *DEV_GetActDevs(void) {
    return _actdevs;
}
/**
 * @description: 活动设备流设置: 设置活动设备类和设备流
 * @param {DEVS_TypeDef} *self
 * @param {devpool_size} stream
 * @return {*}
 */
int8_t DEV_SetActStream(DEVS_TypeDef *self, devpool_size stream) {
    if(self == NULL || stream > _devsize || stream > self->size) {
        return 1;
    }
    self->stream = stream;
    _actdevs = self;
    _actdev = _devpool[self->pool + self->stream];
    return 0;
}
/**
 * @description: 活动设备流获取: 获取当前活动设备类的设备流
 * @param {*}
 * @return {*}
 */
DEV_TypeDef *DEV_GetActStream(void) {
    return _actdev;
}
/**
 * @description: 关闭活动设备流
 * @param {*}
 * @return {*}
 */
void DEV_CloseActStream(void) {
    // _actdev = NULL;
    // _actdevs = NULL;
}
/**
 * @description: 设备流设置: 设置某设备类的设备流
 * @param {DEVS_TypeDef} *self
 * @param {devpool_size} stream
 * @return {*}
 */
int8_t DEV_SetStream(DEVS_TypeDef *self, devpool_size stream) {
    if(self == NULL || stream > _devsize || stream > self->size) {
        return 1;
    }
    self->stream = stream;
    return 0;
}
/**
 * @description: 设备流获取: 获取某设备类的设备流
 * @param {DEVS_TypeDef} *self
 * @return {*}
 */
DEV_TypeDef *DEV_GetStream(DEVS_TypeDef *self) {
    if(self == NULL) {
        return NULL;
    }
    return _devpool[self->pool + self->stream];
}
/**
 * @description: 设置活动设备类设备的状态, us>0设置设备忙时长(单位为10us),us=0设置设备闲
 * @param {DEVS_TypeDef} *self
 * @param {uint16_t} tus
 * @return {*}
 */
int8_t DEV_SetActState(uint16_t tus) {
    if(tus > 0 && _actdev->state == 0) {                           //设置设备状态为忙
        for(busypool_size i = 0; i < DEVBUSYLIST_MAXNUM; i++) {    //遍历设备表寻找空位
            if(_devbusylist[i] == 0) {
                _actdev->state = i + 1;    //设置成功, 保存当前设备忙时间记录在忙设备列表的第几个
                _devbusylist[i] = TIMER_getRunTimeus() + tus * 10;
                break;
            }
        }
        if(_actdev->state == 0) {
            return 1;    //设备状态表已满, 设置设备忙失败
        }
    } else if(tus > 0 && _actdev->state > 0) {    //更新设备置忙时间
        _devbusylist[_actdev->state - 1] = TIMER_getRunTimeus() + tus * 10;
    } else if(tus == 0 && _actdev->state > 0) {    //设置设备状态为闲
        _devbusylist[_actdev->state - 1] = 0;
        _actdev->state = 0;
    }
    return 0;
}
/**
 * @description: 获取设备类设备的状态, 先查询忙设备是否满足空闲条件, 然后更新状态并返回
 * @param {DEVS_TypeDef} *self
 * @return {*}
 */
DEV_StateTypeDef DEV_GetActState(void) {
    if(_actdev->state > 0 && TIMER_getRunTimeus() > _devbusylist[_actdev->state - 1]) {
        _devbusylist[_actdev->state - 1] = 0;    //设备忙指定时间后转为空闲状态
        _actdev->state = 0;
    }
    return _actdev->state == 0 ? idle : busy;
}
/**
 * @description: 初始化一个设备类, 从设备池中分配空闲指针指向设备数组, 并将序号更新至设备类句柄中
 * @param {DEVS_TypeDef} *devs
 * @param {DEV_TypeDef} dev
 * @return {*}
 */
int8_t DEV_Init(DEVS_TypeDef *devs, DEV_TypeDef dev[]) {
    if(_devsize + devs->size >= DEVPOOL_MAXNUM) {
        return 1;    //申请失败
    }
    devs->pool = _devsize;
    devs->stream = 0;
    _devsize += devs->size;
    for(devpool_size i = 0; i < devs->size; i++) {
        _devpool[devs->pool + i] = dev + i;
    }
    return 0;
}
/**
 * @description: 对活动设备类批量进行某一操作, 过程中会修改_actdev变量
 * @param {DEVS_TypeDef} *devs
 * @param {void(*)()} *action
 * @return {*}
 */
void DEV_ReCall(void (*action)(void)) {
    DEV_TypeDef *tempdev = _actdev;
    for(devpool_size i = _actdevs->pool; i < _actdevs->pool + _actdevs->size; i++) {
        _actdev = _devpool[i];
        action();
    }
    _actdev = tempdev;
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


/////////////////////////    IO操作部分    /////////////////////////
//todo: 考虑能否模仿hal库, 改为使用assert_param进行有效性判断
#if defined(STM32)
#if defined(STM32REGISTER)
#if !defined(DEVICE_USEMACRO)
inline void DEVIO_WritePin_SET(DEVIO_TypeDef *DEVIO) {
    // void DEV_Error(void);
    // if(DEVIO != NULL && DEVIO->GPIOx == NULL) {
    //     DEV_Error();
    // }
    /* Check the parameters */
    assert_param(IS_GPIO_PIN(DEVIO->GPIO_Pin));
    assert_param(IS_GPIO_PIN_ACTION(GPIO_PIN_SET));
    DEVIO->GPIOx->BSRR = DEVIO->GPIO_Pin;
}
inline void DEVIO_WritePin_RESET(DEVIO_TypeDef *DEVIO) {
    // void DEV_Error(void);
    // if(DEVIO != NULL && DEVIO->GPIOx == NULL) {
    //     DEV_Error();
    // }
    /* Check the parameters */
    assert_param(IS_GPIO_PIN(DEVIO->GPIO_Pin));
    assert_param(IS_GPIO_PIN_ACTION(GPIO_PIN_RESET));
    DEVIO->GPIOx->BSRR = (uint32_t)DEVIO->GPIO_Pin << 16U;
}
inline DEVIO_PinState DEVIO_ReadPin(DEVIO_TypeDef *DEVIO) {
    // void DEV_Error(void);
    // if(DEVIO != NULL && DEVIO->GPIOx == NULL) {
    //     DEV_Error();
    // }
    GPIO_PinState bitstatus;
    /* Check the parameters */
    assert_param(IS_GPIO_PIN(DEVIO->GPIO_Pin));
    if((DEVIO->GPIOx->IDR & DEVIO->GPIO_Pin) != (uint32_t)GPIO_PIN_RESET) {
        bitstatus = GPIO_PIN_SET;
    } else {
        bitstatus = GPIO_PIN_RESET;
    }
    return (DEVIO_PinState)bitstatus;
}
inline void DEV_CLK_GPIO_PIN_CONFI(DEVIO_TypeDef *DEVIO, GPIO_InitTypeDef *GPIO_InitStructure) {
    //todo:clkinit
    GPIO_InitStructure->Pin = DEVIO->GPIO_Pin;
    HAL_GPIO_Init(DEVIO->GPIOx, GPIO_InitStructure);
}
#endif    // DEVICE_USEMACRO
#elif defined(STM32HAL)
#if !defined(DEVICE_USEMACRO)
inline void DEVIO_WritePin_SET(DEVIO_TypeDef *DEVIO) {
    void DEV_Error(void);
    if(DEVIO != NULL && DEVIO->GPIOx == NULL) {
        DEV_Error();
    }
    HAL_GPIO_WritePin(DEVIO->GPIOx, DEVIO->GPIO_Pin, GPIO_PIN_SET);
}
inline void DEVIO_WritePin_RESET(DEVIO_TypeDef *DEVIO) {
    void DEV_Error(void);
    if(DEVIO != NULL && DEVIO->GPIOx == NULL) {
        DEV_Error();
    }
    HAL_GPIO_WritePin(DEVIO->GPIOx, DEVIO->GPIO_Pin, GPIO_PIN_RESET);
}
inline DEVIO_PinState DEVIO_ReadPin(DEVIO_TypeDef *DEVIO) {
    void DEV_Error(void);
    if(DEVIO != NULL && DEVIO->GPIOx == NULL) {
        DEV_Error();
    }
    return (DEVIO_PinState)HAL_GPIO_ReadPin(DEVIO->GPIOx, DEVIO->GPIO_Pin);
}
inline void DEV_CLK_GPIO_PIN_CONFI(DEVIO_TypeDef *DEVIO, GPIO_InitTypeDef *GPIO_InitStructure) {
    //todo:clkinit
    GPIO_InitStructure->Pin = DEVIO->GPIO_Pin;
    HAL_GPIO_Init(DEVIO->GPIOx, GPIO_InitStructure);
}
#endif    // DEVICE_USEMACRO
#elif defined(STM32FWLIB)
#if !defined(DEVICE_USEMACRO)
inline void DEVIO_WritePin_SET(DEVIO_TypeDef *DEVIO) {
    if(DEVIO != NULL && DEVIO->GPIOx != NULL) {
        GPIO_SetBits(DEVIO->GPIOx, DEVIO->GPIO_Pin);
    }
}
inline void DEVIO_WritePin_RESET(DEVIO_TypeDef *DEVIO) {
    if(DEVIO != NULL && DEVIO->GPIOx != NULL) {
        GPIO_ResetBits(DEVIO->GPIOx, DEVIO->GPIO_Pin);
    }
}
inline DEVIO_PinState DEVIO_ReadPin(DEVIO_TypeDef *DEVIO) {
    if(DEVIO != NULL && DEVIO->GPIOx != NULL) {
        return (DEVIO_PinState)GPIO_ReadInputDataBit(DEVIO->GPIOx, DEVIO->GPIO_Pin);
    }
    return HIGH;
}
inline void DEV_CLK_GPIO_PIN_CONFI(DEVIO_TypeDef *DEVIO, GPIO_InitTypeDef *GPIO_InitStructure) {
    RCC_APB2PeriphClockCmd(DEVIO->CLK, ENABLE);
    GPIO_InitStructure->GPIO_Pin = DEVIO->GPIO_Pin;
    GPIO_Init(DEVIO->GPIOx, GPIO_InitStructure);
}
#endif    // DEVICE_USEMACRO
#endif
#endif



/////////////////////////    通信协议实现部分    /////////////////////////
//    模拟通信的IO操作绑定
#if defined(DEVICE_I2C_SOFTWARE_ENABLED)
inline void DEVCMNI_SCL_Set(Direct_TypeDef dir) {}
inline void DEVCMNI_SDA_Set(Direct_TypeDef dir) {}
inline void DEVCMNI_SCL_Out(Potential_TypeDef pot) {
    DEVCMNI_IOTypeDef *devio = DEV_GetActStream()->io;
    if(pot != LOW)
        DEVIO_WritePin_SET(&devio->SCL_SCLK);
    else
        DEVIO_WritePin_RESET(&devio->SCL_SCLK);
}
inline void DEVCMNI_SDA_Out(Potential_TypeDef pot) {
    DEVCMNI_IOTypeDef *devio = DEV_GetActStream()->io;
    if(pot != LOW)
        DEVIO_WritePin_SET(&devio->SDA_SDO_OWIO);
    else
        DEVIO_WritePin_RESET(&devio->SDA_SDO_OWIO);
}
inline Potential_TypeDef DEVCMNI_SDA_In(void) {
    DEVCMNI_IOTypeDef *devio = DEV_GetActStream()->io;
    DEVIO_PinState bit = DEVIO_ReadPin(&devio->SDA_SDO_OWIO);
    if(bit != DEVIO_PIN_RESET)
        return HIGH;
    else
        return LOW;
}
#endif    // DEVICE_I2C_SOFTWARE_ENABLED
#if defined(DEVICE_SPI_SOFTWARE_ENABLED)
inline void DEVCMNI_SCLK_Out(Potential_TypeDef pot) {
    DEVCMNI_IOTypeDef *devio = DEV_GetActStream()->io;
    if(pot == LOW)
        DEVIO_WritePin_RESET(&devio->SCL_SCLK);
    else
        DEVIO_WritePin_SET(&devio->SCL_SCLK);
}
inline void DEVCMNI_SDO_Out(Potential_TypeDef pot) {
    DEVCMNI_IOTypeDef *devio = DEV_GetActStream()->io;
    if(pot == LOW)
        DEVIO_WritePin_RESET(&devio->SDA_SDO_OWIO);
    else
        DEVIO_WritePin_SET(&devio->SDA_SDO_OWIO);
}
inline void DEVCMNI_CS_Out(Potential_TypeDef pot) {
    DEVCMNI_IOTypeDef *devio = DEV_GetActStream()->io;
    if(devio->CS.GPIOx != NULL) {
        if(pot == LOW)
            DEVIO_WritePin_RESET(&devio->CS);
        else
            DEVIO_WritePin_SET(&devio->CS);
    }
}
#endif    // DEVICE_SPI_SOFTWARE_ENABLED
#if defined(DEVICE_ONEWIRE_SOFTWARE_ENABLED)
void DEVCMNI_OWIO_Set(Direct_TypeDef dir) {}
void DEVCMNI_OWIO_Out(Potential_TypeDef pot) {
    DEVCMNI_IOTypeDef *devio = DEV_GetActStream()->io;
    if(pot == LOW)
        DEVIO_WritePin_RESET(&devio->SDA_SDO_OWIO);
    else
        DEVIO_WritePin_SET(&devio->SDA_SDO_OWIO);
}
Potential_TypeDef DEVCMNI_OWIO_In(void) {
    DEVCMNI_IOTypeDef *devio = DEV_GetActStream()->io;
    DEVIO_PinState bit = DEVIO_ReadPin(&devio->SDA_SDO_OWIO);
    if(bit == DEVIO_PIN_RESET)
        return LOW;
    else
        return HIGH;
}
#endif    // DEVICE_ONEWIRE_SOFTWARE_ENABLED
inline void DEVCMNI_Delayus(uint16_t us) {
    if(us) {
#if defined(STM32)
#if defined(STM32HAL)
        delayus_timer(us);
#elif defined(STM32FWLIB)
        delayus_timer(us);
#endif
#endif
    }
}
inline void DEVCMNI_Delayms(uint16_t ms) {
    if(ms) {
#if defined(STM32)
#if defined(STM32HAL)
#ifdef __TIMER_H
        delayms_timer(ms);
#else
        HAL_Delay(ms);
#endif
#elif defined(STM32FWLIB)
        delayms_timer(ms);
#endif
#endif
    }
}

//模拟通信基于IO操作的实例
#ifdef DEVICE_I2C_SOFTWARE_ENABLED
I2C_SoftHandleTypeDef ahi2c[SOFTBUS_I2C] = {
    {
        .SCL_Set = DEVCMNI_SCL_Set,
        .SDA_Set = DEVCMNI_SDA_Set,
        .SCL_Out = DEVCMNI_SCL_Out,
        .SDA_Out = DEVCMNI_SDA_Out,
        .SDA_In = DEVCMNI_SDA_In,
        .delayus = DEVCMNI_Delayus,
        .delayms = DEVCMNI_Delayms,
    },
};
#endif
#ifdef DEVICE_SPI_SOFTWARE_ENABLED
SPI_SoftHandleTypeDef ahspi[SOFTBUS_SPI] = {
    {
        .SCLK_Out = DEVCMNI_SCLK_Out,
        .SDO_Out = DEVCMNI_SDO_Out,
        .CS_Out = DEVCMNI_CS_Out,
        .delayus = DEVCMNI_Delayus,
        .delayms = DEVCMNI_Delayms,
    },
};
#endif
#ifdef DEVICE_ONEWIRE_SOFTWARE_ENABLED
ONEWIRE_SoftHandleTypeDef ahowre[SOFTBUS_ONEWIRE] = {
    {
        .num = 1,
        .flag_search = 0,
        .OWIO_Set = DEVCMNI_OWIO_Set,
        .OWIO_Out = DEVCMNI_OWIO_Out,
        .OWIO_In = DEVCMNI_OWIO_In,
        .delayus = DEVCMNI_Delayus,
        .delayms = DEVCMNI_Delayms,
    },
};
#endif


//    I2C/SPI/ONEWIRE通信IO初始化配置
void DEVCMNI_IOInit(void) {
    DEV_TypeDef *dev = DEV_GetActStream();
    DEVCMNI_IOTypeDef *devio = (DEVCMNI_IOTypeDef *)dev->io;
    //todo: 考虑能否模仿hal库, 改为使用assert_param进行有效性判断
    if(dev->cmni == NULL) {    //若设备没有通信配置, 则直接返回
        return;
    }
    if(dev->cmni->type == 0 || dev->cmni->dev == 0) {    //若设备通信配置未正确配置, 则报错
        DEV_Error();
    }
    if(dev->cmni->handle == NULL || dev->io == NULL) {    //若设备通信句柄为空, 或IO句柄为空, 则报错
        DEV_Error();
    }
    //不同设备实际使用的通信引脚不同, 未定义的引脚不会被初始化
#if defined(STM32HAL)
    //HAL库的初始化可由cubeMX在main函数中完成, 此处会再次对通信的引脚进行初始化
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    if(dev->cmni->type == I2C) {
        if(dev->cmni->dev == SOFTWARE) {
            GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;    //初始化SCL
            GPIO_InitStructure.Pull = GPIO_PULLUP;
            GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
            DEV_CLK_GPIO_PIN_CONFI(&devio->SCL_SCLK, &GPIO_InitStructure);
            GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;    //初始化SDA
            GPIO_InitStructure.Pull = GPIO_PULLUP;
            GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
            DEV_CLK_GPIO_PIN_CONFI(&devio->SDA_SDO_OWIO, &GPIO_InitStructure);
        } else if(dev->cmni->dev == HARDWARE) {
            //hal库的硬件I2C初始化,待补充
        }
    } else if(dev->cmni->type == SPI) {
        if(dev->cmni->dev == SOFTWARE) {
            GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;    //初始化SCLK
            GPIO_InitStructure.Pull = GPIO_PULLUP;
            GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
            DEV_CLK_GPIO_PIN_CONFI(&devio->SCL_SCLK, &GPIO_InitStructure);
            GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;    //初始化SDO
            GPIO_InitStructure.Pull = GPIO_PULLUP;
            GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
            DEV_CLK_GPIO_PIN_CONFI(&devio->SDA_SDO_OWIO, &GPIO_InitStructure);
            if(devio->CS.GPIOx != NULL) {
                GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;    //初始化CS
                GPIO_InitStructure.Pull = GPIO_NOPULL;
                GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
                DEV_CLK_GPIO_PIN_CONFI(&devio->CS, &GPIO_InitStructure);
            }
        } else if(dev->cmni->dev == HARDWARE) {
            //hal库的硬件SPI初始化,待补充
        }
    } else if(dev->cmni->type == ONEWIRE) {
        if(dev->cmni->dev == SOFTWARE) {
            GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;    //初始化OWIO
            GPIO_InitStructure.Pull = GPIO_PULLUP;
            GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
            DEV_CLK_GPIO_PIN_CONFI(&devio->SDA_SDO_OWIO, &GPIO_InitStructure);
        }
    }
#elif defined(STM32FWLIBF1)
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    if(dev->cmni->type == I2C) {
        if(dev->cmni->dev == SOFTWARE) {
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;    //初始化SCL
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            DEV_CLK_GPIO_PIN_CONFI(&devio->SCL_SCLK, &GPIO_InitStructure);
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;    //初始化SDA
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            DEV_CLK_GPIO_PIN_CONFI(&devio->SDA_SDO_OWIO, &GPIO_InitStructure);
        } else if(dev->cmni->dev == HARDWARE) {
            //固件库的硬件I2C初始化,待补充
        }
    } else if(dev->cmni->type == SPI) {
        if(dev->cmni->dev == SOFTWARE) {
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;    //初始化SCLK
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            DEV_CLK_GPIO_PIN_CONFI(&devio->SCL_SCLK, &GPIO_InitStructure);
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;    //初始化SDO
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            DEV_CLK_GPIO_PIN_CONFI(&devio->SDA_SDO_OWIO, &GPIO_InitStructure);
            if(devio->CS.GPIOx != NULL) {
                GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //初始化CS
                GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
                DEV_CLK_GPIO_PIN_CONFI(&devio->CS, &GPIO_InitStructure);
            }
        } else if(dev->cmni->dev == HARDWARE) {
            //固件库的硬件SPI初始化,待补充
        }
    } else if(dev->cmni->type == ONEWIRE) {
        if(dev->cmni->dev == SOFTWARE) {
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;    //初始化OWIO
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            DEV_CLK_GPIO_PIN_CONFI(&devio->SDA_SDO_OWIO, &GPIO_InitStructure);
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
            DEVIO_WritePin_RESET(&devio->CS);
        }
    } else if(dev->cmni->type == ONEWIRE) {
        if(dev->cmni->dev == SOFTWARE) {
            DEVIO_WritePin_SET(&devio->SDA_SDO_OWIO);
        }
    }
}
//    I2C/SPI通信驱动函数
//todo: 检查硬件句柄是否为null
void DEVCMNI_WriteByte(uint8_t data, uint8_t address, int8_t skip) {
    DEV_TypeDef *dev = DEV_GetActStream();
    DEVCMNI_IOTypeDef *devio = (DEVCMNI_IOTypeDef *)dev->io;
    void *handle = dev->cmni->handle;
    if(dev->cmni->type == I2C) {
#if defined(DEVICE_I2C_SOFTWARE_ENABLED)
        if(dev->cmni->dev == SOFTWARE) {
            MODULAR_I2C_WriteByte((I2C_ModuleHandleTypeDef *)handle, address, data, skip, 0x0);
        } else if(dev->cmni->dev == HARDWARE) {
#if defined(STM32HAL)
#if defined(HAL_I2C_MODULE_ENABLED)
            HAL_I2C_Mem_Write(((I2C_ModuleHandleTypeDef *)handle)->bushandle, (((I2C_ModuleHandleTypeDef *)handle)->addr << 1) | 0X00, address, I2C_MEMADD_SIZE_8BIT, &data, 1, 0x100);
#elif defined(HAL_FMPI2C_MODULE_ENABLED)
            HAL_FMPI2C_Mem_Write(((I2C_ModuleHandleTypeDef *)handle)->bushandle, (((I2C_ModuleHandleTypeDef *)handle)->addr << 1) | 0X00, address, FMPI2C_MEMADD_SIZE_8BIT, &data, 1, 0x100);
#endif    // HAL_I2C_MODULE_ENABLED | HAL_FMPI2C_MODULE_ENABLED
#elif defined(STM32FWLIBF1)
            //固件库的硬件I2C驱动函数,待补充
#endif
        }
#endif    // DEVICE_I2C_SOFTWARE_ENABLED
    } else if(dev->cmni->type == SPI) {
#if defined(DEVICE_SPI_SOFTWARE_ENABLED)
        if(dev->cmni->dev == SOFTWARE) {
            MODULAR_SPI_WriteByte((SPI_ModuleHandleTypeDef *)handle, data, skip, 0x0);
        } else if(dev->cmni->dev == HARDWARE) {
            if(devio->CS.GPIOx != NULL) {
                DEVIO_WritePin_SET(&devio->CS);
            }
#if defined(STM32HAL)
#if defined(HAL_SPI_MODULE_ENABLED)
            HAL_SPI_Transmit(((SPI_ModuleHandleTypeDef *)handle)->bushandle, &data, 1, 0x100);
#elif defined(HAL_QSPI_MODULE_ENABLED)
            //todo: SPI_Write for QSPI
#endif    // HAL_SPI_MODULE_ENABLED | HAL_QSPI_MODULE_ENABLED
#elif defined(STM32FWLIBF1)
            //固件库的硬件SPI驱动函数,待补充
#endif
            if(devio->CS.GPIOx != NULL) {
                DEVIO_WritePin_SET(&devio->CS);
            }
        }
#endif    // DEVICE_SPI_SOFTWARE_ENABLED
    } else if(dev->cmni->type == ONEWIRE) {
#if defined(DEVICE_ONEWIRE_SOFTWARE_ENABLED)
        if(dev->cmni->dev == SOFTWARE) {
            MODULAR_ONEWIRE_WriteByte((ONEWIRE_ModuleHandleTypeDef *)handle, data, skip, 0x0);
        }
#endif    // DEVICE_ONEWIRE_SOFTWARE_ENABLED
    }
}
extern uint32_t time5, time6, time7, time8;
void DEVCMNI_Write(uint8_t *pdata, uint16_t size, uint8_t address, int8_t skip) {
    DEV_TypeDef *dev = DEV_GetActStream();
    DEVCMNI_IOTypeDef *devio = (DEVCMNI_IOTypeDef *)dev->io;
    void *handle = dev->cmni->handle;
    if(dev->cmni->type == I2C) {
        time5 = TIMER_getRunTimeus();
#if defined(DEVICE_I2C_SOFTWARE_ENABLED)
        if(dev->cmni->dev == SOFTWARE) {
            MODULAR_I2C_Write((I2C_ModuleHandleTypeDef *)handle, address, pdata, size, skip, 0x0);
        } else if(dev->cmni->dev == HARDWARE) {
#if defined(STM32HAL)
#if defined(HAL_I2C_MODULE_ENABLED)
            HAL_I2C_Mem_Write(((I2C_ModuleHandleTypeDef *)handle)->bushandle, (((I2C_ModuleHandleTypeDef *)handle)->addr << 1) | 0X00, address, I2C_MEMADD_SIZE_8BIT, pdata, size, 0x100);
#elif defined(HAL_FMPI2C_MODULE_ENABLED)
            HAL_FMPI2C_Mem_Write(((I2C_ModuleHandleTypeDef *)handle)->bushandle, (((I2C_ModuleHandleTypeDef *)handle)->addr << 1) | 0X00, address, FMPI2C_MEMADD_SIZE_8BIT, pdata, size, 0x100);
#endif    // HAL_I2C_MODULE_ENABLED | HAL_FMPI2C_MODULE_ENABLED
#elif defined(STM32FWLIBF1)
            //固件库的硬件I2C驱动函数,待补充
#endif
        }
#endif    // DEVICE_SPI_SOFTWARE_ENABLED
        time6 = TIMER_getRunTimeus();
    } else if(dev->cmni->type == SPI) {
        time7 = TIMER_getRunTimeus();
#if defined(DEVICE_SPI_SOFTWARE_ENABLED)
        if(dev->cmni->dev == SOFTWARE) {
            MODULAR_SPI_Write(((SPI_ModuleHandleTypeDef *)handle), pdata, size, skip, 0x0);
        } else if(dev->cmni->dev == HARDWARE) {
            if(devio->CS.GPIOx != NULL) {
                DEVIO_WritePin_RESET(&devio->CS);
            }
#if defined(STM32HAL)
#if defined(HAL_SPI_MODULE_ENABLED)
            HAL_SPI_Transmit(((SPI_ModuleHandleTypeDef *)handle)->bushandle, pdata, size, 0x100);
#elif defined(HAL_QSPI_MODULE_ENABLED)
            //todo: SPI_Write for QSPI
#endif    // HAL_SPI_MODULE_ENABLED | HAL_QSPI_MODULE_ENABLED
#elif defined(STM32FWLIBF1)
            //固件库的硬件SPI驱动函数,待补充
#endif
            if(devio->CS.GPIOx != NULL) {
                DEVIO_WritePin_SET(&devio->CS);
            }
        }
#endif    // DEVICE_SPI_SOFTWARE_ENABLED
        time8 = TIMER_getRunTimeus();
    } else if(dev->cmni->type == ONEWIRE) {
#if defined(DEVICE_ONEWIRE_SOFTWARE_ENABLED)
        if(dev->cmni->dev == SOFTWARE) {
            MODULAR_ONEWIRE_Write((ONEWIRE_ModuleHandleTypeDef *)handle, pdata, size, skip, 0x0);
        }
#endif    // DEVICE_SPI_SOFTWARE_ENABLED
    }
}

/////////////////////////    设备初始化配置    /////////////////////////
void DEV_Confi(DEVS_TypeDef *devs, DEV_TypeDef dev[]) {
    if(DEV_Init(devs, dev) == 1) {    //初始化一类设备实例到设备池中
        DEV_Error();
    }
    DEV_SetActStream(devs, 0);
    DEV_ReCall(DEVCMNI_IOInit);    //若设备实例有通信配置, 则初始化所有通信引脚
    DEV_CloseActStream();
    return;
}
