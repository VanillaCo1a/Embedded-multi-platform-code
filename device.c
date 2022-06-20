#include "device.h"
#include "timer.h"

/////////////////////////    设备控制部分    /////////////////////////
static devpool_size _devsize = 0;
static DEVS_TypeDef *_actdevs = NULL;
static DEV_TypeDef *_actdev = NULL;
static DEVCMNIIO_TypeDef *_actdevcmniio = NULL;
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
    _actdevcmniio = _devpool[self->pool + self->stream]->io;
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
    _actdevcmniio = _devpool[self->pool + self->stream]->io;
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
    _actdev = NULL;
    _actdevs = NULL;
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
    if(devs->type == DEVUNDEF || devs->size < 0 || _devsize + devs->size >= DEVPOOL_MAXNUM) {
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
 * @description: 对某一设备类批量进行某一操作
 * @param {DEVS_TypeDef} *devs
 * @param {void(*)()} *action
 * @return {*}
 */
void DEV_ReCall(DEVS_TypeDef *devs, void (*action)(void)) {
    for(devpool_size i = 0; i < devs->size; i++) {
        DEV_SetActStream(devs, i);
        action();
    }
    DEV_CloseActStream();
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
#if defined(STM32HAL)
inline void DEVIO_WritePin_SET(DEVIO_TypeDef *DEVIO) {
#if defined(STM32REGISTER)
    assert_param(IS_GPIO_PIN(DEVIO->GPIO_Pin));
    assert_param(IS_GPIO_PIN_ACTION(GPIO_PIN_SET));
    DEVIO->GPIOx->BSRR = DEVIO->GPIO_Pin;
#else
    if(DEVIO == NULL || DEVIO->GPIOx == NULL)
        DEV_Error();
    HAL_GPIO_WritePin(DEVIO->GPIOx, DEVIO->GPIO_Pin, GPIO_PIN_SET);
#endif
}
inline void DEVIO_WritePin_RESET(DEVIO_TypeDef *DEVIO) {
#if defined(STM32REGISTER)
    assert_param(IS_GPIO_PIN(DEVIO->GPIO_Pin));
    assert_param(IS_GPIO_PIN_ACTION(GPIO_PIN_RESET));
    DEVIO->GPIOx->BSRR = (uint32_t)DEVIO->GPIO_Pin << 16U;
#else
    if(DEVIO == NULL || DEVIO->GPIOx == NULL)
        DEV_Error();
    HAL_GPIO_WritePin(DEVIO->GPIOx, DEVIO->GPIO_Pin, GPIO_PIN_RESET);
#endif
}
inline void DEVIO_WritePin(DEVIO_TypeDef *DEVIO, DEVIO_PinState PinState) {
#if defined(STM32REGISTER)
    assert_param(IS_GPIO_PIN(DEVIO->GPIO_Pin));
    assert_param(IS_GPIO_PIN_ACTION(GPIO_PIN_RESET));
    DEVIO->GPIOx->BSRR = (uint32_t)DEVIO->GPIO_Pin << (!PinState << 4);
    //#define BitBand(Addr, Bit) *((volatile int *)(((int)(Addr)&0x60000000) + 0x02000000 + (int)(Addr)*0x20 + (Bit)*4))
    //BitBand(&DEVIO->GPIOx->ODR, PinState == OLED0_SCK_Pin ? 13 : 15) = PinState;
#else
    if(DEVIO == NULL || DEVIO->GPIOx == NULL)
        DEV_Error();
    HAL_GPIO_WritePin(DEVIO->GPIOx, DEVIO->GPIO_Pin, PinState);
#endif
}
inline DEVIO_PinState DEVIO_ReadPin(DEVIO_TypeDef *DEVIO) {
#if defined(STM32REGISTER)
    assert_param(IS_GPIO_PIN(DEVIO->GPIO_Pin));
    return (bool)(DEVIO->GPIOx->IDR & DEVIO->GPIO_Pin);
#else
    if(DEVIO == NULL || DEVIO->GPIOx == NULL)
        DEV_Error();
    return (DEVIO_PinState)HAL_GPIO_ReadPin(DEVIO->GPIOx, DEVIO->GPIO_Pin);
#endif
}
inline void DEV_CLK_GPIO_PIN_CONFI(DEVIO_TypeDef *DEVIO, GPIO_InitTypeDef *GPIO_InitStructure) {
    //todo:clkinit
    if(DEVIO == NULL || DEVIO->GPIOx == NULL)
        DEV_Error();
    GPIO_InitStructure->Pin = DEVIO->GPIO_Pin;
    HAL_GPIO_Init(DEVIO->GPIOx, GPIO_InitStructure);
}
inline bool DEVIO_NULL(DEVIO_TypeDef *DEVIO) {
    if(DEVIO->GPIOx == NULL) {
        return true;
    }
    return false;
}
#elif defined(STM32FWLIB)
inline void DEVIO_WritePin_SET(DEVIO_TypeDef *DEVIO) {
    if(DEVIO == NULL || DEVIO->GPIOx == NULL)
        DEV_Error();
    GPIO_SetBits(DEVIO->GPIOx, DEVIO->GPIO_Pin);
}
inline void DEVIO_WritePin_RESET(DEVIO_TypeDef *DEVIO) {
    if(DEVIO == NULL || DEVIO->GPIOx == NULL)
        DEV_Error();
    GPIO_ResetBits(DEVIO->GPIOx, DEVIO->GPIO_Pin);
}
inline void DEVIO_WritePin(DEVIO_TypeDef *DEVIO, DEVIO_PinState PinState) {
    if(DEVIO == NULL || DEVIO->GPIOx == NULL)
        DEV_Error();
    if(PinState)
        GPIO_SetBits(DEVIO->GPIOx, DEVIO->GPIO_Pin);
    else
        GPIO_ResetBits(DEVIO->GPIOx, DEVIO->GPIO_Pin);
}
inline DEVIO_PinState DEVIO_ReadPin(DEVIO_TypeDef *DEVIO) {
    if(DEVIO == NULL || DEVIO->GPIOx == NULL)
        DEV_Error();
    return (DEVIO_PinState)GPIO_ReadInputDataBit(DEVIO->GPIOx, DEVIO->GPIO_Pin);
}
inline void DEV_CLK_GPIO_PIN_CONFI(DEVIO_TypeDef *DEVIO, GPIO_InitTypeDef *GPIO_InitStructure) {
    if(DEVIO == NULL || DEVIO->GPIOx == NULL)
        DEV_Error();
    RCC_APB2PeriphClockCmd(DEVIO->CLK, ENABLE);
    GPIO_InitStructure->GPIO_Pin = DEVIO->GPIO_Pin;
    GPIO_Init(DEVIO->GPIOx, GPIO_InitStructure);
}
inline bool DEVIO_NULL(DEVIO_TypeDef *DEVIO) {
    if(DEVIO->GPIOx == NULL) {
        return true;
    }
    return false;
}
#endif
#endif


/////////////////////////    通信协议实现部分    /////////////////////////
//    模拟通信的IO操作绑定
inline void DEVCMNI_SCL_Set(bool dir) {}
inline void DEVCMNI_SDA_OWRE_Set(bool dir) {}
inline void DEVCMNI_SCL_SCK_Out(bool pot) {
    DEVIO_WritePin(&_actdevcmniio->SCL_SCK, pot);
}
inline void DEVCMNI_SDA_SDI_OWRE_Out(bool pot) {
    DEVIO_WritePin(&_actdevcmniio->SDA_SDI_OWRE, pot);
}
inline bool DEVCMNI_SCL_In(void) {
    return DEVIO_ReadPin(&_actdevcmniio->SCL_SCK);
}
inline bool DEVCMNI_SDA_OWRE_In(void) {
    return DEVIO_ReadPin(&_actdevcmniio->SDA_SDI_OWRE);
}
inline bool DEVCMNI_SDO_In(void) {
    return DEVIO_ReadPin(&_actdevcmniio->SDO);
}
inline void DEVCMNI_CS_Out(bool pot) {
    DEVIO_WritePin(&_actdevcmniio->CS, pot);
}
void DEVCMNI_Error(int8_t err) {
    DEV_Error();
}
void DEVCMNI_Delayus(uint16_t us) {
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
void DEVCMNI_Delayms(uint16_t ms) {
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
int8_t DEVCMNI_Delayus_paral(uint16_t us) {
    if(us) {
#if defined(STM32)
#if defined(STM32HAL)
        return delayus_timer_paral(us);
#elif defined(STM32FWLIB)
        return delayus_timer_paral(us);
#endif
#endif
    }
    return 1;
}

//模拟通信基于IO操作的实例
I2C_SoftHandleTypeDef ahi2c[I2C_SOFTBUS_NUM] = {
    {

#ifdef I2CBUS_USEPOINTER
        .SCL_Set = DEVCMNI_SCL_Set,
        .SDA_Set = DEVCMNI_SDA_OWRE_Set,
        .SCL_Out = DEVCMNI_SCL_SCK_Out,
        .SDA_Out = DEVCMNI_SDA_SDI_OWRE_Out,
        .SCL_In = DEVCMNI_SCL_In,
        .SDA_In = DEVCMNI_SDA_OWRE_In,
        .error = DEVCMNI_Error,
        .delayus = DEVCMNI_Delayus,
        .delayms = DEVCMNI_Delayms,
        .delayus_paral = DEVCMNI_Delayus_paral,
#endif
    },
};
SPI_SoftHandleTypeDef ahspi[SPI_SOFTBUS_NUM] = {
    {

#ifdef SPIBUS_USEPOINTER
        .SCK_Out = DEVCMNI_SCL_SCK_Out,
        .SDI_Out = DEVCMNI_SDA_SDI_OWRE_Out,
        .CS_Out = DEVCMNI_CS_Out,
        .error = DEVCMNI_Error,
        .delayus = DEVCMNI_Delayus,
        .delayms = DEVCMNI_Delayms,
        .delayus_paral = DEVCMNI_Delayus_paral,
#endif
    },
};
ONEWIRE_SoftHandleTypeDef ahowre[ONEWIRE_SOFTBUS_NUM] = {
    {
        .num = 1,
        .flag_search = 0,
#ifdef OWREBUS_USEPOINTER
        .OWIO_Set = DEVCMNI_SDA_OWRE_Set,
        .OWIO_Out = DEVCMNI_SDA_SDI_OWRE_Out,
        .OWIO_In = DEVCMNI_SDA_OWRE_In,
        .error = DEVCMNI_Error,
        .delayus = DEVCMNI_Delayus,
        .delayms = DEVCMNI_Delayms,
        .delayus_paral = DEVCMNI_Delayus_paral,
#endif
    },
};

//    I2C/SPI/ONEWIRE通信IO初始化配置
void DEVCMNI_IOInit(void) {
    //todo: 考虑能否模仿hal库, 改为使用assert_param进行有效性判断
    if(_actdev->cmni == NULL) {    //若设备没有通信配置, 则直接返回
        return;
    }
    if(_actdev->cmni->protocol == 0 || _actdev->cmni->ware == 0) {    //若设备通信配置未正确配置, 则报错
        DEV_Error();
    }
    if(_actdev->cmni->handle == NULL || _actdev->io == NULL) {    //若设备通信句柄为空, 或IO句柄为空, 则报错
        DEV_Error();
    }
    //不同设备实际使用的通信引脚不同, 未定义的引脚不会被初始化
#if defined(STM32HAL)
    //HAL库的初始化可由cubeMX在main函数中完成, 此处会再次对通信的引脚进行初始化
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    if(_actdev->cmni->protocol == I2C) {
        if(_actdev->cmni->ware == SOFTWARE) {
            GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
            GPIO_InitStructure.Pull = GPIO_PULLUP;
            GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
            DEV_CLK_GPIO_PIN_CONFI(&_actdevcmniio->SCL_SCK, &GPIO_InitStructure);         //初始化SCL
            DEV_CLK_GPIO_PIN_CONFI(&_actdevcmniio->SDA_SDI_OWRE, &GPIO_InitStructure);    //初始化SDA
        } else if(_actdev->cmni->ware == HARDWARE) {
            //hal库的硬件I2C初始化,待补充
        }
    } else if(_actdev->cmni->protocol == SPI) {
        if(_actdev->cmni->ware == SOFTWARE) {
            GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
            GPIO_InitStructure.Pull = GPIO_NOPULL;
            GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_MEDIUM;
            DEV_CLK_GPIO_PIN_CONFI(&_actdevcmniio->SCL_SCK, &GPIO_InitStructure);    //初始化SCK
            if(!DEVIO_NULL(&_actdevcmniio->SDA_SDI_OWRE)) {
                DEV_CLK_GPIO_PIN_CONFI(&_actdevcmniio->SDA_SDI_OWRE, &GPIO_InitStructure);    //初始化SDI
            }
            if(!DEVIO_NULL(&_actdevcmniio->SDO)) {
                GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
                GPIO_InitStructure.Pull = GPIO_NOPULL;
                GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_MEDIUM;
                DEV_CLK_GPIO_PIN_CONFI(&_actdevcmniio->SDO, &GPIO_InitStructure);    //初始化SDO
            }
            if(!DEVIO_NULL(&_actdevcmniio->CS)) {
                GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
                GPIO_InitStructure.Pull = GPIO_NOPULL;
                GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
                DEV_CLK_GPIO_PIN_CONFI(&_actdevcmniio->CS, &GPIO_InitStructure);    //初始化CS
            }
        } else if(_actdev->cmni->ware == HARDWARE) {
            //hal库的硬件SPI初始化,待补充
        }
    } else if(_actdev->cmni->protocol == ONEWIRE) {
        if(_actdev->cmni->ware == SOFTWARE) {
            GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
            GPIO_InitStructure.Pull = GPIO_PULLUP;
            GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
            DEV_CLK_GPIO_PIN_CONFI(&_actdevcmniio->SDA_SDI_OWRE, &GPIO_InitStructure);    //初始化OWIO
        }
    }
#elif defined(STM32FWLIBF1)
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    if(_actdev->cmni->protocol == I2C) {
        if(_actdev->cmni->ware == SOFTWARE) {
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            DEV_CLK_GPIO_PIN_CONFI(&_actdevcmniio->SCL_SCK, &GPIO_InitStructure);    //初始化SCL
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            DEV_CLK_GPIO_PIN_CONFI(&_actdevcmniio->SDA_SDI_OWRE, &GPIO_InitStructure);    //初始化SDA
        } else if(_actdev->cmni->ware == HARDWARE) {
            //固件库的硬件I2C初始化,待补充
        }
    } else if(_actdev->cmni->protocol == SPI) {
        if(_actdev->cmni->ware == SOFTWARE) {
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            DEV_CLK_GPIO_PIN_CONFI(&_actdevcmniio->SCL_SCK, &GPIO_InitStructure);    //初始化SCK
            if(!DEVIO_NULL(&_actdevcmniio->SDA_SDI_OWRE)) {
                GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
                GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
                DEV_CLK_GPIO_PIN_CONFI(&_actdevcmniio->SDA_SDI_OWRE, &GPIO_InitStructure);    //初始化SDI
            }
            if(!DEVIO_NULL(&_actdevcmniio->SDO)) {
                GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
                GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
                DEV_CLK_GPIO_PIN_CONFI(&_actdevcmniio->SDO, &GPIO_InitStructure);    //初始化SDO
            }
            if(!DEVIO_NULL(&_actdevcmniio->CS)) {
                GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
                GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
                DEV_CLK_GPIO_PIN_CONFI(&_actdevcmniio->CS, &GPIO_InitStructure);    //初始化CS
            }
        } else if(_actdev->cmni->ware == HARDWARE) {
            //固件库的硬件SPI初始化,待补充
        }
    } else if(_actdev->cmni->protocol == ONEWIRE) {
        if(_actdev->cmni->ware == SOFTWARE) {
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            DEV_CLK_GPIO_PIN_CONFI(&_actdevcmniio->SDA_SDI_OWRE, &GPIO_InitStructure);    //初始化OWIO
        }
    }
#endif
    //IO电平配置
    if(_actdev->cmni->protocol == I2C) {
        if(_actdev->cmni->ware == SOFTWARE) {
            DEVIO_WritePin_SET(&_actdevcmniio->SCL_SCK);
            DEVIO_WritePin_SET(&_actdevcmniio->SDA_SDI_OWRE);
        }
    } else if(_actdev->cmni->protocol == SPI) {
        if(_actdev->cmni->ware == SOFTWARE) {
            DEVIO_WritePin_SET(&_actdevcmniio->SCL_SCK);
            if(!DEVIO_NULL(&_actdevcmniio->SDA_SDI_OWRE)) {
                DEVIO_WritePin_SET(&_actdevcmniio->SDA_SDI_OWRE);
            }
            if(!DEVIO_NULL(&_actdevcmniio->SDO)) {
                DEVIO_WritePin_SET(&_actdevcmniio->SDO);
            }
        }
        if(!DEVIO_NULL(&_actdevcmniio->CS)) {
            DEVIO_WritePin_SET(&_actdevcmniio->CS);
        }
    } else if(_actdev->cmni->protocol == ONEWIRE) {
        if(_actdev->cmni->ware == SOFTWARE) {
            DEVIO_WritePin_SET(&_actdevcmniio->SDA_SDI_OWRE);
        }
    }
}
//    I2C/SPI/ONEWIRE通信驱动函数
extern uint32_t time5, time6, time7, time8, time9, time10;
void DEVCMNI_WriteByte(uint8_t byte, uint8_t address, bool skip) {
    if(_actdev == NULL || _actdevcmniio == NULL || _actdev->cmni == NULL || _actdev->cmni->handle == NULL)
        DEV_Error();
    void *handle = _actdev->cmni->handle;
    if(_actdev->cmni->protocol == I2C) {
        if(_actdev->cmni->ware == SOFTWARE) {
#if defined(DEVICE_I2C_SOFTWARE_ENABLED)
            DEV_I2C_Write((I2C_ModuleHandleTypeDef *)handle, address, &byte, 1, skip, 0x0);
#endif    // DEVICE_I2C_SOFTWARE_ENABLED
        } else if(_actdev->cmni->ware == HARDWARE) {
#if defined(STM32HAL)
#if defined(HAL_I2C_MODULE_ENABLED)
            HAL_I2C_Mem_Write(((I2C_ModuleHandleTypeDef *)handle)->bushandle, (((I2C_ModuleHandleTypeDef *)handle)->addr << 1) | 0X00, address, I2C_MEMADD_SIZE_8BIT, &byte, 1, 0x100);
#elif defined(HAL_FMPI2C_MODULE_ENABLED)
            HAL_FMPI2C_Mem_Write(((I2C_ModuleHandleTypeDef *)handle)->bushandle, (((I2C_ModuleHandleTypeDef *)handle)->addr << 1) | 0X00, address, FMPI2C_MEMADD_SIZE_8BIT, &byte, 1, 0x100);
#endif    // HAL_I2C_MODULE_ENABLED | HAL_FMPI2C_MODULE_ENABLED
#elif defined(STM32FWLIBF1)
            //固件库的硬件I2C驱动函数,待补充
#endif
        }
    } else if(_actdev->cmni->protocol == SPI) {
        if(_actdev->cmni->ware == SOFTWARE) {
#if defined(DEVICE_SPI_SOFTWARE_ENABLED)
            DEV_SPI_Transmit((SPI_ModuleHandleTypeDef *)handle, &byte, 1, skip, 0x0);
#endif    // DEVICE_SPI_SOFTWARE_ENABLED
        } else if(_actdev->cmni->ware == HARDWARE) {
            if(_actdevcmniio->CS.GPIOx != NULL) {
                DEVIO_WritePin_RESET(&_actdevcmniio->CS);
            }
#if defined(STM32HAL)
#if defined(HAL_SPI_MODULE_ENABLED)
            HAL_SPI_Transmit(((SPI_ModuleHandleTypeDef *)handle)->bushandle, &byte, 1, 0x100);
#elif defined(HAL_QSPI_MODULE_ENABLED)
            //todo: SPI_Write for QSPI
#endif    // HAL_SPI_MODULE_ENABLED | HAL_QSPI_MODULE_ENABLED
#elif defined(STM32FWLIBF1)
            //固件库的硬件SPI驱动函数,待补充
#endif
            if(_actdevcmniio->CS.GPIOx != NULL) {
                DEVIO_WritePin_SET(&_actdevcmniio->CS);
            }
        }
    } else if(_actdev->cmni->protocol == ONEWIRE) {
        time9 = TIMER_getRunTimeus();
        if(_actdev->cmni->ware == SOFTWARE) {
#if defined(DEVICE_ONEWIRE_SOFTWARE_ENABLED)
            DEV_ONEWIRE_Write((ONEWIRE_ModuleHandleTypeDef *)handle, &byte, 1, skip, 0x0);
#endif    // DEVICE_ONEWIRE_SOFTWARE_ENABLED
        }
        time10 = TIMER_getRunTimeus();
    }
}
uint8_t DEVCMNI_ReadByte(uint8_t address, bool skip) {
    if(_actdev == NULL || _actdevcmniio == NULL || _actdev->cmni == NULL || _actdev->cmni->handle == NULL)
        DEV_Error();
    uint8_t byte = 0;
    void *handle = _actdev->cmni->handle;
    if(_actdev->cmni->protocol == I2C) {
        if(_actdev->cmni->ware == SOFTWARE) {
#if defined(DEVICE_I2C_SOFTWARE_ENABLED)
            DEV_I2C_Read((I2C_ModuleHandleTypeDef *)handle, address, &byte, 1, skip, 0x0);
#endif    // DEVICE_I2C_SOFTWARE_ENABLED
        } else if(_actdev->cmni->ware == HARDWARE) {
#if defined(STM32HAL)
#if defined(HAL_I2C_MODULE_ENABLED)
            HAL_I2C_Mem_Read(((I2C_ModuleHandleTypeDef *)handle)->bushandle, (((I2C_ModuleHandleTypeDef *)handle)->addr << 1) | 0X00, address, I2C_MEMADD_SIZE_8BIT, &byte, 1, 0x100);
#elif defined(HAL_FMPI2C_MODULE_ENABLED)
            HAL_FMPI2C_Mem_Read(((I2C_ModuleHandleTypeDef *)handle)->bushandle, (((I2C_ModuleHandleTypeDef *)handle)->addr << 1) | 0X00, address, FMPI2C_MEMADD_SIZE_8BIT, &byte, 1, 0x100);
#endif    // HAL_I2C_MODULE_ENABLED | HAL_FMPI2C_MODULE_ENABLED
#elif defined(STM32FWLIBF1)
            //固件库的硬件I2C驱动函数,待补充
#endif
        }
    } else if(_actdev->cmni->protocol == SPI) {
        if(_actdev->cmni->ware == SOFTWARE) {
#if defined(DEVICE_SPI_SOFTWARE_ENABLED)
            DEV_SPI_Transmit((SPI_ModuleHandleTypeDef *)handle, &byte, 1, skip, 0x0);
#endif    // DEVICE_SPI_SOFTWARE_ENABLED
        } else if(_actdev->cmni->ware == HARDWARE) {
            if(_actdevcmniio->CS.GPIOx != NULL) {
                DEVIO_WritePin_RESET(&_actdevcmniio->CS);
            }
#if defined(STM32HAL)
#if defined(HAL_SPI_MODULE_ENABLED)
            HAL_SPI_Receive(((SPI_ModuleHandleTypeDef *)handle)->bushandle, &byte, 1, 0x100);
#elif defined(HAL_QSPI_MODULE_ENABLED)
            //todo: SPI_Write for QSPI
#endif    // HAL_SPI_MODULE_ENABLED | HAL_QSPI_MODULE_ENABLED
#elif defined(STM32FWLIBF1)
            //固件库的硬件SPI驱动函数,待补充
#endif
            if(_actdevcmniio->CS.GPIOx != NULL) {
                DEVIO_WritePin_SET(&_actdevcmniio->CS);
            }
        }
    } else if(_actdev->cmni->protocol == ONEWIRE) {
        if(_actdev->cmni->ware == SOFTWARE) {
#if defined(DEVICE_ONEWIRE_SOFTWARE_ENABLED)
            DEV_ONEWIRE_Read((ONEWIRE_ModuleHandleTypeDef *)handle, &byte, 1);
#endif    // DEVICE_ONEWIRE_SOFTWARE_ENABLED
        }
    }
    return byte;
}
bool DEVCMNI_ReadBit(uint8_t address, bool skip) {
    if(_actdev == NULL || _actdevcmniio == NULL || _actdev->cmni == NULL || _actdev->cmni->handle == NULL)
        DEV_Error();
    bool bit = 0;
    void *handle = _actdev->cmni->handle;
    if(_actdev->cmni->protocol == I2C) {
        if(_actdev->cmni->ware == SOFTWARE) {
#if defined(DEVICE_I2C_SOFTWARE_ENABLED)
            //..
#endif    // DEVICE_I2C_SOFTWARE_ENABLED
        }
    } else if(_actdev->cmni->protocol == SPI) {
        if(_actdev->cmni->ware == SOFTWARE) {
#if defined(DEVICE_SPI_SOFTWARE_ENABLED)
            //..
#endif    // DEVICE_SPI_SOFTWARE_ENABLED
        }
    } else if(_actdev->cmni->protocol == ONEWIRE) {
        if(_actdev->cmni->ware == SOFTWARE) {
#if defined(DEVICE_ONEWIRE_SOFTWARE_ENABLED)
            bit = DEV_ONEWIRE_ReadBit((ONEWIRE_ModuleHandleTypeDef *)handle);
#endif    // DEVICE_ONEWIRE_SOFTWARE_ENABLED
        }
    }
    return bit;
}
void DEVCMNI_Write(uint8_t *pdata, uint16_t size, uint8_t address, bool skip) {
    if(_actdev == NULL || _actdevcmniio == NULL || _actdev->cmni == NULL || _actdev->cmni->handle == NULL)
        DEV_Error();
    void *handle = _actdev->cmni->handle;
    if(_actdev->cmni->protocol == I2C) {
        time5 = TIMER_getRunTimeus();
        if(_actdev->cmni->ware == SOFTWARE) {
#if defined(DEVICE_I2C_SOFTWARE_ENABLED)
            DEV_I2C_Write((I2C_ModuleHandleTypeDef *)handle, address, pdata, size, skip, 0x0);
#endif    // DEVICE_I2C_SOFTWARE_ENABLED
        } else if(_actdev->cmni->ware == HARDWARE) {
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
        time6 = TIMER_getRunTimeus();
    } else if(_actdev->cmni->protocol == SPI) {
        time7 = TIMER_getRunTimeus();
        if(_actdev->cmni->ware == SOFTWARE) {
#if defined(DEVICE_SPI_SOFTWARE_ENABLED)
            DEV_SPI_Transmit(((SPI_ModuleHandleTypeDef *)handle), pdata, size, skip, 0x0);
#endif    // DEVICE_SPI_SOFTWARE_ENABLED
        } else if(_actdev->cmni->ware == HARDWARE) {
            if(_actdevcmniio->CS.GPIOx != NULL) {
                DEVIO_WritePin_RESET(&_actdevcmniio->CS);
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
            if(_actdevcmniio->CS.GPIOx != NULL) {
                DEVIO_WritePin_SET(&_actdevcmniio->CS);
            }
        }
        time8 = TIMER_getRunTimeus();
    } else if(_actdev->cmni->protocol == ONEWIRE) {
        if(_actdev->cmni->ware == SOFTWARE) {
#if defined(DEVICE_ONEWIRE_SOFTWARE_ENABLED)
            DEV_ONEWIRE_Write((ONEWIRE_ModuleHandleTypeDef *)handle, pdata, size, skip, 0x0);
#endif    // DEVICE_ONEWIRE_SOFTWARE_ENABLED
        }
    }
}
void DEVCMNI_Read(uint8_t *pdata, uint16_t size, uint8_t address, bool skip) {
    if(_actdev == NULL || _actdevcmniio == NULL || _actdev->cmni == NULL || _actdev->cmni->handle == NULL)
        DEV_Error();
    void *handle = _actdev->cmni->handle;
    if(_actdev->cmni->protocol == I2C) {
        if(_actdev->cmni->ware == SOFTWARE) {
#if defined(DEVICE_I2C_SOFTWARE_ENABLED)
            DEV_I2C_Read((I2C_ModuleHandleTypeDef *)handle, address, pdata, size, skip, 0x0);
#endif    // DEVICE_I2C_SOFTWARE_ENABLED
        } else if(_actdev->cmni->ware == HARDWARE) {
#if defined(STM32HAL)
#if defined(HAL_I2C_MODULE_ENABLED)
            HAL_I2C_Mem_Read(((I2C_ModuleHandleTypeDef *)handle)->bushandle, (((I2C_ModuleHandleTypeDef *)handle)->addr << 1) | 0X00, address, I2C_MEMADD_SIZE_8BIT, pdata, size, 0x100);
#elif defined(HAL_FMPI2C_MODULE_ENABLED)
            HAL_FMPI2C_Mem_Read(((I2C_ModuleHandleTypeDef *)handle)->bushandle, (((I2C_ModuleHandleTypeDef *)handle)->addr << 1) | 0X00, address, FMPI2C_MEMADD_SIZE_8BIT, pdata, size, 0x100);
#endif    // HAL_I2C_MODULE_ENABLED | HAL_FMPI2C_MODULE_ENABLED
#elif defined(STM32FWLIBF1)
            //固件库的硬件I2C驱动函数,待补充
#endif
        }
    } else if(_actdev->cmni->protocol == SPI) {
        if(_actdev->cmni->ware == SOFTWARE) {
#if defined(DEVICE_SPI_SOFTWARE_ENABLED)
            DEV_SPI_Transmit(((SPI_ModuleHandleTypeDef *)handle), pdata, size, skip, 0x0);
#endif    // DEVICE_SPI_SOFTWARE_ENABLED
        } else if(_actdev->cmni->ware == HARDWARE) {
            if(_actdevcmniio->CS.GPIOx != NULL) {
                DEVIO_WritePin_RESET(&_actdevcmniio->CS);
            }
#if defined(STM32HAL)
#if defined(HAL_SPI_MODULE_ENABLED)
            HAL_SPI_Receive(((SPI_ModuleHandleTypeDef *)handle)->bushandle, pdata, size, 0x100);
#elif defined(HAL_QSPI_MODULE_ENABLED)
            //todo: SPI_Write for QSPI
#endif    // HAL_SPI_MODULE_ENABLED | HAL_QSPI_MODULE_ENABLED
#elif defined(STM32FWLIBF1)
            //固件库的硬件SPI驱动函数,待补充
#endif
            if(_actdevcmniio->CS.GPIOx != NULL) {
                DEVIO_WritePin_SET(&_actdevcmniio->CS);
            }
        }
    } else if(_actdev->cmni->protocol == ONEWIRE) {
        if(_actdev->cmni->ware == SOFTWARE) {
#if defined(DEVICE_ONEWIRE_SOFTWARE_ENABLED)
            DEV_ONEWIRE_Read((ONEWIRE_ModuleHandleTypeDef *)handle, pdata, size);
#endif    // DEVICE_ONEWIRE_SOFTWARE_ENABLED
        }
    }
}


/////////////////////////    设备初始化配置    /////////////////////////
void DEV_Confi(DEVS_TypeDef *devs, DEV_TypeDef dev[]) {
    if(DEV_Init(devs, dev) == 1) {    //初始化一类设备实例到设备池中
        DEV_Error();
    }
    DEV_ReCall(devs, DEVCMNI_IOInit);    //若设备实例有通信配置, 则初始化所有通信引脚
    return;
}
