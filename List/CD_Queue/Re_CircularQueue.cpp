#include <iostream>
#include <string>

constexpr int MAXSIZE = 4;

using std::cout;

struct Books
{
    int book_id;
    std::string name;
    double price;

    Books () = default;
    Books (int bk, std::string n, double p) :
    book_id (bk), name (n), price (p) {}
};

struct Queue
{
    Books* data;
    int front = 0;
    int rear = 0;
    int tag = 0;
};

//  初始化循环队列
Queue* InitQueue ()
{
    Queue* Q = new Queue;
    Q->data = new Books[MAXSIZE];
    return Q;
}

//  入队
void enqueue (Queue* Q,const Books& B)
{
    if (Q->front == Q->rear && Q->tag == 1)
    {
        std::cout << "Queue full" << '\n';
        return;
    }
    Q->data[Q->rear] = B;
    Q->rear = ((Q->rear) + 1) % MAXSIZE;
    Q->tag = 1;
}

//  出队
void dequeue (Queue* Q)
{
    if (Q->front == Q->rear && Q->tag == 0)
    {
        std::cout << "Queue empty" << '\n';
        return;
    }
    Q->front = ((Q->front) + 1) % MAXSIZE;
    Q->tag = 0;
}

//  打印循环队列
void PrintQueue (Queue* Q)
{
    int i = Q->front;
    if (Q->front == Q->rear && Q->tag == 0)
    {
        std::cout << "Queue empty" << '\n';
        return;
    }
    do
    {
        std::cout << Q->data[i].book_id << " " 
                << Q->data[i].name << " "
                << Q->data[i].price << '\n';
        i = (i + 1) % MAXSIZE;
    }   while (i != Q->rear);
}

void DeleteQueue (Queue*& Q)
{
    if (Q == nullptr) return;
    delete[] (Q->data);
    Q->data = nullptr;
    delete (Q);
    Q = nullptr;
}

int main ()
{
    Queue* Q = InitQueue ();
    enqueue (Q, Books {1, "bk1", 13});
    enqueue (Q, Books {2, "bk2", 24});
    enqueue (Q, Books {3, "bk3", 33});
    enqueue (Q, Books {4, "bk4", 41});

    PrintQueue (Q);
    std::cout << "==================" << '\n';

    dequeue (Q);
    PrintQueue (Q);

    DeleteQueue (Q);
    return 0;
}