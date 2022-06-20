#include "queue_adt.h"

//************************ 循环队列ADT ************************//
Queue_structure *createNewQueue(uint8_t MAXNUM) {
    Queue_structure *Queue = malloc(sizeof(Queue_structure));
    Queue->head = 0;
    Queue->tail = 0;
    Queue->maxnum = MAXNUM;
    Queue->element = malloc(Queue->maxnum * sizeof(uint16_t));
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
        if((Queue->tail + 1) % Queue->maxnum == Queue->head) {
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
    if(isQueueFull(Queue) == 1) {
        Queue->element[Queue->tail] = record;
        Queue->tail = (Queue->tail + 1) % Queue->maxnum;
        Queue->head = (Queue->head + 1) % Queue->maxnum;
        flag = 1;
    } else if(isQueueFull(Queue) == 0) {
        Queue->element[Queue->tail] = record;
        Queue->tail = (Queue->tail + 1) % Queue->maxnum;
        flag = 0;
    } else if(isQueueFull(Queue) == -1) {
        flag = -1;
    }
    return flag;
}
uint16_t QueueOut(Queue_structure *Queue) {
    uint16_t record = 0;
    if(isQueueEmpty(Queue) == 0) {
        record = Queue->element[Queue->head];
        Queue->head = (Queue->head + 1) % Queue->maxnum;
        return record;
    } else if(isQueueEmpty(Queue) == 0) {
        return 0;
    } else {
        return 0;
    }
}
int8_t QueueDel(Queue_structure *Queue) {
    if(isQueueEmpty(Queue) == 0) {
        QueueOut(Queue);
        return 1;
    } else if(isQueueEmpty(Queue) == 1) {
        return 0;
    } else {
        return 0;
    }
}
uint16_t QueueCheckHead(Queue_structure *Queue) {
    if(isQueueEmpty(Queue) == 0) {
        return Queue->element[Queue->head];
    } else {
        return 0;
    }
}
uint16_t QueueCheckTail(Queue_structure *Queue) {
    if(isQueueEmpty(Queue) == 0) {
        return Queue->element[(Queue->tail - 1 + Queue->maxnum) % Queue->maxnum];
    } else {
        return 0;
    }
}
int8_t QueueViewElement(Queue_structure *Queue) {
    if(isQueueEmpty(Queue) == 0) {
        return (Queue->tail - Queue->head + Queue->maxnum) % Queue->maxnum;
    } else {
        return 0;
    }
}
//************************************************************//
