#include <iostream>
#include <string>

//  单链表
struct Books
{
    int bookid;
    std::string name;
    double price;
};

struct Node
{
    Books data;
    Node* next;
};


//  初始化链表
// Node* InitList ()
// {
//     Node* head = new Node;
//     head->next = nullptr;
//     head->data = {};
//     return head;
// }
Node* InitList () { return new Node {{}, nullptr}; }


// Node* Create_newNode (Books B)
// {
//     Node* Temp = new Node;
//     Temp->data.bookid = B.bookid;
//     Temp->data.name = B.name;
//     Temp->data.price = B.price;

//     Temp->next = nullptr;
//     return Temp;
// }
Node* Create_newNode (const Books& B)
{
    return new Node {B, nullptr};
}


//  头插法
void InsertHead (Node* head, Books B)
{
    Node* newNode = Create_newNode (B);
    newNode->next = head->next;
    head->next = newNode;
}

//  尾插法
void InsertTail (Node* head, Books B)
{
    Node* newNode = Create_newNode (B);
    Node* current = head;
    while (current->next != nullptr)
    {
        current = current->next;
    }
    current->next = newNode;
}

//  指定位置插入元素
void InsertAt (Node* head, int index, Books B)
{
    if (head == nullptr || index < 1) return;

    Node* newNode = Create_newNode (B);
    Node* current = head;
    for (int i = 0; i != index - 1; ++i) { current = current->next; }
    newNode->next = current->next;
    current->next = newNode;
}

//  指定位置删除元素
void DeleteAt (Node* head, int index)
{
    if (head == nullptr || index < 1) return;

    Node* current = head;
    for (int i = 0; i != index - 1; ++i) { current = current->next; }
    Node* toDelete = current->next;
    current->next = toDelete->next;
    delete toDelete;
}

//  打印链表
void PrintList (Node* head)
{
    if (head == nullptr)
    {
        std::cout << "List nullptr" << '\n';
        return;
    }

    Node* current = head->next;
    while (current != nullptr)
    {
        std::cout << "bookid : " << current->data.bookid << " "
                << "name : " << current->data.name << " "
                << "price : " << current->data.price << '\n';
        current = current->next;
    }
}

//  释放链表内存
void DeleteList (Node* head)
{
    Node* temp;
    while (head != nullptr)
    {
        temp = head;
        head = head->next;
        delete temp;
    }
}

//  查找倒数第 k 个节点,快慢指针
Books FindLast_K (Node* head, int k)
{
    if (head == nullptr || k <= 0) return {};

    Node* fast = head;
    Node* slow = head;
    for (int i = 0; i != k; ++i) 
    { 
        if (fast == nullptr) return {};
        fast = fast->next; 
    }
    while (fast != nullptr)
    {
        fast = fast->next;
        slow = slow->next;
    }
    return slow->data;
}

//  反转链表，三指针法
void ReverseList (Node* head)
{
    if (head == nullptr) return;

    Node* pre = nullptr;
    Node* curr = head->next;
    while (curr != nullptr)
    {
        Node* next = curr->next;
        curr->next = pre;
        pre = curr;
        curr = next;
    }
    head->next = pre;
}

//  删除中间节点
void DeleteMid (Node* head)
{
    if (head == nullptr) return;

    Node* curr = head->next;
    int cnt = 1;
    while (curr != nullptr)
    {
        curr = curr->next;
        ++cnt;
    }
    int idx = (cnt + 1) / 2;
    Node* pre_todelete = head;
    for (int i = 0; i != idx - 1; ++i)
    {
        pre_todelete = pre_todelete->next;
    }
    Node* todelete = pre_todelete->next;
    pre_todelete->next = todelete->next;
    delete todelete;
}

//  删除中间节点，快慢指针
void DeleteMid_Double (Node* head)
{
    if (head->next == nullptr) return;

    Node* fast = head;
    Node* slow = head;
    Node* pre = nullptr;
    while (fast != nullptr && fast->next != nullptr)
    {
        fast = fast->next->next;
        pre = slow;
        slow = slow->next;
    }
    pre->next = slow->next;
    delete slow;
}

int main ()
{
    Books bk1 = {1, "bk1", 14};
    Books bk2 = {2, "bk2", 22};
    Books bk3 = {3, "bk3", 31};
    Books bk4 = {4, "bk4", 44};

    Node* head = InitList ();

    InsertHead (head, bk1);
    InsertHead (head, bk2);
    InsertTail (head, bk3);
    PrintList (head);
    std::cout << "====================" << '\n';

    DeleteAt (head, 2);
    PrintList (head);
    std::cout << "====================" << '\n';
    InsertAt (head, 2, bk4);
    PrintList (head);
    std::cout << "====================" << '\n';

    std::cout << "FindLast_3 : "
            << FindLast_K (head, 3).name << '\n';
    std::cout << "====================" << '\n';

    ReverseList (head);
    PrintList (head);
    std::cout << "====================" << '\n';

    DeleteMid (head);
    PrintList (head);
    std::cout << "====================" << '\n';
    
    DeleteList (head);
    head = nullptr;
    return 0;
}