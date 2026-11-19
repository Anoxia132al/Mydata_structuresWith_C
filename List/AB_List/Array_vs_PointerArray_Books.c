#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INIT_CAPACITY 5 // 初始容量

typedef struct BooksList {
    int id;
    char name[20];
    int price;
} Books;

typedef struct {
    Books* data;  // 指向动态分配数组的指针
    int length;  // 当前实际元素个数
    int capacity;  // 当前允许的最大容量
} SeqList;

//  为Books赋值
void AssignBooks(Books* B, int id, char* name, int price) {
    B->id = id;
    strcpy(B->name, name);
    B->price = price;
}

//  初始化顺序表
void InitList(SeqList* L, int initCapacity) {
    L->data = (Books*)malloc(sizeof(Books) * initCapacity);
    if (L->data == NULL) {
        printf("内存分配失败！\n");
        return;
    }
    L->length = 0;
    L->capacity = initCapacity;
}
//  从顺序表末尾添加元素
void add(SeqList* L, Books B) {
    //ExpandCapacity(L);
    L->data[L->length] = B;
    L->length++;
}
//  插入元素
void Insert(SeqList* L, int index, Books B) {
    // 边界检查：index 必须在 0 到 length 之间
    if (index < 0 || index > L->length) {
        printf("插入位置不合法！\n");
        return;
    }
    // 检查是否需要扩容
    //ExpandCapacity(L);
    // 将 index 及后面的元素统一向后移动一位
    for (int i = L->length; i >= index; i--) {
        L->data[i] = L->data[i - 1];
    }
    // 插入新元素
    L->data[index-1] = B;
    L->length++;
}
//  打印顺序表
void PrintList(SeqList* L) {
    printf("顺序表元素:\n");
    for (int i = 0; i < L->length; i++) {
        printf("%d %s %d \n", L->data[i].id, L->data[i].name, L->data[i].price);
    }
    printf("\n");
}

int main(){
    SeqList L;
    InitList(&L, INIT_CAPACITY);

    Books B1, B2, B3;
    AssignBooks(&B1, 1, "C语言", 100);
    add(&L, B1);
    AssignBooks(&B2, 2, "Java", 200);
    add(&L, B2);
    AssignBooks(&B3, 3, "Python", 300);
    add(&L, B3);
    printf("插入前:\n");
    PrintList(&L);

    Books B4;
    AssignBooks(&B4, 4, "JavaScript", 400);
    Insert(&L, 2, B4);

    printf("插入后:\n");
    PrintList(&L);
    return 0;
}