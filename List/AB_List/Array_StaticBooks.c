#include <stdio.h>

#define LIST_SIZE 3

typedef struct BooksList
{
    int ISBN;
    char* Title;
    double Price;
} Books;

void Init(int ISBN, char* Title, double Price, Books* b){
    b->ISBN = ISBN;
    b->Title = Title;
    b->Price = Price;
}

/* 加了长度参数 n，不再把 3 写死在循环里 */
void show(Books* List, int n){
    for(int i = 0; i < n; i++){
        printf("ISBN: %d, Title: %s, Price: %.2f\n", List[i].ISBN, List[i].Title, List[i].Price);
    }
}

void SearchByISBN(Books* List, int n, int ISBN){
    for(int i = 0; i < n; i++){
        if(List[i].ISBN == ISBN){
            printf("ISBN: %d\n", List[i].ISBN);
            printf("Title: %s\n", List[i].Title);
            printf("Price: %.2f\n", List[i].Price);
            return;
        }
    }
    printf("Not found: ISBN %d\n", ISBN);
}

void TitleChange(Books* b, char* newTitle){
    b->Title = newTitle;
}

/* 改数组元素：List[index] 本身就是左值，直接写就行 */
void SetTitleByIndex(Books* List, int n, int index, char* newTitle){
    if(index < 0 || index >= n){
        printf("index out of range: %d\n", index);
        return;
    }
    List[index].Title = newTitle;
}

/* ---------- 方案二：数组存指针，元素用 -> 访问 ---------- */
void showPtr(Books** List, int n){
    for(int i = 0; i < n; i++){
        printf("ISBN: %d, Title: %s, Price: %.2f\n",
               List[i]->ISBN, List[i]->Title, List[i]->Price);
    }
}

int main(){

    Books b1 = {12345, "Book1", 100.0};
    Books b2 = {67890, "Book2", 200.0};
    Books b3;
    Init(11223344, "Book3", 300.0, &b3);

    /* ===== 方案一：值数组 —— 地址必须从数组元素上取 ===== */
    Books List[LIST_SIZE] = {b1, b2, b3};

    printf("===== 1. Before =====\n");
    show(List, LIST_SIZE);

    printf("===== 2. Change Title =====\n");
    TitleChange(&List[1], "Book2_1");          /* 关键：&List[1]，不是 &b2 */
    List[1].Price = 250.0;                     /* 也可以直接改字段 */
    SetTitleByIndex(List, LIST_SIZE, 2, "Book3_1");
    show(List, LIST_SIZE);

    printf("===== 3. b2 itself keeps its old value (separate copy) =====\n");
    printf("b2.Title = %s, b2.Price = %.2f\n", b2.Title, b2.Price);

    printf("===== 4. Search By ISBN =====\n");
    SearchByISBN(List, LIST_SIZE, 12345);
    SearchByISBN(List, LIST_SIZE, 11223344);
    SearchByISBN(List, LIST_SIZE, 99999);

    /* ===== 方案二：指针数组 —— b1/b2/b3 与 List 共享同一个对象 ===== */
    printf("===== 5. Pointer array: b2 and List[1] are the same object =====\n");
    Books* PtrList[LIST_SIZE] = { &b1, &b2, &b3 };
    TitleChange(&b2, "Book2_2");   /* 改 b2，PtrList[1] 跟着变 */
    showPtr(PtrList, LIST_SIZE);

    return 0;
}
