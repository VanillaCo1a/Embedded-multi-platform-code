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
#define UARTSIZE 2000
static Item nodes[UARTNUM][2][UARTSIZE];
static volatile size_t nodesSize[UARTNUM][3] = {0};
static volatile bool signalOver[UARTNUM][2] = {1, 1};
/* 指定空间数据接收完毕/总线空闲时中断回调函数 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size) {
    if(huart == &huart1) {
        signalOver[0][0] = true;
        nodesSize[0][0] = size;
    }
}
/* 数据发送完毕中断回调函数 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if(huart == &huart1) {
        signalOver[0][1] = true;
    }
}
/* 根据标志位查询串口是否接收完成的函数
若否, 则设置下次接收数据的空间, 并返回false; 若是, 返回true */
bool LinkLayer_Receive(UART_HandleTypeDef *huart, size_t size) {
    if(huart == &huart1) {
        if(signalOver[0][0]) {
            signalOver[0][0] = false;
            nodesSize[0][2] = MIN(size, UARTSIZE);
            return true;
        }
    }
    return false;
}
/* 根据标志位查询串口是否发送完成的函数
若否, 则返回false; 若是, 则返回true */
bool LinkLayer_Transmit(UART_HandleTypeDef *huart) {
    if(huart == &huart1) {
        if(signalOver[0][1]) {
            signalOver[0][1] = false;
            return true;
        }
    }
    return false;
}

void LinkLayer_GetBuffer(Table *table) {
    AddTableBehind(table, nodes[0][0], nodesSize[0][0]);
    if(nodesSize[0][2] != 0) {
        HAL_UARTEx_ReceiveToIdle_IT(&huart1, nodes[0][0], nodesSize[0][2]);
    }
}
void LinkLayer_SetBuffer(Table *table) {
    size_t table_size = TableItemCount(table);
    nodesSize[0][1] = MIN(table_size, UARTSIZE);
    DelTableFront(table, nodes[0][1], nodesSize[0][1]);
    if(nodesSize[0][1] != 0) {
        HAL_UART_Transmit_IT(&huart1, nodes[0][1], nodesSize[0][1]);
    }
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
void upToLayer(uint8_t uart, void (*unpack)(Table *table)) {
    if(unpack == NULL) {
        return;
    }
    unpack(&_dataTable[0]);
}
void downToLayer(uint8_t uart, void (*pack)(Table *table)) {
    if(pack == NULL) {
        return;
    }
    pack(&_dataTable[1]);
}

void UART_Init(void) {
    InitializeTable(&_dataTable[0], _data[0], sizeof(_data[0]) / sizeof(Node));
    InitializeTable(&_dataTable[1], _data[1], sizeof(_data[1]) / sizeof(Node));
    printf("UART1初始化完毕\r\n");
}
size_t UART1_ScanArray(uint8_t arr[], size_t size) {
    size_t table_size = 0;
    /* 将待接收区域的空间设置为size, 但不大于BUFSIZE */
    if(LinkLayer_Receive(&huart1, MIN(size, BUFSIZE))) {
        upToLayer(1, LinkLayer_GetBuffer);
        upToLayer(1, TransportLayer_Unpack);
        upToLayer(1, ApplicationLayer_UnpackArray);
        /* 数据已经抵达应用层, 所占空间不大于之前设置的size */
        table_size = TableItemCount(&_dataTable[0]);
        DelTableFront(&_dataTable[0], arr, table_size);
        EmptyTheTable(&_dataTable[0]);
        return table_size;
    }
    return 0;
}
size_t UART1_ScanString(char *str, size_t size) {
    size_t table_size = 0;
    if(LinkLayer_Receive(&huart1, MIN(size - 1, BUFSIZE))) {
        upToLayer(1, LinkLayer_GetBuffer);
        upToLayer(1, TransportLayer_Unpack);
        upToLayer(1, ApplicationLayer_UnpackArray);
        table_size = TableItemCount(&_dataTable[0]);
        DelTableFront(&_dataTable[0], (Item *)str, table_size);
        str[table_size] = '\0';
        EmptyTheTable(&_dataTable[0]);
        return table_size;
    }
    return false;
}
bool UART1_PrintArray(uint8_t arr[], size_t size) {
    if(LinkLayer_Transmit(&huart1)) {
        /* 将待发送区域的空间设置为size, 超过BUFSIZE的部分将被弃置 */
        AddTableBehind(&_dataTable[1], arr, size);
        downToLayer(1, ApplicationLayer_PackArray);
        downToLayer(1, TransportLayer_Pack);
        downToLayer(1, LinkLayer_SetBuffer);
        EmptyTheTable(&_dataTable[1]);
        return true;
    }
    return false;
}
bool UART1_PrintString(char *str) {
    if(LinkLayer_Transmit(&huart1)) {
        AddTableBehind(&_dataTable[1], (Item *)str, strlen(str));
        downToLayer(1, ApplicationLayer_PackArray);
        downToLayer(1, TransportLayer_Pack);
        downToLayer(1, LinkLayer_SetBuffer);
        EmptyTheTable(&_dataTable[1]);
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
