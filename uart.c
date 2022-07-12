#include "queue.h"
#include "table.h"
#include "timer.h"
#include "uart.h"
#include "usart.h"

/* 宏函数 */
#define MIN(A, B) ((A) < (B)) ? (A) : (B)
#define MAX(A, B) ((A) > (B)) ? (A) : (B)

/* 开启调试输出 */
// #define DEBUG_UART
void PrintArray(uint8_t arr[], size_t size_t);
void PrintQueue(Queue *queue, size_t size_t);
bool CopyQueueTable(Queue *pq, Table *table, bool direct);

/* printf重定向 */
int fputc(int ch, FILE *f) {
    /* 这里需要加一个volatile, 不然会被AC6编译优化掉 */
    volatile bool res = 0;
    while(!(res = UART1_PrintArray((uint8_t *)&ch, 1))) continue;
    return ch;
}
int _write(int fd, char *pBuffer, int size) {
    volatile bool res = 0;
    while(!(res = UART1_PrintArray((uint8_t *)pBuffer, size))) continue;
    return size;
}


/*****                  物理层&数据链路层                   *****/
/* 定义缓冲区长度和数组下标 */
#define UARTSIZE 2000
#define RECEIVE  0
#define TRANSMIT 1
typedef struct uartbuffer {
    Item buf[2][UARTSIZE];
    size_t size[2];
    size_t count[2];
    bool signal[2];
} UartBuffer;
static UartBuffer buart1 = {.signal = {0, 1}};
static USART_TypeDef *acthuart;

static UartBuffer *UART_GetBuffer(USART_TypeDef *USARTx) {
    if(USARTx == USART1) {
        return &buart1;
    }
    return NULL;
}
/* 指定空间数据接收完毕/总线空闲时中断回调函数 */
void FWLIB_UARTEx_RxEventCallback(USART_TypeDef *USARTx, uint16_t size) {
    UartBuffer *buart = UART_GetBuffer(USARTx);
    buart->signal[RECEIVE] = true;
    buart->size[RECEIVE] = size;
}
/* 数据发送完毕中断回调函数 */
void FWLIB_UART_TxCpltCallback(USART_TypeDef *USARTx) {
    UartBuffer *buart = UART_GetBuffer(USARTx);
    buart->signal[TRANSMIT] = true;
}

/* 根据标志位查询串口是否收发完成的函数
若否, 则返回false; 若是, 则返回true */
static bool LinkLayer_UartReady(uint8_t type) {
    UartBuffer *buart = UART_GetBuffer(acthuart);
    if(buart->signal[type]) {
        buart->signal[type] = false;
        return true;
    }
    return false;
}
/* 开始串口接收 */
static void LinkLayer_ReceiveStart(size_t size) {
    UartBuffer *buart = UART_GetBuffer(acthuart);
    size_t _size = MIN(size, UARTSIZE);
    if(_size != 0) {
        FWLIB_UARTEx_ReceiveToIdle_IT(acthuart, buart->buf[RECEIVE], _size);
    }
}
/* 开始串口发送 */
static void LinkLayer_TransmitStart(void) {
    UartBuffer *buart = UART_GetBuffer(acthuart);
    if(buart->size[TRANSMIT] != 0) {
        FWLIB_UART_Transmit_IT(acthuart, buart->buf[TRANSMIT], buart->size[TRANSMIT]);
    }
}

void LinkLayer_GetBuffer(Table *table) {
    UartBuffer *buart = UART_GetBuffer(acthuart);
    AddTableBehind(table, buart->buf[RECEIVE], buart->size[RECEIVE]);
}
void LinkLayer_SetBuffer(Table *table) {
    UartBuffer *buart = UART_GetBuffer(acthuart);
    size_t table_size = TableItemCount(table);
    buart->size[TRANSMIT] = MIN(table_size, UARTSIZE);
    DelTableFront(table, buart->buf[TRANSMIT], buart->size[TRANSMIT]);
}


/*****                  传输层                  *****/

/* 前后缀结构体定义 */
typedef struct presuffix {
    uint8_t size;
    char str[9];
} PreSuffix;
/* 前后缀定义 */
// static PreSuffix presuffix[][2] = {{{1, {"\0"}}, {2, {"\r\n"}}}};
void changePreSuffix(uint8_t data[], size_t size, PreSuffix *affixin, PreSuffix *affixout) {
    size_t i = 0, j = 0;
    for(i = 0; i < size; i++) {
        if(affixin != NULL && affixin->size != 0) {
            /* 存在内部后缀, 进行后缀匹配 */
            if(data[i] == affixin->str[j]) {
                j++;
                if(j == affixin->size) {
                    /* 内部后缀匹配成功, 替换为外部后缀 */
                    i -= j;
                    j = 0;
                    if(affixout != NULL && affixout->size != 0) {
                        /* tofix: 这里将覆盖后缀后的下一字符串, 且默认不会发生数组越界 */
                        for(j = 0; j < affixout->size; j++) {
                            data[i + j] = affixout->str[j];
                        }
                    }
                }
            } else {
                /* 后缀不匹配, 退回当次匹配的头部并入队一个字节 */
                i -= j;
                j = 0;
            }
        }
    }
}

void TransportLayer_Unpack(Table *table) {
}

void TransportLayer_Pack(Table *table) {
}


/*****                  应用层                  *****/

void ApplicationLayer_UnpackArray(Table *table) {
}

void ApplicationLayer_PackArray(Table *table) {
}
void ApplicationLayer_PackComputer(Table *table) {
}


/*****                  用户调用                  *****/
Node _data[2][BUFSIZE];
Table _dataTable[2];
void upToLayer(void (*unpack)(Table *table)) {
    if(unpack == NULL) {
        return;
    }
    unpack(&_dataTable[RECEIVE]);
}
void downToLayer(void (*pack)(Table *table)) {
    if(pack == NULL) {
        return;
    }
    pack(&_dataTable[TRANSMIT]);
}

