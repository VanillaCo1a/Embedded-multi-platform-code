#include "uart.h"

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
    int8_t i = 0;
    if(*record != '\0') {
        for(i = 0; *(record + i) != '\0'; i++) {
            QueueIn(strBuf1_Transmit, *(record + i));
        }
        QueueIn(strBuf1_Transmit, '\0');
        numTransmitBuf1++;
        return 1;
    }
    return 0;
}
//从缓存区读出1个字符串, 返回其地址, 若读取失败则返回NULL
//!!!注意此函数会申请内存空间, 使用后务必进行空间的释放!!!
uint8_t *readBuf1_Receive(void) {
    int8_t i = 0;
    uint8_t *record = malloc(50 * sizeof(uint8_t));    //在堆空间中申请内存大小有限
    if(!isQueueEmpty(strBuf1_Receive) && numReceiveBuf1) {
        for(i = 0; (record[i] = (uint8_t)QueueOut(strBuf1_Receive)) != '\0'; i++) {
        }
        numReceiveBuf1--;
    }
    if(i) {
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
            delay_us(100);    //适当延迟,勿调
        } else {
            USART_SendData(USART1, '\r');
            delay_us(100);    //适当延迟,勿调
            USART_SendData(USART1, '\n');
            delay_us(100);    //适当延迟,勿调
            numTransmitBuf1--;
        }
    }
    if(!isQueueEmpty(strBuf1_Transmit) && numTransmitBuf1) {
        return 1;
    } else {
        return 0;
    }
}
void UART1_Receive(void) {    //接收函数, 当串口收到数据时自动存入缓存区, 放在对应串口中断中即可
    uint16_t record = 0;
    if((record = USART_ReceiveData(USART1)) >= '!') {
        QueueIn(strBuf1_Receive, record);
    } else {
        if(record == '\r') {
            QueueIn(strBuf1_Receive, '\0');
            numReceiveBuf1++;
            delay_us(500);    //适当延迟,勿调
        }
        record = USART_ReceiveData(USART1);    //将多余后缀如\r\n读入
    }
}
