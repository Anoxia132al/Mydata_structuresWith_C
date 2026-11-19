#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct BooksList {
    int id;
    char name[20];
    int price;
} Books;

typedef struct node{
    Books* data;
    struct node* Next;
    struct node* Pre;
} Node;

//  初始化双向链表,返回头节点
Node* InitList() {
    Node* head = (Node*)malloc(sizeof(Node));
    head->data = NULL;
    head->Next = NULL;
    head->Pre = NULL;
    return head;
}

//  头插法插入元素
void InsertHead(Node* head, Books b) {
    // 为新节点分配内存
    Node* newNode = (Node*)malloc(sizeof(Node));
    // 为新节点数据分配内存
    newNode->data = (Books*)malloc(sizeof(Books));
    // 为新节点赋值
    /*
    newNode->data->id = b.id;
    strcpy(newNode->data->name, b.name);
    newNode->data->price = b.price;
    */
    *(newNode->data) = b;

    newNode->Next = head->Next;  // 新节点指向头节点的下一个节点
    newNode->Pre = head;  // 新节点的前一个节点指向头节点
    if (head->Next != NULL) {
        head->Next->Pre = newNode;  // 头节点的下一个节点的前一个节点指向新节点
    }
    head->Next = newNode;  // 头节点的下一个节点指向新节点
}

//  尾插法插入元素
void InsertTail(Node* head, Books b) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = (Books*)malloc(sizeof(Books));
    *(newNode->data) = b;

    //找到最后一个节点
    Node* current = head->Next;
    while (current->Next != NULL) {
        current = current->Next;
    }
    //将新节点插入到最后一个节点的后面
    newNode->Next = NULL;
    newNode->Pre = current;
    current->Next = newNode;
}

//  在指定位置插入元素


//  删除指定位置元素


//  打印双向链表
void PrintList(Node* head) {
    Node* current = head->Next;
    while (current != NULL) {
        printf("%d %s %d\n", current->data->id, current->data->name, current->data->price);
        current = current->Next;
    }
}

//  释放双向链表内存
void FreeList(Node* head) {
    Node* current = head->Next;
    while (current != NULL) {
        Node* next = current->Next;
        free(current->data);  // 释放节点数据内存
        free(current);  // 释放节点内存
        current = next;
    }
    free(head);  // 释放头节点内存
}

int main() {
    Node* head = InitList();  // 初始化双向链表
    
    Books b1 = {1, "Book1", 100};
    Books b2 = {2, "Book2", 200};
    InsertHead(head, b2);
    InsertHead(head, b1);
    printf("after insertHead:\n");
    PrintList(head);

    Books b3 = {3, "Book3", 300};
    Books b4 = {4, "Book4", 400};
    InsertTail(head, b3);
    InsertTail(head, b4);
    printf("after insertTail:\n");
    PrintList(head);

    FreeList(head);  // 释放双向链表内存
    return 0;
}