void UART_Init(void) {
    USART1_Confi();
    InitializeTable(&_dataTable[RECEIVE], _data[RECEIVE], sizeof(_data[RECEIVE]) / sizeof(Node));
    InitializeTable(&_dataTable[TRANSMIT], _data[TRANSMIT], sizeof(_data[TRANSMIT]) / sizeof(Node));
    printf("UART1初始化完毕\r\n");
}
size_t UART1_ScanArray(uint8_t arr[], size_t size) {
    size_t table_size = 0;
    acthuart = USART1;
    /* 将待接收区域的空间设置为size, 但不大于BUFSIZE */
    if(!LinkLayer_UartReady(RECEIVE)) {
        LinkLayer_ReceiveStart(MIN(size, BUFSIZE));
    } else {
        upToLayer(LinkLayer_GetBuffer);
        upToLayer(TransportLayer_Unpack);
        upToLayer(ApplicationLayer_UnpackArray);
        /* 数据已经抵达应用层, 所占空间不大于之前设置的size */
        table_size = TableItemCount(&_dataTable[RECEIVE]);
        DelTableFront(&_dataTable[RECEIVE], arr, table_size);
        EmptyTheTable(&_dataTable[RECEIVE]);
        return table_size;
    }
    return 0;
}
size_t UART1_ScanString(char *str, size_t size) {
    size_t table_size = 0;
    acthuart = USART1;
    if(!LinkLayer_UartReady(RECEIVE)) {
        LinkLayer_ReceiveStart(MIN(size - 1, BUFSIZE));
    } else {
        upToLayer(LinkLayer_GetBuffer);
        upToLayer(TransportLayer_Unpack);
        upToLayer(ApplicationLayer_UnpackArray);
        table_size = TableItemCount(&_dataTable[RECEIVE]);
        DelTableFront(&_dataTable[RECEIVE], (Item *)str, table_size);
        str[table_size] = '\0';
        EmptyTheTable(&_dataTable[RECEIVE]);
        return table_size;
    }
    return false;
}
bool UART1_PrintArray(uint8_t arr[], size_t size) {
    acthuart = USART1;
    if(LinkLayer_UartReady(TRANSMIT)) {
        /* 将待发送区域的空间设置为size, 超过BUFSIZE的部分将被弃置 */
        AddTableBehind(&_dataTable[TRANSMIT], arr, size);
        downToLayer(ApplicationLayer_PackArray);
        downToLayer(TransportLayer_Pack);
        downToLayer(LinkLayer_SetBuffer);
        LinkLayer_TransmitStart();
        EmptyTheTable(&_dataTable[TRANSMIT]);
        return true;
    }
    return false;
}
bool UART1_PrintString(char *str) {
    acthuart = USART1;
    if(LinkLayer_UartReady(TRANSMIT)) {
        AddTableBehind(&_dataTable[TRANSMIT], (Item *)str, strlen(str));
        downToLayer(ApplicationLayer_PackArray);
        downToLayer(TransportLayer_Pack);
        downToLayer(LinkLayer_SetBuffer);
        LinkLayer_TransmitStart();
        EmptyTheTable(&_dataTable[TRANSMIT]);
        return true;
    }
    return false;
}

/* 在队列和线性表中交换数据 */
bool CopyQueueTable(Queue *pq, Table *table, bool direct) {
    Item byte;
    if(pq == NULL || table == NULL) {
        return false;
    }
#ifdef DEBUG_UART
    printf("\r\n----------queueStart = %d %d----------\r\n", pq->head, pq->tail);
    PrintQueue(pq, size);
    printf("\r\n----------queueEnd----------\r\n");
#endif
    if(direct) {
        while(DelTableFront(table, &byte, 1)) {
            EnQueue(pq, &byte);
        }
    } else {
        while(DeQueue(pq, &byte)) {
            AddTableBehind(table, &byte, 1);
        }
    }
#ifdef DEBUG_UART
    printf("\r\n----------queueStart = %d %d----------\r\n", pq->head, pq->tail);
    PrintQueue(pq, size);
    printf("\r\n----------queueEnd----------\r\n");
    printf("\r\n----------dataStart----------\r\n");
    PrintArray(data, size);
    printf("\r\n----------dataEnd----------\r\n");
#endif
    return true;
}
/* 调试输出, 前提为printf函数正常工作 */
void PrintArray(uint8_t arr[], size_t size) {
    size_t i = 0;
    for(i = 0; i < size; i++) {
        printf("%02X|", arr[i]);
        if(i + 1 != size) {
            if((i + 1) % 50 == 0) {
                printf("\r\n");
            }
            if((i + 1) % 1000 == 0) {
                printf("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \r\n");
            }
        }
    }
    printf("\r\n");
    for(i = 0; i < size; i++) {
        printf("%c", arr[i]);
    }
}
void PrintQueue(Queue *queue, size_t size) {
    size_t i = 0;
    uint8_t buf;
    for(i = 0; i < size; i++) {
    }
    for(i = 0; i < size; i++) {
        CheckQueue(queue, &buf, i);
        if(i == 0) {
            printf(" ---> ");
        }
        if(i == QueueItemCount(queue)) {
            printf(" <--- ");
        }
        printf("%02X|", buf);
        if(i + 1 != size) {
            if((i + 1) % 50 == 0) {
                printf("\r\n");
            }
            if((i + 1) % 1000 == 0) {
                printf("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \r\n");
            }
        }
    }
    printf("\r\n");
    for(i = 0; i < size; i++) {
        CheckQueue(queue, &buf, i);
        printf("%c", buf);
    }
}
