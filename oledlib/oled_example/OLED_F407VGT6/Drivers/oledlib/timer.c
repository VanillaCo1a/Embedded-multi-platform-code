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
/***    ��һ�ε��ú���ʱ��ʼ��ʱ, ��������ʱ���رȽϽ��, ��ʱδ��������0, �����򷵻�1
ע��: һ����ʱ����ͬһ���������ı������ε��ñ�����, ���ε��ð����˱������ĺ���, 
����ɾ�̬�����ظ�ʹ�õ��µ���ʱʧ�������, �����Ҫ�ڳ���ζ�Ӧλ�����þֲ���̬����, 
��Ӧ���ڴ�ռ�ý϶�, ���Ż�

���÷�����:
    {static uint64_t compare = 0;   static int8_t state = 0;
    TIMER_delayus(10, &compare, &state);}
    {static uint32_t compare = 0;   static int8_t state = 0;
    TIMER_delayms(10, &compare, &state);}
    {static uint32_t compare = 0;   static int8_t state = 0;
    TIMER_delays(10, &compare, &state);}        ***/

//state1.����:��
//��ȡ��ǰʱ��, ��¼��ǰʱ��, ��¼part
//state2.����:ʱ�� ����:0/1(����part)
//��ȡ��ǰʱ��, �Ƚ���ʷʱ��
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
