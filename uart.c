#include "uart.h"
#define DEBUG_UART        0
#define Transmit_delay(x) delayus_timer(x, &flag_comparetime, &flag_reset)    //发信单桢数据后等待延迟, 波特率越低需要延时越长, 115200下的延迟极限在80us左右
#define Receive_delay(x)  delayms_timer(x, &flag_comparetime, &flag_reset)    //多帧收信等待全部读入缓冲区延迟, 波特率越低需要延时越长, 若不等待, 从队列读入数据时就会在队尾多读一个乱码值. 推测不能一检测缓冲区有数据就读入, 否则会因读取太快导致错乱
//波特率为115200时的延时参数: Transmit_delay(100); Receive_delay(10);				波特率为9600时的延时参数: Transmit_delay(1500); Receive_delay(100);			其他情形可酌情修改

/* 输出后缀数组的第1个值为输出后缀长度 */
const uint8_t tail1_inside[] = {'\0'};
const uint8_t tail1_outside[] = {'\r', '\n'};
/* 队列缓冲区 */
Queue_structure *strBuf1_Receive, *strBuf1_Transmit;

void UART_createBufferArea(int8_t num) {    //建2个缓存区存放字符, 数据类型为循环队列
    switch(num) {
    case 1:
        strBuf1_Receive = createNewQueue(100);
        strBuf1_Transmit = createNewQueue(100);
        break;
    }
}
void UART_clearBufferArea(int8_t num) {    //清空对应的缓存区
    switch(num) {
    case 1:
        while(!isQueueEmpty(strBuf1_Receive)) {
            QueueDel(strBuf1_Receive);
        }
        while(!isQueueEmpty(strBuf1_Transmit)) {
            QueueDel(strBuf1_Transmit);
        }
        break;
    }
}
void UART_Init(void) {
    USART1_Confi();
    UART_createBufferArea(1);
}

//将通信串更改后缀后写入缓冲区, 等待发信函数发信
int8_t writeBuf1_Transmit(uint8_t *record, uint8_t *suffix_in, uint8_t *suffix_out) {
    int16_t i = 0, j = 0;

    if(record != NULL) {
        for(i = 0; !isQueueFull(strBuf1_Transmit); i++) {
            if(suffix_in != NULL && suffix_in[0] != 0) {
                if(record[i] == suffix_in[j + 1]) {
                    j++;
                    if(j == suffix_in[0]) {
                        break;
                    }
                } else {
                    if(j != 0) {
                        i = i - j;
                        j = 0;
                    }
                    QueueIn(strBuf1_Transmit, record[i]);
                }
            } else {
                if((suffix_out != NULL) && (QueueViewSpace(strBuf1_Transmit) == suffix_out[0])) {
                    break;
                }
                QueueIn(strBuf1_Transmit, record[i]);
            }
        }
        if(suffix_out != NULL && suffix_out[0] != 0) {
            for(j = 0; j < suffix_out[0]; j++) {
                QueueIn(strBuf1_Transmit, suffix_out[j + 1]);
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
uint8_t *readBuf1_Receive(uint8_t *suffix_in, uint8_t *suffix_out) {
    static int8_t flag_receive = 0;
    static int8_t delay_result = 0;
    int16_t i = 0, j = 0, k = 0;
    uint8_t *record = malloc(strBuf1_Receive->maxnum * sizeof(uint8_t));    //在堆空间中申请有限大小内存

    if(isQueueEmpty(strBuf1_Receive) == 0) {    //检测缓冲区不为空后等待一段时间再读入, 权宜之计
        flag_receive = 1;
        {
            static uint16_t flag_comparetime = 0;
            static int8_t flag_reset = 0;
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
            if(suffix_out != NULL && suffix_out[0] != 0) {
                if(record[i] == suffix_out[j + 1]) {
                    j++;
                    if(j == suffix_out[0]) {
                        for(k = 0; k < suffix_out[0]; k++) {
                            record[i - j + (k + 1)] = 0;
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
        }
        if(suffix_in != NULL && suffix_in[0] != 0) {
            for(k = 0; k < suffix_in[0]; k++) {
                record[i - j + (k + 1)] = suffix_in[k + 1];
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
    if(delay_choose == 0) {
        if(!isQueueEmpty(strBuf1_Transmit)) {
            USART_SendData(USART1, QueueOut(strBuf1_Transmit));
            delay_choose = 1;
        }
    } else {
        {
            static uint16_t flag_comparetime = 0;
            static int8_t flag_reset = 0;
            if(Transmit_delay(100) == 1) {
                delay_choose = 0;
            }
        }
    }
    if(isQueueEmpty(strBuf1_Transmit) && (delay_choose == 0)) {    //当判断当次发送后缓存区为空时, 返回1
        return 1;
    } else {
        return 0;
    }
}
void UART1_Receive(void) {    //接收函数, 当串口收到数据时自动存入缓存区, 放在对应串口中断中即可
    QueueIn(strBuf1_Receive, USART_ReceiveData(USART1));
}
