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
int8_t TIMER_delayus(uint16_t us, uint64_t *compare_us, int8_t *state) {
    if(*state == 0) {
        *compare_us = time_us;
        (*state)++;
    }
    if(*state==1 && time_us-*compare_us>=us) {
        *state = 0;
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
        *state = 0;
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
        *state = 0;
        return 1;
    }
    return 0;
}

#if (MCU_COMPILER == MCU_STM32FWLIB)
void TIM4_IRQHandler(void) {
    if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);    //清除TIM4更新中断标志 
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
    //0.在程序开始时先设置中断优先级分组
    #ifndef NVICGROUP
    System_NVIC_GroupInit();
    #endif
    //1.调用配置定时器的函数
    TIM4_Init();
    //2.调用配置中断优先级的函数
    TIM4_NVIC_Init();
    //3.调用配置中断工作模式的库函数
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);    //设置中断的工作模式; 此处设置为使能'更新中断'模式
    //4.调用使能定时器的库函数
    TIM_Cmd(TIM4, ENABLE);    //使能/失能计数器计数; 此处使能计数器
}
void TIM4_Init(void) {
    //0.定义一个配置定时器的结构体变量
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    //1.初始化定时器时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    //2.1调用配置定时器工作模式的库函数
    /***    相比51单片机的定时器初值重装方法, 32的有几点不同, 
        stm32的定时器为16位自动重装模式, 频率可通过分频系数(0-65535)调节, 且重装的是末值(0-65535)而非初值, 于是:
            1.装填初值时的变量为终止步数
            2.装填初值时的分频系数可选, 默认下1个晶振周期即累加1次
            3.不需要软件重装初值
            4.不需要将初值分入2个8位定时器中
            因此, 51单片机中的装填公式由 待重装初值H=(65536-延时时间(us)/(12/晶振频率))/256 和 待重装初值L=(65536-延时时间(us)/(12/晶振频率))%256
            变化为 待重装初值 = 延时时间/((分频系数+1)/CK_PSC时钟频率)-1 也即 延时时间 = (待重装初值+1)*(分频系数+1)/CK_PSC时钟频率
            由公式, 可知定时范围可以从1*72/72=1(us)到65536*65536/72=59652323(us)≈59(s), 定时器的功能十分强悍    ***/
    TIM_TimeBaseInitStructure.TIM_Period = 99;    //装入初值; 此处装入99
    TIM_TimeBaseInitStructure.TIM_Prescaler = 71;    //装入预分频系数; 此处装入71
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;    //设置计数模式; 此处设置为向上计数模式
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;    //问题不大, 随便选一个填上去----视频上这么说道
    //2.2将配置好定时器设置的结构体变量传入初始化函数
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);
}
void TIM4_NVIC_Init(void) {
    //0.定义一个配置中断优先级的结构体变量
    NVIC_InitTypeDef NVIC_InitStructure;
    //1.1调用配置中断优先级的库函数
    //抢占优先级高的中断不仅可以优先执行, 而且可以打断抢占优先级低的中断执行中断; 
    //而同一抢占优先级下, 响应优先级高的中断只能在同时排队时优先触发, 不能打断响应优先级低的其它中断
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;    //设置要配置的中断通道; 此处配置的为定时器TIM4
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;    //设置中断的抢占优先级, 范围为0~2^抢占优先级位数-1(越小越优先)
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;    //设置中断的响应优先级, 范围为0~2^响应优先级位数-1
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    //使能/失能IRQ通道
    //1.2将配置好中断优先级设置的结构体传入初始化库函数
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
