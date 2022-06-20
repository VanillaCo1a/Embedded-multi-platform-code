#include "uart.h"
#define QUEUESIZE         3000
#define DEBUG_UART        1
#define Transmit_delay(x) timer_delayus(x, &flag_comparetime, &flag_reset)    //发信单桢数据后等待延迟, 波特率越低需要延时越长, 115200下的延迟极限在80us左右
#define Receive_delay(x)  timer_delayms(x, &flag_comparetime, &flag_reset)    //多帧收信等待全部读入缓冲区延迟, 波特率越低需要延时越长, 若不等待, 从队列读入数据时就会在队尾多读一个乱码值. 推测不能一检测缓冲区有数据就读入, 否则会因读取太快导致错乱
//波特率为115200时的延时参数: Transmit_delay(100); Receive_delay(10);				波特率为9600时的延时参数: Transmit_delay(1500); Receive_delay(100);			其他情形可酌情修改

/* 输出后缀数组的第1个值为输出后缀长度 */
static uint8_t tail1_inside[] = {1, '\0'};
static uint8_t tail1_outside[] = {2, '\r', '\n'};
/* 队列缓冲区 */
Queue bufReceive1, bufTransmit1;
static Node nodesr1[QUEUESIZE + 1], nodest1[QUEUESIZE + 1];
static void arrPrint(uint8_t arr[], size_t size_t);
static void bufPrint(Queue *buf, size_t size_t);

//将通信串更改后缀后写入缓冲区, 以等待发信函数发信, 返回读取结果
bool UART_writeBuf(Queue *bufTransmit, uint8_t data[], size_t size, uint8_t *suffix_in, uint8_t *suffix_out) {
    size_t i = 0, j = 0;
    if(data == NULL) {
        return false;
    }
    for(i = 0; i < size; i++) {
        if(suffix_in != NULL && suffix_in[0] != 0) {
            /* 存在内部后缀, 进行后缀匹配 */
            if(data[i] == suffix_in[j + 1]) {
                j++;
                if(j == suffix_in[0]) {
                    /* 后缀匹配, 跳出循环 */
                    break;
                }
            } else {
                /* 后缀不匹配, 退回当次匹配的头部并入队一个字节 */
                i -= j;
                j = 0;
                EnQueue(bufTransmit, &data[i]);
            }
        } else {
            /* 不存在内部后缀, 入队至队列满 */
            EnQueue(bufTransmit, &data[i]);
        }
    }
    /* 内部后缀匹配成功, 添加外部后缀 */
    if(suffix_out != NULL && suffix_out[0] != 0) {
        for(j = 0; j < suffix_out[0]; j++) {
            EnQueue(bufTransmit, &suffix_out[j + 1]);
        }
    }
#if DEBUG_UART
    printf("\r\n----------bufTransmitStart = %d %d----------\r\n", bufTransmit->head, bufTransmit->tail);
    bufPrint(bufTransmit, size);
    printf("\r\n----------bufTransmitEnd----------\r\n");
#endif
    return true;
}

