#include <stdlib.h>
#include <string.h>
#include "table.h"

#define MIN(A, B) (((A) <= (B)) ? (A) : (B))
#define MAX(A, B) (((A) >= (B)) ? (A) : (B))

static void CopyToNode(Item *pi, Node *pn);
static void CopyToItem(Node *pn, Item *pi);

/* 初始化顺序表 */
void InitializeTable(Table *table, Node items[], size_t size) {
    table->num = 0;
    table->size = size;
    table->items = items;
}

/* 检查顺序表是否已满 */
bool TableIsFull(const Table *table) {
    return table->num == table->size;
}

/* 检查顺序表是否为空 */
bool TableIsEmpty(const Table *table) {
    return table->num == 0;
}

/* 确定顺序表中的项数 */
size_t TableItemCount(const Table *table) {
    if(table == NULL) {
        return 0;
    }
    return table->num;
}

/* 在顺序表的开头添加项 */
bool AddTableFront(Table *table, Item pitem[], size_t size) {
    size_t delta;
    if(table == NULL) {
        return false;
    }
    delta = MIN(size, table->size);
    table->num = MIN(table->num, table->size - delta);
    memmove(&table->items[delta], &table->items[0], table->num);
    for(size_t i = 0; i < delta; i++) {
        CopyToNode(&pitem[i], &table->items[i]);
    }
    table->num += delta;
    return true;
}

/* 在顺序表的末尾添加项 */
bool AddTableBehind(Table *table, Item pitem[], size_t size) {
    size_t delta;
    if(table == NULL || TableIsFull(table)) {
        return false;
    }
    delta = MIN(size, table->size - table->num);
    for(size_t i = 0; i < delta; i++) {
        CopyToNode(&pitem[i], &table->items[table->num + i]);
    }
    table->num += delta;
    return true;
}

/* 从顺序表的开头删除项 */
bool DelTableFront(Table *table, Item pitem[], size_t size) {
    size_t delta;
    if(table == NULL || TableIsEmpty(table)) {
        return false;
    }
    delta = MIN(size, table->num);
    for(size_t i = 0; i < delta; i++) {
        CopyToItem(&table->items[i], &pitem[i]);
    }
    memmove(&table->items[0], &table->items[delta], table->num - delta);
    table->num -= delta;
    return true;
}

/* 从顺序表的末尾删除项 */
bool DelTableBehind(Table *table, Item pitem[], size_t size) {
    size_t delta;
    if(table == NULL || TableIsEmpty(table)) {
        return false;
    }
    delta = MIN(size, table->num);
    for(size_t i = 0; i < delta; i++) {
        CopyToItem(&table->items[table->num - delta + i], &pitem[i]);
    }
    table->num -= delta;
    return true;
}

/* 检查顺序表中的某项 */
bool CheckTable(Table *table, Item *pitem, size_t num) {
    if((table == NULL) || (num > table->num)) {
        return false;
    }
    *pitem = table->items[num].item;
    return true;
}

/* 清空顺序表 */
void EmptyTheTable(Table *table) {
    table->num = 0;
}

/* 局部函数 */
static void CopyToNode(Item *pi, Node *pn) {
    pn->item = *pi;
}
static void CopyToItem(Node *pn, Item *pi) {
    *pi = pn->item;
}
