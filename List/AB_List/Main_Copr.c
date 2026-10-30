#include <stdio.h>
#include <stdlib.h> // 需要用到 malloc 和 free

// 1. 定义链表节点结构体
typedef struct Node {
    int data;           // 数据域（这里简单存一个整数，也可以换成你之前的结构体）
    struct Node* next;  // 指针域，指向下一个节点
} Node;

// 2. 创建一个新节点
Node* createNode(int value) {
    // 动态分配内存
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("内存分配失败！\n");
        return NULL;
    }
    newNode->data = value; // 赋值
    newNode->next = NULL;  // 新节点的下一个默认指向空
    return newNode;
}

// 3. 在链表末尾添加节点（尾插法）
// 返回新的头节点指针（因为如果链表为空，新节点就是头节点）
Node* appendNode(Node* head, int value) {
    Node* newNode = createNode(value);
    
    // 如果链表为空，新节点直接作为头节点
    if (head == NULL) {
        return newNode;
    }
    
    // 如果链表不为空，找到最后一个节点
    Node* temp = head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    // 将最后一个节点的 next 指向新节点
    temp->next = newNode;
    
    return head;
}

// 4. 遍历并打印链表
void printList(Node* head) {
    Node* temp = head;
    printf("链表内容: ");
    while (temp != NULL) {
        printf("%d -> ", temp->data);
        temp = temp->next; // 移动到下一个节点
    }
    printf("NULL\n");
}

// 5. 释放链表内存（非常重要，防止内存泄漏）
void freeList(Node* head) {
    Node* temp;
    while (head != NULL) {
        temp = head;       // 记录当前节点
        head = head->next; // 先走到下一个节点
        free(temp);        // 释放当前节点
    }
}

// 主函数测试
int main() {
    Node* head = NULL; // 初始化空链表
    
    // 添加几个节点
    head = appendNode(head, 10);
    head = appendNode(head, 20);
    head = appendNode(head, 30);
    
    // 打印链表
    printList(head); // 输出: 10 -> 20 -> 30 -> NULL
    
    // 释放内存
    freeList(head);
    
    return 0;
}