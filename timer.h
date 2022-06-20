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

/* delay_timer(原地等待型), 常用于微秒级延时
输入: 延时时间  输出: 无 */
#define delayus_timer(TIME) ({while(!delayus_timer_paral(TIME));})
#define delayms_timer(TIME) ({while(!delayms_timer_paral(TIME));})
#define delays_timer(TIME) ({while(!delays_timer_paral(TIME));})
/* delay_timer_paral(查询终止型), 查询到延时结束后恢复至停止状态, 再次调用时才进行下次计时
输入: 延时时间  输出: 延时情况(1延时结束/0延时未结束) */
#define delayus_timer_paral(TIME) ({ static uint64_t compare = 0;   static int8_t state = 0; \
    TIMER_uscmptor(TIME, &compare, &state); })
#define delayms_timer_paral(TIME) ({ static uint64_t compare = 0;   static int8_t state = 0; \
    TIMER_mscmptor(TIME, &compare, &state); })
#define delays_timer_paral(TIME) ({ static uint64_t compare = 0;   static int8_t state = 0; \
    TIMER_scmptor(TIME, &compare, &state); })
/* delay_Timer_paral(查询重装型), 查询到延时结束后开始下次计时
输入: 延时时间(若为0则视为停止计时,返回0)  输出: 延时情况(1延时结束(查询后自动开始下一次计时)/0延时未结束) */
#define delayus_Timer_paral(TIME) ({ static uint64_t compare = 0;   static int8_t state = 0; \
    typeof(TIMER_uscmptor(TIME, &compare, &state)) result = TIMER_uscmptor(TIME, &compare, &state); \
    if(TIME==0) result = state = 0;     else if(result==1) TIMER_uscmptor(TIME, &compare, &state); \
    result; })
#define delayms_Timer_paral(TIME) ({ static uint64_t compare = 0;   static int8_t state = 0; \
    typeof(TIMER_mscmptor(TIME, &compare, &state)) result = TIMER_mscmptor(TIME, &compare, &state); \
    if(TIME==0) result = state = 0;     else if(result==1) TIMER_mscmptor(TIME, &compare, &state); \
    result; })
#define delays_Timer_paral(TIME) ({ static uint64_t compare = 0;   static int8_t state = 0; \
    typeof(TIMER_scmptor(TIME, &compare, &state)) result = TIMER_scmptor(TIME, &compare, &state); \
    if(TIME==0) result = state = 0;     else if(result==1) TIMER_scmptor(TIME, &compare, &state); \
    result; })

uint64_t TIMER_getRunTimeus(void);
uint32_t TIMER_getRunTimems(void);
uint32_t TIMER_getRunTimes(void);

int8_t TIMER_uscmptor(uint16_t, uint64_t *, int8_t *);
int8_t TIMER_mscmptor(uint16_t, uint64_t *, int8_t *);
int8_t TIMER_scmptor(uint16_t, uint64_t *, int8_t *);

void TIM4_Confi(void);
#endif
