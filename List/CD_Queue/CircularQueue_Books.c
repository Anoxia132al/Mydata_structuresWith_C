#include <stdio.h>
#include <stdlib.h>

#define MAXSIZE 8

typedef struct BookList {
    int id;
    char name[20];
    int price;
}Books;

typedef struct {
    Books* data;
    int front;
    int rear;
    int tag;
}Queue;

//  初始化队列
Queue* InitQueue() {
    Queue* q = (Queue*)malloc(sizeof(Queue)*MAXSIZE);
    q->data = (Books*)malloc(sizeof(Books)*MAXSIZE);
    q->front = 0;
    q->rear = 0;
    return q;
}

//  入队
void equeue(Queue* q, Books* b) {
    /*
    if((q->rear + 1) % MAXSIZE == q->front) {
        printf(" full\n");
        return;
    }
    */
   if (q->front == q->rear && q->tag == 1 ) {
        printf("full");
        return;
   }
    q->data[q->rear] = *b;
    q->rear = ((q->rear) + 1) % MAXSIZE;
    q->tag = 1; 
}

//  出队
void dequeue(Queue* q) {
    /*
    if(q->front == q->rear){
        printf(" empty\n");
        return;
    }
    */
    if (q->front == q->rear && q->tag == 0 ) {
        printf("empty");
        return;
   }
    q->front = ((q->front) + 1) % MAXSIZE;
    q->tag = 0;
}

void FreeQueue(Queue* q) {
    if (q == NULL) {
        return;
    }
    free(q->data);
    q->data = NULL;
    free(q);
}

void PrintQueue(Queue* q) {
    int i = q->front;  
    if (q->rear == i && q->tag == 0) {
        printf("full\n");
        return;
    }
    do {
        printf(" %d %s %d\n",q->data[i].id, q->data[i].name, q->data[i].price);
        i = (i + 1) % MAXSIZE;
    } while (i != q->rear);
}

int main() {
    Queue* q = InitQueue();

    Books b1 = {1, "book1", 100};
    Books b2 = {2, "book2", 200};
    Books b3 = {3, "book3", 300};
    Books b4 = {4, "book4", 400};
    Books b5 = {5, "book5", 500};
    Books b6 = {6, "book6", 600};
    Books b7 = {7, "book7", 700};
    Books b8 = {8, "book8", 800};
    equeue(q, &b1);
    equeue(q, &b2);
    equeue(q, &b3);
    equeue(q, &b4);
    equeue(q, &b5);
    equeue(q, &b6);
    equeue(q, &b7);
    equeue(q, &b8);
    PrintQueue(q);
    printf("==========\n");

    dequeue(q);
    PrintQueue(q);
    printf("==========\n");

    FreeQueue(q);
    q = NULL;
    return 0;
}