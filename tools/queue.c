#include <stdlib.h>
#include "queue.h"

static void CopyToNode(Item *pi, Node *pn);
static void CopyToItem(Node *pn, Item *pi);

/* 初始化队列 */
void InitializeQueue(Queue *pq, Node items[], size_t size) {
    pq->head = 0;
    pq->tail = 0;
    pq->size = size;
    pq->items = items;
}

/* 检查队列是否已满 */
bool QueueIsEmpty(const Queue *pq) {
    return (pq) && (pq->head == pq->tail);
}

/* 检查队列是否为空 */
bool QueueIsFull(const Queue *pq) {
    return (pq) && ((pq->tail + 1) % pq->size == pq->head);
}

/* 确定队列中的项数 */
size_t QueueItemCount(const Queue *pq) {
    if(pq == NULL) {
        return 0;
    }
    return (pq->tail - pq->head + pq->size) % pq->size;
}

/* 在队列末尾添加项 */
bool EnQueue(Queue *pq, Item *pitem) {
    if(pq == NULL) {
        return false;
    }
    CopyToNode(pitem, &pq->items[pq->tail]);
    if(QueueIsFull(pq)) {
        pq->tail = (pq->tail + 1) % pq->size;
        pq->head = (pq->head + 1) % pq->size;
    } else {
        pq->tail = (pq->tail + 1) % pq->size;
    }
    return true;
}

/* 从队列的开头删除项 */
bool DeQueue(Queue *pq, Item *pitem) {
    if((pq == NULL) || QueueIsEmpty(pq)) {
        return false;
    }
    CopyToItem(&pq->items[pq->head], pitem);
    pq->head = (pq->head + 1) % pq->size;
    return true;
}

/* 检查队列中的某项 */
bool CheckQueue(Queue *pq, Item *pitem, size_t num) {
    if((pq == NULL) || (num > pq->size)) {
        return false;
    }
    *pitem = pq->items[(pq->head + num) % pq->size].item;
    return true;
}

/* 清空队列 */
void EmptyTheQueue(Queue *pq) {
    Item dummy;
    while(!QueueIsEmpty(pq)) {
        DeQueue(pq, &dummy);
    }
}

/* 局部函数 */
static void CopyToNode(Item *pi, Node *pn) {
    pn->item = *pi;
}
static void CopyToItem(Node *pn, Item *pi) {
    *pi = pn->item;
}
