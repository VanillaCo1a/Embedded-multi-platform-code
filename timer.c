#include "timer.h"

////////////////////////////////////////////////////////////////////////////
uint64_t time_us = 0;
uint32_t time_ms = 0, time_s = 0;
uint64_t TIMER_getRunTimeus(void) {
    return time_us;
}
uint32_t TIMER_getRunTimems(void) {
    return time_ms;
}
uint32_t TIMER_getRunTimes(void) {
    return time_s;
}

////////////////////////////////////////////////////////////////////////////
/***    第一次调用函数时开始计时, 后续调用时返回比较结果, 延时未结束返回0, 结束则返回1
注意: 一次延时内在同一个函数不改变参数多次调用本函数, 或多次调用包含了本函数的函数, 
会造成静态变量重复使用导致的延时失灵的问题, 因此需要在程序段对应位置设置局部静态变量, 
相应的内存占用较多, 待优化

调用方法例:
    {static uint64_t compare = 0;   static int8_t state = 0;
    TIMER_delayus(10, &compare, &state);}
    {static uint32_t compare = 0;   static int8_t state = 0;
    TIMER_delayms(10, &compare, &state);}
    {static uint32_t compare = 0;   static int8_t state = 0;
    TIMER_delays(10, &compare, &state);}        ***/

//state1.输入:空
//获取当前时间, 记录当前时间, 记录part
//state2.输入:时间 返回:0/1(清零part)
//获取当前时间, 比较历史时间
int8_t TIMER_delayhus(uint16_t hus, uint32_t *compare_hus, int8_t *state) {
    if(*state == 0) {
        *compare_hus = time_us/100;
        (*state)++;
    }
    if(*state==1 && time_us/100-*compare_hus>=hus) {
        (*state)--;
        return 1;
    }
    return 0;
}
int8_t TIMER_delayms(uint16_t ms, uint32_t *compare_ms, int8_t *state) {
    if(*state == 0) {
        *compare_ms = time_ms;
        (*state)++;
    }
    if(*state==1 && time_ms-*compare_ms>=ms) {
        (*state)--;
        return 1;
    }
    return 0;
}
int8_t TIMER_delays(uint16_t s, uint32_t *compare_s, int8_t *state) {
    if(*state == 0) {
        *compare_s = time_s;
        (*state)++;
    }
    if(*state==1 && time_s-*compare_s>=s) {
        (*state)--;
        return 1;
    }
    return 0;
}

#if (MCU_COMPILER == MCU_STM32FWLIB)
void TIM4_IRQHandler(void) {
    if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
        time_us += 100;
        if((time_us%1000) == 0) {
            time_ms++;
            if((time_ms%1000) == 0) {
                time_s++;
            }
        }
    }
}

void TIM4_Confi(void) {
    #ifndef NVICGROUP
    System_NVIC_GroupInit();
    #endif
    TIM4_Init();
    TIM4_NVIC_Init();
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM4, ENABLE);
}
void TIM4_Init(void) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    TIM_TimeBaseInitStructure.TIM_Period = 99;
    TIM_TimeBaseInitStructure.TIM_Prescaler = 71;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);
}
void TIM4_NVIC_Init(void) {
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
#elif (MCU_COMPILER == MCU_STM32HAL)
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if(htim == &htim4) {
        time_us += 100;
        if((time_us%1000) == 0) {
            time_ms++;
            if((time_ms%1000) == 0) {
                time_s++;
            }
        }
    }
}
#endif
