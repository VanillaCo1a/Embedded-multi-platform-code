#include "queue_adt.h"

//************************ 循环队列ADT ************************//
Queue_structure *createNewQueue(void) {
    Queue_structure *Queue = malloc(sizeof(Queue_structure));
    Queue->head = 0;
    Queue->tail = 0;
    Queue->MAXNUM = 100;
    Queue->element = malloc(Queue->MAXNUM * sizeof(uint16_t));
    return Queue;
}
int8_t isQueueNull(Queue_structure *Queue) {
    if(Queue) {
        return 0;
    } else {
        return 1;
    }
}
int8_t isQueueEmpty(Queue_structure *Queue) {
    if(!isQueueNull(Queue)) {
        if(Queue->head == Queue->tail) {
            return 1;
        } else {
            return 0;
        }
    } else {
        return -1;
    }
}
int8_t isQueueFull(Queue_structure *Queue) {
    if(!isQueueNull(Queue)) {
        if((Queue->tail + 1) % Queue->MAXNUM == Queue->head) {
            return 1;
        } else {
            return 0;
        }
    } else {
        return -1;
    }
}
int8_t QueueIn(Queue_structure *Queue, uint16_t record) {
    int8_t flag = 0;
    if(isQueueFull(Queue)) {
        Queue->element[Queue->tail] = record;
        Queue->tail = (Queue->tail + 1) % Queue->MAXNUM;
        Queue->head = (Queue->head + 1) % Queue->MAXNUM;
        flag = 1;
    } else {
        Queue->element[Queue->tail] = record;
        Queue->tail = (Queue->tail + 1) % Queue->MAXNUM;
        flag = 0;
    }
    return flag;
}
uint16_t QueueOut(Queue_structure *Queue) {
    uint16_t record = 0;
    if(isQueueEmpty(Queue)) {
        return 0;
    } else {
        record = Queue->element[Queue->head];
        Queue->head = (Queue->head + 1) % Queue->MAXNUM;
        return record;
    }
}
int8_t QueueDel(Queue_structure *Queue) {
    if(isQueueEmpty(Queue)) {
        return 0;
    } else {
        Queue->head = (Queue->head + 1) % Queue->MAXNUM;
        return 1;
    }
}
//************************************************************//
