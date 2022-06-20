#ifndef _QUEUE_H_
#define _QUEUE_H_
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/* 配置Item类型的定义 */
#ifndef ITEM
#define ITEM
typedef uint8_t Item;
#endif    // !ITEM

#ifndef NODE
#define NODE
typedef struct node {
    Item item;
} Node;
#endif    // !NODE
typedef struct queue {
    size_t head;
    size_t tail;
    size_t size;
    Node *items;
} Queue;

/* 操作:        初始化队列                                           */
/* 前提条件:    pq指向一个队列                                       */
/*              items是存放队列空间的数组                            */
/*              size是最大项数                                   */
/* 后置条件:    队列被初始化为空                                     */
void InitializeQueue(Queue *pq, Node items[], size_t size);

/* 操作:        检查队列是否已满                                     */
/* 前提条件:    pq指向之前被初始化的队列                             */
/* 后置条件:    如果队列存在且已满则返回true, 否则返回false          */
bool QueueIsFull(const Queue *pq);

/* 操作:        检查队列是否为空                                     */
/* 前提条件:    pq指向之前被初始化的队列                             */
/* 后置条件:    如果队列存在且为空则返回true, 否则返回false          */
bool QueueIsEmpty(const Queue *pq);

/* 操作:        确定队列中的项数                                     */
/* 前提条件:    pq指向之前被初始化的队列                             */
/* 后置条件:    返回队列中的项数                                     */
size_t QueueItemCount(const Queue *pq);

/* 操作:        在队列末尾添加项                                     */
/* 前提条件:    pq指向之前被初始化的队列                             */
/*              pitem指向要被添加在队列末尾的项                      */
/* 后置条件:    如果队列存在, *pitem将被添加在队列的末尾,            */
/*              函数返回true; 否则, 队列不改变, 函数返回false        */
bool EnQueue(Queue *pq, Item *pitem);

/* 操作:        从队列的开头删除项                                   */
/* 前提条件:    pq指向之前被初始化的队列                             */
/* 后置条件:    如果队列存在且不为空, 队列首端的item将被拷贝到*pitem */
/*              中并被删除, 且函数返回true; 否则, 函数返回false      */
bool DeQueue(Queue *pq, Item *pitem);

/* 操作:        检查队列中的某项                                     */
/* 前提条件:    pq指向之前被初始化的队列                             */
/*              num是从零开始, 在队列项数的范围内的数字              */
/* 后置条件:    如果队列存在且不为空, 队列的第num项item将被拷贝到    */
/*              *pitem中, 且函数返回true; 否则, 函数返回false        */
bool CheckQueue(Queue *pq, Item *pitem, size_t num);

/* 操作:        清空队列                                             */
/* 前提条件:    pq指向之前被初始化的队列                             */
/* 后置条件:    队列被清空                                           */
void EmptyTheQueue(Queue *pq);

#endif
