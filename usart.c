#include "usart.h"

/*********************************************************************	库函数方式复用GPIO	*********************************************************************/
/********************	哪些GPIO引脚可以复用为哪些内置外设需根据stm32型号自行查找数据手册, 如ZET6可见《STM32中文参考手册 V10》P109, P116~P121	********************/
/***************************	复用GPIO的工作模式(GPIO_InitStructure.GPIO_Mode)的配置是相对通用的, 可见《STM32中文参考手册 V10》P110	****************************/
void USART1_GPIO_ReuseInit(void) {
    //0.定义一个配置GPIO的结构体
    GPIO_InitTypeDef GPIO_InitStructure;
    //1.1初始化需复用GPIO的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);    //设置需要开启的APB2时钟; 此处设置GPIOA的时钟开启
    //1.2初始化需复用外设的对应时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);    //设置需要开启的APB2时钟; 此处设置串口1的时钟开启
    //2.1调用配置需复用GPIO的工作模式的函数
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;            //初始化需复用的引脚; 此处初始化GPIO的9引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;      //设置需复用GPIO的工作模式; 此处设置需复用为USART1_TX, 故设置为'复用推挽输出'
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    //设置需复用GPIO的工作频率; 此处设置为50Mhz
    //2.2将配置好GPIO设置的结构体传入初始化函数
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //同一总线下可能有多个引脚需要复用, 重复2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;               //初始化需复用的引脚; 此处初始化GPIO的10引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    //设置需复用GPIO的工作模式; 此处设置需复用为USART1_RX, 故设置为'浮空输入'
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}
/******************************************	函数效果: GPIOA.2和GPOIA.3作为串口1的TX和RX引脚使用, 复用初始化和配置完成	******************************************/

/*************************************************************	配置串口通讯USART_Init库函数	*******************************************************************/
void USART1_Init(void) {
    //0.定义一个配置USART的结构体
    USART_InitTypeDef USART_InitStructure;    //如果前文已经定义过, 则不需要
    //1.初始化USART时钟
    //RCC_APB1PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//设置需要开启的APB1/APB2时钟, USART1使用APB2时钟, USART2,USART3使用APB1时钟; 此处设置USART1的时钟开启
    //2.1调用配置USART工作方式的库函数
    USART_InitStructure.USART_BaudRate = 115200;                                       //设置串口通讯的波特率; 此处设置为115200
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                        //设置单帧字长; 此处设置为8位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                             //设置停止位位数; 此处设置为1位
    USART_InitStructure.USART_Parity = USART_Parity_No;                                //设置是否需要进行奇偶校验; 此处设置为无奇偶校验位
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;                    //设置发送(Tx)和接收(Rx)端是否使能; 此处设置为均使能
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;    //设置硬件流有效性; 此处设置为不使用硬件流
    //2.2将配置好串口设置的结构体变量传入初始化函数
    USART_Init(USART1, &USART_InitStructure);
}
/**************************************************************	函数效果: 串口1的初始化与配置完成	**************************************************************/

void USART1_NVIC_Init(void) {
    //0.定义一个配置中断优先级的结构体变量
    NVIC_InitTypeDef NVIC_InitStructure;
    //1.1调用配置中断优先级的函数
    //抢占优先级高的中断不仅可以优先执行, 而且可以打断抢占优先级低的中断执行中断;
    //而同一抢占优先级下, 响应优先级高的中断只能在同时排队时优先触发, 不能打断响应优先级低的其它中断
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;            //设置要配置的中断通道; 此处配置的为串口USART1
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;    //设置中断的抢占优先级, 范围为0~2^抢占优先级位数-1(越小越优先)
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;           //设置中断的响应优先级, 范围为0~2^响应优先级位数-1
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;              //使能/失能IRQ通道
    //1.2将配置好中断优先级设置的结构体传入初始化函数
    NVIC_Init(&NVIC_InitStructure);
}
/****************************	函数效果: 将定时器USART1的中断优先级设置为2级抢占优先级,2级响应优先级	********************************************/

void USART1_Confi(void) {
    //0.(如果需要串口中断)在程序开始时先设置中断优先级分组
    //System_NVIC_GroupInit();
    //1.调用配置引脚复用/重映射的函数, 初始化USART对应引脚
    USART1_GPIO_ReuseInit();    //初始化需要配置的USART对应引脚; 此处初始化USART1的默认引脚复用功能
    //2.调用配置串口通讯的函数
    USART1_Init();
    //3.(如果需要串口中断)调用配置中断优先级的函数
    USART1_NVIC_Init();
    //4.(如果需要串口中断)调用配置中断工作模式的库函数
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);    //设置中断的工作模式; 此处设置串口1为'当接收缓存区非空时触发中断'
    //5.调用使能串口的库函数
    USART_Cmd(USART1, ENABLE);    //使能/失能串口通讯; 此处使能串口1
}

void USART1_IRQHandler(void) {
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
        UART1_Receive();
    }
}
