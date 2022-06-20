#include "uart.h"
#define DEBUG_UART        0
#define Transmit_delay(x) delayhus_timer(x, &flag_comparetime, &flag_reset)    //发信单桢数据后等待延迟, 波特率越低需要延时越长, 115200下的延迟极限在80us左右
#define Receive_delay(x)  delayms_timer(x, &flag_comparetime, &flag_reset)     //多帧收信等待全部读入缓冲区延迟, 波特率越低需要延时越长, 若不等待, 从队列读入数据时就会在队尾多读一个乱码值. 推测不能一检测缓冲区有数据就读入, 否则会因读取太快导致错乱
//波特率为115200时的延时参数: Transmit_delay(1); Receive_delay(10);				波特率为9600时的延时参数: Transmit_delay(15); Receive_delay(100);			其他情形可酌情修改
const uint8_t tail1_inside[] = {'\0'};
const uint8_t tail1_outside[] = {'\r', '\n'};

int8_t numTransmitBuf1 = 0, numReceiveBuf1 = 0;
Queue_structure *strBuf1_Receive, *strBuf1_Transmit;

void UART1_createBufferArea(void) {    //建2个缓存区存放字符, 数据类型为循环队列
    strBuf1_Receive = createNewQueue(100);
    strBuf1_Transmit = createNewQueue(100);
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
void UART1_Ctrl(void) {
    uint8_t *temp;    //串口接收数据
    if((temp = readBuf1_Receive()) != NULL) {
        writeBuf1_Transmit(temp);    //回显
        //printf("%s", temp);	//justfortest
        free(temp);
    }
}

//将通信串更改后缀后写入缓冲区, 等待发信函数发信
int8_t writeBuf1_Transmit(uint8_t *record) {
    int16_t i = 0, j = 0;

    if(record != NULL) {
        for(i = 0; !isQueueFull(strBuf1_Transmit); i++) {
            if(record[i] == tail1_inside[j]) {
                j++;
                if(j == sizeof(tail1_inside) / sizeof(uint8_t)) {
                    for(j = 0; j < sizeof(tail1_outside) / sizeof(uint8_t); j++) {
                        QueueIn(strBuf1_Transmit, tail1_outside[j]);
                    }
                    break;
                }
            } else {
                if(j != 0) {
                    i = i - j;
                    j = 0;
                }
                QueueIn(strBuf1_Transmit, record[i]);
            }
        }
#if DEBUG_UART
        printf("\r\nstrBuf1_Transmit=%d %d\r\n", strBuf1_Transmit->head, strBuf1_Transmit->tail);
        for(i = 0; i < strBuf1_Transmit->maxnum; i++) {
            printf(" %X", strBuf1_Transmit->element[i]);
        }
        printf("\r\n\r\n\r\n");
#endif
        return 1;
    } else {
        return 0;
    }
}
//根据设置的外部后缀从缓存区读出1个通信串, 并添加所设置的内部后缀, 返回其地址, 若读取失败则返回NULL
//!!!注意此函数会申请内存空间, 使用后务必进行空间的释放!!!
uint8_t *readBuf1_Receive(void) {
    static int8_t flag_receive = 0;
    static int8_t delay_result = 0;
    int16_t i = 0, j = 0, k = 0;
    uint8_t *record = malloc(strBuf1_Receive->maxnum * sizeof(uint8_t));    //在堆空间中申请有限大小内存

    if(isQueueEmpty(strBuf1_Receive) == 0) {    //检测缓冲区不为空后等待一段时间再读入, 权宜之计
        flag_receive = 1;
        {
            static int16_t flag_comparetime = 0;
            static int8_t flag_reset = 1;
            delay_result = Receive_delay(10);
        }
    }
    if((flag_receive == 1) && delay_result) {
        flag_receive = 0;
        for(i = 0; isQueueEmpty(strBuf1_Receive) == 0; i++) {
            if(k == 0) {
                record[i] = (uint8_t)QueueOut(strBuf1_Receive);
            } else {
                k--;
            }
            if(record[i] == tail1_outside[j]) {
                j++;
                if(j == sizeof(tail1_outside) / sizeof(uint8_t)) {
                    for(k = 0; k < sizeof(tail1_outside) / sizeof(uint8_t); k++) {
                        record[i - (j - 1) + k] = 0;
                    }
                    for(k = 0; k < sizeof(tail1_inside) / sizeof(uint8_t); k++) {
                        record[i - (j - 1) + k] = tail1_inside[k];
                    }
                    break;
                }
            } else {
                if(j != 0) {
                    i = i - j;
                    k = j;
                    j = 0;
                }
            }
        }
#if DEBUG_UART
        printf("\r\n\r\n\r\nstrBuf1_Receive=%d %d\r\n", strBuf1_Receive->head, strBuf1_Receive->tail);
        for(i = 0; i < strBuf1_Receive->maxnum; i++) {
            printf("%X ", strBuf1_Receive->element[i]);
        }
        printf("\r\n");
#endif
#if DEBUG_UART
        printf("\r\n");
        for(i = 0; i < strBuf1_Receive->maxnum; i++) {
            printf("%X|", record[i]);
        }
        printf("\r\n");
#endif
        return record;
    } else {
        free(record);
        return NULL;
    }
}
int8_t UART1_Transmit(void) {    //发信函数, 在缓存区被装填字符串后自动发送, 放在主函数中不断循环即可
    static int8_t delay_choose = 0;
    if(!isQueueEmpty(strBuf1_Transmit)) {
        if(delay_choose == 0) {
            USART_SendData(USART1, QueueOut(strBuf1_Transmit));
            delay_choose = 1;
        } else {
            {
                static int16_t flag_comparetime = 0;
                static int8_t flag_reset = 1;
                if(Transmit_delay(1) == 1) {
                    delay_choose = 0;
                }
            }
        }
    }
    if(!isQueueEmpty(strBuf1_Transmit) && (delay_choose == 0)) {    //当判断当次发送后缓存区为空时, 返回0
        return 1;
    } else {
        return 0;
    }
}
void UART1_Receive(void) {    //接收函数, 当串口收到数据时自动存入缓存区, 放在对应串口中断中即可
    QueueIn(strBuf1_Receive, USART_ReceiveData(USART1));
}
