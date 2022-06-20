#include "uart.h"
#define DEBUG            0
#define Transmit_delay() delayus_timer(100)    //发信延迟, 波特率越低需要延时越长
#define Receive_delay()  delayus_timer(500)    //收信延迟, 波特率越低需要延时越长

const uint8_t tail1_inside[] = {'\0'};
const uint8_t tail1_outside[] = {'\r', '\n'};

int8_t flag_receive1 = 0;
int8_t numTransmitBuf1 = 0, numReceiveBuf1 = 0;
Queue_structure *strBuf1_Receive, *strBuf1_Transmit;

void UART1_createBufferArea(void) {    //建2个缓存区存放字符, 数据类型为循环队列
    strBuf1_Receive = createNewQueue();
    strBuf1_Transmit = createNewQueue();
}
void UART1_clearBufferArea(void) {    //清空对应的缓存区
    while(!isQueueEmpty(strBuf1_Receive)) {
        QueueDel(strBuf1_Receive);
    }
    while(!isQueueEmpty(strBuf1_Transmit)) {
        QueueDel(strBuf1_Transmit);
    }
}
void UART_Init(void) {
    USART1_Confi();
    UART1_createBufferArea();
}

//将1个字符串装入缓存区, 同时记录缓存区内字符串数量以待通信使用
int8_t writeBuf1_Transmit(uint8_t *record) {
    int16_t i = 0;
    if(record != NULL) {
        for(i = 0; *(record + i) != '\0'; i++) {
            QueueIn(strBuf1_Transmit, *(record + i));
        }
        QueueIn(strBuf1_Transmit, '\0');
        numTransmitBuf1++;

#if DEBUG
        printf("\r\nnumTransmitBuf1=%d\r\n", numTransmitBuf1);
        printf("strBuf1_Transmit=%d %d\r\n", strBuf1_Transmit->head, strBuf1_Transmit->tail);
        for(i = 0; i < strBuf1_Transmit->maxnum; i++) {
            printf(" %X", strBuf1_Transmit->element[i]);
        }
        printf("\r\n\r\n\r\n");
#endif
        return 1;
    }
    return 0;
}
//从缓存区读出1个字符串, 返回其地址, 若读取失败则返回NULL
//!!!注意此函数会申请内存空间, 使用后务必进行空间的释放!!!
uint8_t *readBuf1_Receive(void) {
    int16_t i = 0;
    uint8_t *record = malloc(100 * sizeof(uint8_t));    //在堆空间中申请内存大小有限
    if(!isQueueEmpty(strBuf1_Receive) && numReceiveBuf1) {
        Receive_delay();
        Receive_delay();    //这里不等待的话, 队列读入时就会在队尾多读一个乱码值, 待研究
#if DEBUG
        printf("\r\n\r\n\r\nnumReceiveBuf1=%d\r\n", numReceiveBuf1);
        printf("strBuf1_Receive=%d %d\r\n", strBuf1_Receive->head, strBuf1_Receive->tail);
        for(i = 0; i < strBuf1_Receive->maxnum; i++) {
            printf("%X ", strBuf1_Receive->element[i]);
        }
        printf("\r\n");
#endif

        for(i = 0; (record[i] = (uint8_t)QueueOut(strBuf1_Receive)) != '\0'; i++) {
        }
        numReceiveBuf1--;
#if DEBUG
        printf("\r\n");
        for(i = 0; i < strBuf1_Receive->maxnum; i++) {
            printf("%X|", record[i]);
        }
        printf("\r\n");
#endif

        return record;
    } else {
        free(record + i);
        return NULL;
    }
}
int8_t UART1_Transmit(void) {    //发信函数, 在缓存区被装填字符串后自动发送, 放在主函数中不断循环即可
    uint16_t record = 0;
    if(!isQueueEmpty(strBuf1_Transmit) && numTransmitBuf1) {
        if((record = QueueOut(strBuf1_Transmit)) != '\0') {
            USART_SendData(USART1, record);
            Transmit_delay();
        } else {
            USART_SendData(USART1, '\r');
            Transmit_delay();
            USART_SendData(USART1, '\n');
            Transmit_delay();
            numTransmitBuf1--;
        }
    }
    if(!isQueueEmpty(strBuf1_Transmit) && numTransmitBuf1) {    //当判断当次发送后缓存区为空时, 返回0
        return 1;
    } else {
        return 0;
    }
}
void UART1_Receive(void) {    //接收函数, 当串口收到数据时自动存入缓存区, 放在对应串口中断中即可
    uint16_t record = USART_ReceiveData(USART1);

    if(record == '\r') {
        if(flag_receive1 == 0) {
            flag_receive1 = 1;
        } else {
            flag_receive1 = 0;
        }
    } else if(record == '\n') {
        if(flag_receive1 == 1) {
            flag_receive1 = 0;
            QueueIn(strBuf1_Receive, '\0');
            numReceiveBuf1++;
        } else {
            flag_receive1 = 0;
        }
    } else {
        flag_receive1 = 0;
        if(record >= '!') {
            QueueIn(strBuf1_Receive, record);
            if(flag_receive1 != 0) {
                flag_receive1 = 0;
            }
        }
    }
}
