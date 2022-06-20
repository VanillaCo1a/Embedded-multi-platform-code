#ifndef __TIMER_H
#define __TIMER_H
#include "device.h"
#define USEMARCO 1
#if MCU_COMPILER == MCU_STM32HAL
#define TimerError 0    //为了修正函数运行带来的误差, 在使用不同型号芯片前需调试计算得到数值
#endif

////////////////////////////////////////////////////////////////////////////
static inline uint64_t TIMER_getRunTimeus(void) {
    extern volatile uint64_t time_us;
    extern volatile int8_t flag_timerrupt;
    uint64_t time = 0;
    flag_timerrupt = 1;    //若定时器更新中断触发于下条语句过程中, 则再执行一次
    time = time_us + (TIM4->CNT) % 1000;
    if(flag_timerrupt == 0) {
        time = time_us + (TIM4->CNT) % 1000;
    }
    return time;
}
uint32_t TIMER_getRunTimems(void);
uint32_t TIMER_getRunTimes(void);
////////////////////////////////////////////////////////////////////////////
/* 宏函数, 查询式定时器延时, 在代码的每个调用处定义局部静态变量和标志用于记录、查询延时
使用了形如x=({1});的语法, 需要开启GNU扩展支持(C/C++ -> GNU extensions) */
/* delay_timer(原地等待型)
输入: 延时时间  输出: 无 */
#define delayus_timer(TIME) ({while(!delayus_timer_paral(TIME)); })
#define delayms_timer(TIME) ({while(!delayms_timer_paral(TIME)); })
#define delays_timer(TIME) ({while(!delays_timer_paral(TIME)); })
/* delay_timer_paral(查询终止型), 查询到延时结束后恢复至停止状态, 再次调用时才进行下次计时
输入: 延时时间  输出: 延时情况(1延时结束/0延时未结束) */
#define delayus_timer_paral(TIME) ({ static volatile uint64_t compare = 0;   static volatile int8_t state = 0; \
    TIMER_uscmptor(TIME, &compare, &state); })
#define delayms_timer_paral(TIME) ({ static volatile uint64_t compare = 0;   static volatile int8_t state = 0; \
    TIMER_mscmptor(TIME, &compare, &state); })
#define delays_timer_paral(TIME) ({ static volatile uint64_t compare = 0;   static volatile int8_t state = 0; \
    TIMER_scmptor(TIME, &compare, &state); })
/* delay_Timer_paral(查询重装型), 查询到延时结束后开始下次计时
输入: 延时时间(若为0则视为停止计时,返回0)  输出: 延时情况(1延时结束(查询后自动开始下一次计时)/0延时未结束) */
#define delayus_Timer_paral(TIME) ({ static volatile uint64_t compare = 0;   static volatile int8_t state = 0; \
    typeof(TIMER_uscmptor(TIME, &compare, &state)) result = TIMER_uscmptor(TIME, &compare, &state); \
    if(TIME==0) result = state = 0;     else if(result==1) TIMER_uscmptor(TIME, &compare, &state); \
    result; })
#define delayms_Timer_paral(TIME) ({ static volatile uint64_t compare = 0;   static volatile int8_t state = 0; \
    typeof(TIMER_mscmptor(TIME, &compare, &state)) result = TIMER_mscmptor(TIME, &compare, &state); \
    if(TIME==0) result = state = 0;     else if(result==1) TIMER_mscmptor(TIME, &compare, &state); \
    result; })
#define delays_Timer_paral(TIME) ({ static volatile uint64_t compare = 0;   static volatile int8_t state = 0; \
    typeof(TIMER_scmptor(TIME, &compare, &state)) result = TIMER_scmptor(TIME, &compare, &state); \
    if(TIME==0) result = state = 0;     else if(result==1) TIMER_scmptor(TIME, &compare, &state); \
    result; })

//100%的情况下精度在5us内, 99.8%的情况下精度在1us内, 当比较器遇到定时器更新中断时误差增大
#if USEMARCO
#define TIMER_uscmptor(us, compare, state) ({        \
    extern volatile uint64_t time_us;                \
    extern volatile int8_t flag_timerrupt;           \
    uint64_t time = 0;                               \
    int8_t result = 0;                               \
    if(*(state) == 0) {                              \
        flag_timerrupt = 1;                          \
        *(compare) = time_us + TIM4->CNT;   \
        if(flag_timerrupt == 0) {                    \
            *compare = time_us + TIM4->CNT; \
        }                                            \
        (*(state))++;                                \
    }                                                \
    flag_timerrupt = 1;                              \
    time = time_us + TIM4->CNT;             \
    if(flag_timerrupt == 0) {                        \
        time = time_us + TIM4->CNT;         \
    }                                                \
    if(time + TimerError >= *compare + us) {         \
        (*(state))--;                                \
        result = 1;                                  \
    }                                                \
    result;                                          \
})
#else
static inline int8_t TIMER_uscmptor(uint16_t us, volatile uint64_t *compare, volatile int8_t *state) {
    extern volatile uint64_t time_us;
    extern volatile int8_t flag_timerrupt;
    uint64_t time = 0;
    int8_t result = 0;
    if(*state == 0) {
        flag_timerrupt = 1;
        *compare = time_us + TIM4->CNT;
        if(flag_timerrupt == 0) {
            *compare = time_us + TIM4->CNT;
        }
        (*state)++;
    }
    flag_timerrupt = 1;
    time = time_us + TIM4->CNT;
    if(flag_timerrupt == 0) {
        time = time_us + TIM4->CNT;
    }
    if(time + TimerError >= *compare + us) {
        (*state)--;
        result = 1;
    }
    return result;
}
#endif    // USEMARCO
int8_t TIMER_mscmptor(uint16_t, volatile uint64_t *, volatile int8_t *);
int8_t TIMER_scmptor(uint16_t, volatile uint64_t *, volatile int8_t *);

void TIM4_Confi(void);

#undef USEMARCO
#endif
