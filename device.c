#include "device.h"
#include "timer.h"

#define DEVBUSY_MAXNUM 20             //忙设备数量上限
typedef uint8_t busylistsize;         //忙设备数量类型
uint64_t busylist[DEVBUSY_MAXNUM];    //忙设备表, 0为空, 非0为设备置忙时刻(us)
void BUSYLIST_Init(void) {
    for(busylistsize i = 0; i < DEVBUSY_MAXNUM; i++) {
        busylist[i] = 0;
    }
}

/**
 * @description: 初始化一类设备
 * @param {DEV_TypeDef} *self
 * @param {uint8_t} num
 * @return {*}
 */
int8_t DEV_Init(DEV_TypeDef *self, uint8_t num) {
#ifndef __BUSYLIST_INIT
#define __BUSYLIST_INIT
    BUSYLIST_Init();    //第一次初始化设备实例时也初始化忙设备表
#endif                  // !__BUSYLIST_INIT
    if(num > DEV_MAXNUM) {
        return 1;
    }
    self->_num = num;
    self->_stream = 0;
    for(devsize i = 0; i < DEV_MAXNUM; i++) {
        self->_state[i] = 0;
    }
    return 0;
}
/**
 * @description: 设置活动设备(0~最大值-1)
 * @param {DEV_TypeDef} *self
 * @param {uint8_t} num
 * @return {*}
 */
int8_t DEV_setiostream(DEV_TypeDef *self, uint8_t num) {
    if(self == NULL) {
        return 1;
    }
    if(num >= self->_num) {
        return 1;
    }
    self->_stream = num;
    return 0;
}
/**
 * @description: 获取活动设备
 * @param {DEV_TypeDef} *self
 * @return {*}
 */
uint8_t DEV_getiostream(DEV_TypeDef *self) {
    if(self == NULL) {
        return self->_num;
    }
    return self->_stream;
}
/**
 * @description: 设置活动设备状态, us>0表示设置设备忙时长(单位为10us),us=0表示设备闲
 * @param {DEV_TypeDef} *self
 * @param {uint16_t} us
 * @return {*}
 */
int8_t DEV_setState(DEV_TypeDef *self, uint16_t tus) {
    if(tus > 0 && self->_state[self->_stream] == 0) {
        for(busylistsize i = 0; i < DEVBUSY_MAXNUM; i++) {
            if(busylist[i] == 0) {
                self->_state[self->_stream] = i + 1;    //设置成功, 保存当前设备忙时间记录在忙设备列表的第几个
                busylist[i] = TIMER_getRunTimeus() + tus * 10;
                break;
            }
        }
        if(self->_state[self->_stream] == 0) {
            return 1;    //设备状态表已满, 设置设备忙失败
        }
    } else if(tus > 0 && self->_state[self->_stream] > 0) {
        busylist[self->_state[self->_stream] - 1] = TIMER_getRunTimeus() + tus * 10;
    } else if(tus == 0 && self->_state[self->_stream] > 0) {
        busylist[self->_state[self->_stream] - 1] = 0;
        self->_state[self->_stream] = 0;
    }
    return 0;
}
/**
 * @description: 获取活动设备状态, 先查询忙设备是否满足空闲条件, 然后更新状态并返回
 * @param {DEV_TypeDef} *self
 * @return {*}
 */
DEV_StateTypeDef DEV_getState(DEV_TypeDef *self) {
    if(self->_state[self->_stream] > 0 && TIMER_getRunTimeus() > busylist[self->_state[self->_stream] - 1]) {
        busylist[self->_state[self->_stream] - 1] = 0;    //设备忙指定时间后转为空闲状态
        self->_state[self->_stream] = 0;
    }
    //(self->_state[self->_stream] == 0) ? idle : busy;
    return (DEV_StateTypeDef)(self->_state[self->_stream] != idle);
}
