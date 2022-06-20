#ifndef __TIMER_H
#define __TIMER_H
#include "head.h"

#define delays_timer(X)     {static uint32_t compare = 0;   static int8_t state = 0;	while(!TIMER_delays(X, &compare, &state))	;}
#define delayms_timer(X)    {static uint32_t compare = 0;   static int8_t state = 0;	while(!TIMER_delayms(X, &compare, &state))	;}
#define delayus_timer(X)    {static uint64_t compare = 0;   static int8_t state = 0;	while(!TIMER_delayus(X, &compare, &state))	;}
#define Delays_timer(X, Y)  {static uint32_t compare = 0;   static int8_t state = 0;    Y=TIMER_delays(X, &compare, &state)}
#define Delayms_timer(X, Y) {static uint32_t compare = 0;   static int8_t state = 0;	Y=TIMER_delayms(X, &compare, &state)}
#define Delayus_timer(X, Y) {static uint64_t compare = 0;   static int8_t state = 0;	Y=TIMER_delayus(X, &compare, &state)}

extern uint64_t time_us;
extern uint32_t time_ms, time_s;
uint64_t TIMER_getRunTimeus(void);
uint32_t TIMER_getRunTimems(void);
uint32_t TIMER_getRunTimes(void);

int8_t TIMER_delayus(uint16_t, uint64_t *, int8_t *);
int8_t TIMER_delayms(uint16_t, uint32_t *, int8_t *);
int8_t TIMER_delays(uint16_t, uint32_t *, int8_t *);
uint16_t timer_countms(uint16_t *, int8_t *);

void TIM4_Confi(void);
#endif
