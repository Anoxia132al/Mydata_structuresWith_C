#include <stdio.h>
#include <stdlib.h>

#define INIT_CAPACITY 5 // 初始容量

// 1. 定义顺序表结构体
typedef struct {
    int* data;      // 指向动态分配数组的指针
    int length;     // 当前实际元素个数
    int capacity;   // 当前允许的最大容量
} SeqList;

// 2. 初始化顺序表
void InitList(SeqList* L, int initCapacity) {
    L->data = (int*)malloc(sizeof(int) * initCapacity);
    if (L->data == NULL) {
        printf("内存分配失败！\n");
        exit(1);
    }
    L->length = 0;
    L->capacity = initCapacity;
}

// 3. 扩容机制（当数组满了的时候）
void ExpandCapacity(SeqList* L) {
    if (L->length >= L->capacity) {
        int newCapacity = L->capacity * 2; // 容量翻倍
        int* newData = (int*)realloc(L->data, sizeof(int) * newCapacity);
        if (newData == NULL) {
            printf("内存扩容失败！\n");
            exit(1);
        }
        L->data = newData;
        L->capacity = newCapacity;
        printf("【提示】容量已满，自动扩容至 %d\n", newCapacity);
    }
}

// 4. 插入元素（指定位置插入）
// 返回 1 表示成功，0 表示失败
int Insert(SeqList* L, int index, int value) {
    // 边界检查：index 必须在 0 到 length 之间
    if (index < 0 || index > L->length) {
        printf("插入位置不合法！\n");
        return 0;
    }
    
    // 检查是否需要扩容
    ExpandCapacity(L);

    // 将 index 及后面的元素统一向后移动一位
    for (int i = L->length; i > index; i--) {
        L->data[i] = L->data[i - 1];
    }
    
    L->data[index] = value;
    L->length++;
    return 1;
}

// 5. 删除元素（指定位置删除）
int Delete(SeqList* L, int index, int* deletedValue) {
    // 边界检查：index 必须在 0 到 length-1 之间
    if (index < 0 || index >= L->length) {
        printf("删除位置不合法！\n");
        return 0;
    }

    if (deletedValue != NULL) {
        *deletedValue = L->data[index]; // 把被删除的值带出去
    }

    // 将 index 后面的元素统一向前移动一位
    for (int i = index; i < L->length - 1; i++) {
        L->data[i] = L->data[i + 1];
    }
    
    L->length--;
    return 1;
}

// 6. 按值查找
// 返回找到的索引，找不到返回 -1
int Search(SeqList* L, int value) {
    for (int i = 0; i < L->length; i++) {
        if (L->data[i] == value) {
            return i;
        }
    }
    return -1;
}

// 7. 打印顺序表
void PrintList(SeqList* L) {
    printf("当前顺序表 (长度: %d, 容量: %d): ", L->length, L->capacity);
    if (L->length == 0) {
        printf("空表\n");
        return;
    }
    for (int i = 0; i < L->length; i++) {
        printf("%d ", L->data[i]);
    }
    printf("\n");
}

// 8. 释放内存
void FreeList(SeqList* L) {
    if (L->data != NULL) {
        free(L->data);
        L->data = NULL;
        L->length = 0;
        L->capacity = 0;
    }
}

// ================= 测试主函数 =================
int main() {
    SeqList list;
    
    // 1. 初始化容量为 5
    InitList(&list, INIT_CAPACITY);
    
    // 2. 插入数据（会自动触发扩容）
    printf("--- 开始插入数据 ---\n");
    for (int i = 1; i <= 7; i++) {
        Insert(&list, list.length, i * 10); // 尾插法
        PrintList(&list);
    }
    
    // 3. 指定位置插入
    printf("\n--- 在下标 2 插入 99 ---\n");
    Insert(&list, 2, 99);
    PrintList(&list);
    
    // 4. 删除元素
    printf("\n--- 删除下标 0 的元素 ---\n");
    int delVal;
    Delete(&list, 0, &delVal);
    printf("被删除的元素是: %d\n", delVal);
    PrintList(&list);
    
    // 5. 查找
    printf("\n--- 查找元素 50 ---\n");
    int index = Search(&list, 50);
    if (index != -1) {
        printf("元素 50 的下标是: %d\n", index);
    } else {
        printf("未找到元素 50\n");
    }
    
    // 6. 释放内存
    printf("\n--- 释放内存 ---\n");
    FreeList(&list);
    
    return 0;
}