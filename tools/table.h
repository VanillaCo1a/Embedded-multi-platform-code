#ifndef _TABLE_H_
#define _TABLE_H_
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
typedef struct Table {
    size_t num;
    size_t size;
    Node *items;
} Table;

/* 操作:        初始化顺序表                                          */
/* 前提条件:    table指向一个顺序表                                     */
/*              items是存放顺序表空间的数组                           */
/*              size是最大项数                                       */
/* 后置条件:    顺序表被初始化为空                                    */
void InitializeTable(Table *table, Node items[], size_t size);

/* 检查顺序表是否已满 */
bool TableIsFull(const Table *table);

/* 检查顺序表是否为空 */
bool TableIsEmpty(const Table *table);

/* 操作:        确定顺序表中的项数                                     */
/* 前提条件:    table指向之前被初始化的顺序表                             */
/* 后置条件:    返回顺序表中的项数                                     */
size_t TableItemCount(const Table *table);

/* 在顺序表的开头添加项 */
bool AddTableFront(Table *table, Item pitem[], size_t size);

/* 在顺序表的末尾添加项 */
bool AddTableBehind(Table *table, Item pitem[], size_t size);

/* 从顺序表的开头删除项 */
bool DelTableFront(Table *table, Item pitem[], size_t size);

/* 从顺序表的末尾删除项 */
bool DelTableBehind(Table *table, Item pitem[], size_t size);

/* 检查顺序表中的某项 */
bool CheckTable(Table *table, Item *pitem, size_t num);

/* 清空顺序表 */
void EmptyTheTable(Table *table);

#endif
