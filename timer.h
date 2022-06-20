#ifndef __TIMER_H
#define __TIMER_H
#include "main.h"
#ifndef MCU_COMPILER
#define MCU_STM32FWLIB 0
#define MCU_STM32HAL 1
#define MCU_TIBOARD 2
#define MCU_ESP32 3
#define MCU_HC32 4
#define MCU_COMPILER    MCU_STM32HAL    //主控芯片编译环境
#endif

/* 宏函数, 查询式定时器延时, 在代码的每个调用处定义局部静态变量和标志用于记录、查询延时
使用了形如x=({1});的语法, 需要开启GNU扩展支持(C/C++ -> GNU extensions) */

/* delay_timer(单次调用型), 延时结束后恢复至停止状态, 下次调用时开始倒计时
输入: 延时时间  输出: 延时情况(1延时结束/0延时未结束) */
#define delayhus_timer(TIME) ({ static uint32_t compare = 0;   static int8_t state = 0; \
    TIMER_delayhus(TIME, &compare, &state); })
#define delayms_timer(TIME) ({ static uint32_t compare = 0;   static int8_t state = 0; \
    TIMER_delayms(TIME, &compare, &state); })
#define delays_timer(TIME) ({ static uint32_t compare = 0;   static int8_t state = 0; \
    TIMER_delays(TIME, &compare, &state); })
/* delay_Timer(自动重装型), 延时结束后自动开始下次倒计时
输入: 延时时间(若为0则重置停止计时并返回0)  输出: 延时情况(1延时结束(查询后自动开始下一次计时)/0延时未结束) */
#define delayhus_Timer(TIME) ({ static uint32_t compare = 0;   static int8_t state = 0; \
    typeof(TIMER_delayhus(TIME, &compare, &state)) result = TIMER_delayhus(TIME, &compare, &state); \
    if(TIME==0) result = 0;     else if(result==1) TIMER_delayhus(TIME, &compare, &state); \
    result; })
#define delayms_Timer(TIME) ({ static uint32_t compare = 0;   static int8_t state = 0; \
    typeof(TIMER_delayms(TIME, &compare, &state)) result = TIMER_delayms(TIME, &compare, &state); \
    if(TIME==0) result = 0;     else if(result==1) TIMER_delayms(TIME, &compare, &state); \
    result; })
#define delays_Timer(TIME) ({ static uint32_t compare = 0;   static int8_t state = 0; \
    typeof(TIMER_delays(TIME, &compare, &state)) result = TIMER_delays(TIME, &compare, &state); \
    if(TIME==0) result = 0;     else if(result==1) TIMER_delays(TIME, &compare, &state); \
    result; })

extern uint64_t time_us;
extern uint32_t time_ms, time_s;
uint64_t TIMER_getRunTimeus(void);
uint32_t TIMER_getRunTimems(void);
uint32_t TIMER_getRunTimes(void);

int8_t TIMER_delayhus(uint16_t, uint32_t *, int8_t *);
int8_t TIMER_delayms(uint16_t, uint32_t *, int8_t *);
int8_t TIMER_delays(uint16_t, uint32_t *, int8_t *);
uint16_t timer_countms(uint16_t *, int8_t *);

void TIM4_Confi(void);
#endif
