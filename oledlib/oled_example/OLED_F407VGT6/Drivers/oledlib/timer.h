#ifndef __TIMER_H
#define __TIMER_H
#include "main.h"
#ifndef MCU_COMPILER
#define MCU_STM32FWLIB 0
#define MCU_STM32HAL 1
#define MCU_TIBOARD 2
#define MCU_ESP32 3
#define MCU_HC32 4
#define MCU_COMPILER    MCU_STM32HAL    //����оƬ���뻷��
#endif

/* �꺯��, ��ѯʽ��ʱ����ʱ, �ڴ����ÿ�����ô�����ֲ���̬�����ͱ�־���ڼ�¼����ѯ��ʱ
ʹ��������x=({1});���﷨, ��Ҫ����GNU��չ֧��(C/C++ -> GNU extensions) */

/* delay_timer(���ε�����), ��ʱ������ָ���ֹͣ״̬, �´ε���ʱ��ʼ����ʱ
����: ��ʱʱ��  ���: ��ʱ���(1��ʱ����/0��ʱδ����) */
#define delayhus_timer(TIME) ({ static uint32_t compare = 0;   static int8_t state = 0; \
    TIMER_delayhus(TIME, &compare, &state); })
#define delayms_timer(TIME) ({ static uint32_t compare = 0;   static int8_t state = 0; \
    TIMER_delayms(TIME, &compare, &state); })
#define delays_timer(TIME) ({ static uint32_t compare = 0;   static int8_t state = 0; \
    TIMER_delays(TIME, &compare, &state); })
/* delay_Timer(�Զ���װ��), ��ʱ�������Զ���ʼ�´ε���ʱ
����: ��ʱʱ��(��Ϊ0������ֹͣ��ʱ������0)  ���: ��ʱ���(1��ʱ����(��ѯ���Զ���ʼ��һ�μ�ʱ)/0��ʱδ����) */
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
