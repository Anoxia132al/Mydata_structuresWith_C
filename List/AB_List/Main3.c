# include <stdio.h>
# include <stdlib.h>
# include <string.h>

typedef struct BooksList {
    int id;
    char name[20];
    int price;
} Books;

typedef struct node{
    Books* data;
    struct node* Next;
} Node;

//  初始化链表
Node* InitList() {
    Node* head = (Node*)malloc(sizeof(Node));
    head->data = NULL;
    head->Next = NULL;
    return head;
}

//  从头插入元素
void InsertHead(Node* head, Books B) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    // 为新节点分配内存
    newNode->data = (Books*)malloc(sizeof(Books));
    // 为新节点赋值
    newNode->data->id = B.id;
    strcpy(newNode->data->name, B.name);
    newNode->data->price = B.price;
    
    newNode->Next = head->Next;
    head->Next = newNode;

}

//  从尾插入元素
void InsertTail(Node* head, Books B) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    // 为新节点分配内存
    newNode->data = (Books*)malloc(sizeof(Books));
    // 为新节点赋值
    newNode->data->id = B.id;
    strcpy(newNode->data->name, B.name);
    newNode->data->price = B.price;
    newNode->Next = NULL;  // 新节点指向 NULL
    Node* current = head;  // 从头节点开始遍历链表
    while (current->Next != NULL) {
        current = current->Next;  // 移动到下一个节点
    }
    // 从尾插入元素
    current->Next = newNode;
}

//  指定位置插入元素
void InsertAt(Node* head, int index, Books B) {
    //  找到插入位置的前一个节点
    Node* current = head;
    for (int i = 0; i < index - 1; i++) {
        current = current->Next;
    }
    //  插入新元素
    Node* newNode = (Node*)malloc(sizeof(Node));
    // 为新节点分配内存
    newNode->data = (Books*)malloc(sizeof(Books));
    // 为新节点赋值
    newNode->data->id = B.id;
    strcpy(newNode->data->name, B.name);
    newNode->data->price = B.price;
    newNode->Next = current->Next;  // 新节点指向当前节点的下一个节点
    current->Next = newNode;  // 当前节点指向新节点
}

//  指定位置删除元素
void DeleteAt(Node* head, int index) {
    //  找到删除位置的前一个节点
    Node* current = head;
    for (int i = 0; i < index - 1; i++) {
        current = current->Next;
    }
    //  删除元素
    Node* toDelete = current->Next;
    current->Next = toDelete->Next;
    free(toDelete->data);  // 释放节点数据内存
    free(toDelete);  // 释放节点内存 
    
}

//  删除相同元素的节点,保留第一个元素
void DeleteDuplicates(Node* head) {
    //  开辟一个辅助数组,用于存储已经出现过的元素
    int* visited = (int*)malloc(sizeof(int) * 1000000);
    //  初始化辅助数组,将所有元素设为0
    memset(visited, 0, sizeof(int) * 1000000);

    Node* prev = head;
    Node* current = head->Next;
    //  遍历链表,如果当前元素没有出现过,则将它添加到辅助数组中,并删除所有后续的相同元素
    while (current != NULL) {
        if (visited[current->data->id] == 0) {
            visited[current->data->id] = 1;
            prev = prev->Next;
            current = current->Next;
        } else {
            Node* toDelete = current;
            prev->Next = current->Next;
            current = current->Next;  // 移动到下一个节点
            free(toDelete->data);  // 释放节点数据内存
            free(toDelete);  // 释放节点内存
        }
    }
    free(visited);  // 释放辅助数组内存
}

//  打印链表
void PrintList(Node* head) {
    Node* current = head->Next;
    while (current != NULL) {
        printf("%d %s %d\n", current->data->id, current->data->name, current->data->price);
        current = current->Next;
    }
}

//  释放链表内存
void FreeList(Node* head) {
    Node* current = head->Next;  // 从头节点开始遍历链表
    while (current != NULL) {
        Node* next = current->Next;
        free(current->data);  // 释放节点数据内存
        free(current);  // 释放节点内存
        current = next;  // 移动到下一个节点
    }
    free(head);  // 释放头节点内存
}

//  查找倒数第k个节点

// 方法1:  先遍历链表,统计节点数量,再遍历链表,找到倒数第k个节点
Books* FindLastK_Counting(Node* head, int k) {
    Node* current = head;
    int count = 0;
    while (current != NULL) {
        current = current->Next;
        count++;
    }
    if (count < k) {
        return NULL;
    }
    current = head;
    for (int i = 0; i < count - k; i++) {
        current = current->Next;
    }
    return current->data;
}

//  方法2:  使用双指针,一个指针先走k步,另一个指针再走,当第一个指针走到链表末尾时,第二个指针就指向倒数第k个节点
Books* FindLastK_DoublePointer(Node* head, int k) {
    Node* fast = head;
    Node* slow = head;
    for (int i = 0; i < k; i++) {
        fast = fast->Next;
    }
    while (fast != NULL) {
        fast = fast->Next;
        slow = slow->Next;
    }
    return slow->data;
}

//  反转链表,三指针法
void ReverseList(Node* head) {
    Node* prev = NULL;
    Node* current = head->Next;
    while (current != NULL) {
        Node* next = current->Next;
        current->Next = prev;  // 将当前节点的指针指向它的前一个节点
        prev = current;  // 移动到当前节点
        current = next;  // 移动到下一个节点
    }
    head->Next = prev;
}

//  删除链表中间节点
//  遍历链表,找到要删除的节点的前一个节点,将它的指针指向要删除的节点的下一个节点
void DeleteMiddle(Node* head) {
    Node* current = head->Next;
    while (current->Next != NULL) {
        current = current->Next;
    }
    if (current->Next == NULL) {
        printf("要删除的节点不存在\n");
        return;
    }
    Node* toDelete = current->Next;
    current->Next = toDelete->Next;
    free(toDelete->data);  // 释放节点数据内存
    free(toDelete);  // 释放节点内存
}

int main() {
    
    Node* head = InitList();  // 初始化链表

    Books b3 = {3, "Book3", 300};
    Books b4 = {4, "Book4", 400};
    InsertTail(head, b3);
    InsertTail(head, b4);
    
    printf("从尾插入元素后:\n");
    PrintList(head);
    printf("从头插入元素:\n");
    
    Books b1 = {1, "Book1", 100};
    Books b2 = {2, "Book2", 200};
    InsertHead(head, b2);
    InsertHead(head, b1);

    printf("在第3个位置插入元素:\n");
    Books b3_5 = {3, "Book3.5", 500};
    InsertAt(head, 3, b3_5);
    PrintList(head);

    printf("在第3个位置删除元素:\n");
    DeleteAt(head, 3);
    PrintList(head);

    printf("倒数第2个节点为:%d %s %d\n", FindLastK_Counting(head, 2)->id, FindLastK_Counting(head, 2)->name, 
    FindLastK_Counting(head, 2)->price);
    printf("倒数第3个节点为:%d %s %d\n", FindLastK_DoublePointer(head, 3)->id,FindLastK_DoublePointer(head, 3)->name,
    FindLastK_DoublePointer(head, 3)->price);

    InsertTail(head, b3);
    printf("在尾插入元素后:\n");
    PrintList(head);
    printf("删除相同元素后:\n");
    DeleteDuplicates(head);
    PrintList(head);

    printf("反转链表后:\n");
    ReverseList(head);
    PrintList(head);

    printf("删除中间节点后:\n");
    DeleteMiddle(head);
    PrintList(head);
    
    FreeList(head);  // 释放链表内存
    return 0;
}

