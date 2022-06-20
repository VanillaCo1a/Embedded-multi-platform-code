#ifndef __QUEUE_ADT_H
#define __QUEUE_ADT_H
#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"
#include "stdbool.h"

typedef struct {
    uint8_t head;
    uint8_t tail;
    uint8_t maxnum;
    uint16_t *element;
} Queue_structure;

Queue_structure *createNewQueue(uint8_t);
int8_t isQueueNull(Queue_structure *Queue);
int8_t isQueueEmpty(Queue_structure *Queue);
int8_t isQueueFull(Queue_structure *Queue);
int8_t QueueIn(Queue_structure *Queue, uint16_t);
uint16_t QueueOut(Queue_structure *Queue);
int8_t QueueDel(Queue_structure *Queue);
uint16_t QueueCheckHead(Queue_structure *Queue);
uint16_t QueueCheckTail(Queue_structure *Queue);
int8_t QueueViewElement(Queue_structure *Queue);
int8_t QueueViewSpace(Queue_structure *Queue);

#endif
