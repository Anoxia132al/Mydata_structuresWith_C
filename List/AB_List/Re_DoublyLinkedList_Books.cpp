#include <iostream>
#include <string>

struct Books
{
    int book_id = 0;
    std::string name = {};
    double price = 0.0;

    Books () = default;
    Books (int id, std::string n, double p)
        : book_id(id), name(n), price(p) {}
};

struct Node
{
    Books data;
    Node* next;
    Node* pre;

    Node () : data{}, next(nullptr), pre(nullptr) {}
    explicit Node(const Books& book)
        : data(book), next(nullptr), pre(nullptr) {}
};

Node* initList ()
{
    // Node* head = new Node;
    // head->next = nullptr;
    // head->pre = nullptr;
    // return head;
    return new Node;
}

Node* CreatNode (const Books& B)
{
    // Node* tempNode = new Node;
    // tempNode->data = B;
    // tempNode->next = nullptr;
    // tempNode->pre = nullptr;
    // return tempNode;
    return new Node(B);
}

void InsertHead (Node* head,const Books& B)
{
    Node* newNode = CreatNode(B);
    newNode->next = head->next;
    newNode->pre = head;
    if (head->next != nullptr)
        head->next->pre = newNode;
    head->next = newNode;
}
void InsertTail (Node* head, const Books& B)
{
    Node* newNode = CreatNode(B);
    Node* curr = head;
    while (curr->next != nullptr)
        curr = curr->next;
    curr->next = newNode;
    newNode->pre = curr;
}

void PrintList (const Node* head)
{
    Node* curr = head->next;
    while (curr != nullptr)
    {
        std::cout << curr->data.book_id << " "
            << curr->data.name << " "
            << curr->data.price << " ";
        std::cout << std::endl;
        curr = curr->next;
    }   
}

void DeleteList (Node*& head)
{
    Node* curr = head;
    while (curr != nullptr)
    {
        Node* next = curr->next;
        delete curr;
        curr = next;
    }
    head = nullptr;
}

int main ()
{
    Node* head = initList ();

    InsertHead (head, Books(1, "bk1", 13));
    InsertHead (head, Books(2, "bk2", 34));
    std::cout << "after InsertHead : " << '\n';
    PrintList (head);

    InsertTail (head, Books(3, "bk3", 41));
    InsertTail (head, Books(4, "bk4", 56));
    std::cout << "after InsertTail : " << '\n';
    PrintList (head);

    DeleteList (head);
    return 0;
}