//根据设置的外部后缀从缓存区读出1个通信串, 并添加所设置的内部后缀, 返回读取结果
bool UART_readBuf(Queue *bufReceive, uint8_t data[], size_t size, uint8_t *suffix_in, uint8_t *suffix_out) {
    static int8_t flag_receive = 0;
    static int8_t delay_result = 0;
    size_t i = 0, j = 0, k = 0;
    if(!QueueIsEmpty(bufReceive)) {    //检测缓冲区不为空后等待一段时间再读入, 权宜之计
        flag_receive = 1;
        {
            static uint16_t flag_comparetime = 0;
            static int8_t flag_reset = 0;
            delay_result = Receive_delay(10);
        }
    }
    if((flag_receive == 1) && delay_result) {
        flag_receive = 0;
#if DEBUG_UART
        printf("\r\n----------bufReceiveStart = %d %d----------\r\n", bufReceive->head, bufReceive->tail);
        bufPrint(bufReceive, size);
        printf("\r\n----------bufReceiveEnd----------\r\n");
#endif
        /* 读取队列直至队空 */
        for(i = 0; !QueueIsEmpty(bufReceive); i++) {
            if(k == 0) {
                /* 出队一个字节 */
                DeQueue(bufReceive, &data[i]);
            } else {
                /* 对已出队部分继续匹配 */
                k--;
            }
            if(suffix_out != NULL && suffix_out[0] != 0) {
                /* 存在外部后缀, 进行后缀匹配 */
                if(data[i] == suffix_out[j + 1]) {
                    j++;
                    if(j == suffix_out[0]) {
                        /* 后缀匹配, 删除后缀, 跳出循环 */
                        for(k = 0; k < suffix_out[0]; k++) {
                            data[i - j + (k + 1)] = 0;
                        }
                        break;
                    }
                } else {
                    /* 后缀不匹配, 退回当次匹配的头部等待后续出队 */
                    i -= j;
                    k = j;
                    j = 0;
                }
            }
        }
        /* 外部后缀匹配成功, 添加内部后缀 */
        if(suffix_in != NULL && suffix_in[0] != 0) {
            for(k = 0; k < suffix_in[0]; k++) {
                data[i - j + (k + 1)] = suffix_in[k + 1];
            }
        }
#if DEBUG_UART
        printf("\r\n----------dataStart----------\r\n");
        arrPrint(data, size);
        printf("\r\n----------dataEnd----------\r\n");
#endif
        return true;
    } else {
        return false;
    }
}

void UART_Init(void) {
    USART1_Confi();
    InitializeQueue(&bufReceive1, nodesr1, sizeof(nodesr1) / sizeof(Node));
    InitializeQueue(&bufTransmit1, nodest1, sizeof(nodest1) / sizeof(Node));
}

uint8_t data[QUEUESIZE];    //串口接收数据
void UART1_Ctrl(void) {
    if(UART_readBuf(&bufReceive1, data, sizeof(data) / sizeof(uint8_t), tail1_inside, tail1_outside)) {
        UART_writeBuf(&bufTransmit1, data, sizeof(data) / sizeof(uint8_t), tail1_inside, tail1_outside);    //回显
        printf("%s", data);                                                                                 //justfortest
    }
}

int8_t UART1_Transmit(void) {    //发信函数, 在缓存区被装填字符串后自动发送, 放在主函数中不断循环即可
    static int8_t delay_choose = 0;
    uint8_t temp;
    if(delay_choose == 0) {
        if(!QueueIsEmpty(&bufTransmit1)) {
            DeQueue(&bufTransmit1, &temp);
            USART_SendData(USART1, temp);
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
    if(QueueIsEmpty(&bufTransmit1) && (delay_choose == 0)) {    //当判断当次发送后缓存区为空时, 返回1
        return 1;
    } else {
        return 0;
    }
}

void UART1_Receive(void) {    //接收函数, 当串口收到数据时自动存入缓存区, 放在对应串口中断中即可
    uint8_t temp = USART_ReceiveData(USART1);
    EnQueue(&bufReceive1, &temp);
}

static void arrPrint(uint8_t arr[], size_t size) {
    size_t i = 0;
    for(i = 0; i < size; i++) {
        printf("%02X|", arr[i]);
        if(i + 1 != size) {
            if((i + 1) % 50 == 0) {
                printf("\r\n");
            }
            if((i + 1) % 1000 == 0) {
                printf("----------------------------------------------------------------------------------------------------\r\n");
            }
        }
    }
    printf("\r\n");
    for(i = 0; i < size; i++) {
        printf("%c", arr[i]);
    }
}
static void bufPrint(Queue *buf, size_t size) {
    size_t i = 0;
    uint8_t arr[QUEUESIZE];
    for(i = 0; i < size; i++) {
        CheckQueue(buf, &arr[i], i);
    }
    arrPrint(arr, size);
}
