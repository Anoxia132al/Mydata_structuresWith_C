/* Main2_01.c —— 用顺序表（SqList）的形式重新实现图书列表
 *
 * 与 Main2.c 的本质区别：
 *   Main2.c : Books List[3]  —— 定长静态数组，长度写死在代码里，
 *                               没有插入/删除/扩容，length 靠人脑记
 *   本文件  : SeqList        —— 连续空间 + length + capacity 三件套，
 *                               支持插入、删除、按位/按值查找、动态扩容
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INIT_CAPACITY 3

/* ---------------- 元素类型 ---------------- */
typedef struct BooksList {
    int    ISBN;
    char   Title[64];   /* 用字符数组，这样能直接原地改字符串内容 */
    double Price;
} Books;

/* ---------------- 顺序表：数据 + 逻辑长度 + 物理容量 ---------------- */
typedef struct {
    Books* data;      /* 指向连续存储空间的首地址 */
    int    length;    /* 当前元素个数（逻辑长度） */
    int    capacity;  /* 当前能放下几个元素（物理容量） */
} SeqList;

/* ---------------- 构造一个 Books ----------------
 * Title 是字符数组，不能用 "..." 直接赋值，需要拷贝 */
Books MakeBook(int ISBN, const char* Title, double Price) {
    Books b;
    b.ISBN = ISBN;
    snprintf(b.Title, sizeof(b.Title), "%s", Title);
    b.Price = Price;
    return b;
}

/* ---------------- 1. 初始化：分配初始空间 ---------------- */
int InitList(SeqList* L) {
    L->data = (Books*)malloc(sizeof(Books) * INIT_CAPACITY);
    if (L->data == NULL) {
        L->length = 0;
        L->capacity = 0;
        return 0;
    }
    L->length = 0;                 /* 空表：length = 0 */
    L->capacity = INIT_CAPACITY;
    return 1;
}

/* ---------------- 2. 销毁：释放空间 ---------------- */
void DestroyList(SeqList* L) {
    free(L->data);
    L->data = NULL;
    L->length = 0;
    L->capacity = 0;
}

/* ---------------- 3. 扩容：满了就翻倍（内部函数） ---------------- */
static int EnsureCapacity(SeqList* L) {
    if (L->length < L->capacity) return 1;   /* 还有空位，不用扩 */
    int oldCap = L->capacity;
    int newCap = oldCap * 2;
    Books* p = (Books*)realloc(L->data, sizeof(Books) * newCap);
    if (p == NULL) return 0;                 /* 扩容失败，原空间仍有效 */
    L->data = p;
    L->capacity = newCap;
    printf("  [扩容] capacity: %d -> %d\n", oldCap, newCap);
    return 1;
}

/* ---------------- 4. 按位查找：返回元素地址，拿到即可原地改 ----------------
 * 这正是顺序表的看家本领：随机访问 O(1) */
Books* GetElem(SeqList* L, int i) {
    if (i < 0 || i >= L->length) return NULL;   /* 越界挡住 */
    return &L->data[i];
}

/* ---------------- 5. 按值查找：返回下标，找不到返回 -1 ---------------- */
int LocateByISBN(SeqList* L, int ISBN) {
    for (int i = 0; i < L->length; i++) {
        if (L->data[i].ISBN == ISBN) return i;
    }
    return -1;
}

/* ---------------- 6. 插入：在第 i 个位置插入 e（i 合法范围 0..length） ----------------
 * 平均要移动一半元素，O(n) —— 这是顺序表的代价 */
int ListInsert(SeqList* L, int i, Books e) {
    if (i < 0 || i > L->length) return 0;       /* 位置非法 */
    if (!EnsureCapacity(L)) return 0;           /* 空间不够先扩容 */
    for (int j = L->length; j > i; j--) {       /* i 之后的元素整体后移一格 */
        L->data[j] = L->data[j - 1];
    }
    L->data[i] = e;
    L->length++;
    return 1;
}

/* ---------------- 7. 尾插：等价于在 length 位置插入 ---------------- */
int ListAppend(SeqList* L, Books e) {
    return ListInsert(L, L->length, e);
}

/* ---------------- 8. 删除：删掉第 i 个元素，被删的值用 *out 带出 ---------------- */
int ListDelete(SeqList* L, int i, Books* out) {
    if (i < 0 || i >= L->length) return 0;
    if (out != NULL) *out = L->data[i];         /* 先把值带出去，再覆盖 */
    for (int j = i; j < L->length - 1; j++) {   /* i 之后的元素整体前移一格 */
        L->data[j] = L->data[j + 1];
    }
    L->length--;
    return 1;
}

/* ---------------- 9. 遍历打印 ---------------- */
void PrintList(const SeqList* L) {
    printf("  length = %d, capacity = %d\n", L->length, L->capacity);
    for (int i = 0; i < L->length; i++) {
        printf("  [%d] ISBN: %-8d Title: %-12s Price: %.2f\n",
               i, L->data[i].ISBN, L->data[i].Title, L->data[i].Price);
    }
}

int main(void) {
    SeqList L;

    printf("===== 1. InitList =====\n");
    if (!InitList(&L)) {
        printf("初始化失败\n");
        return 1;
    }
    printf("  capacity = %d\n", L.capacity);

    printf("===== 2. 尾插三本书 =====\n");
    ListAppend(&L, MakeBook(12345,    "Book1", 100.0));
    ListAppend(&L, MakeBook(67890,    "Book2", 200.0));
    ListAppend(&L, MakeBook(11223344, "Book3", 300.0));
    PrintList(&L);

    printf("===== 3. 再尾插一本，触发扩容 =====\n");
    ListAppend(&L, MakeBook(55555, "Book4", 400.0));
    PrintList(&L);

    printf("===== 4. 头插一本（后面元素整体后移） =====\n");
    ListInsert(&L, 0, MakeBook(99999, "Book0", 50.0));
    PrintList(&L);

    printf("===== 5. 按值查找 ISBN =====\n");
    int idx = LocateByISBN(&L, 67890);
    printf("  ISBN 67890 的下标 = %d\n", idx);

    printf("===== 6. 按下标取到地址并原地修改 =====\n");
    Books* p = GetElem(&L, idx);
    if (p != NULL) {
        strcpy(p->Title, "Book2_1");    /* 改的是顺序表里的那份内存 */
        p->Price = 250.0;
    }
    PrintList(&L);

    printf("===== 7. 删除下标 0 的元素 =====\n");
    Books removed;
    if (ListDelete(&L, 0, &removed)) {
        printf("  被删除的是: %s\n", removed.Title);
    }
    PrintList(&L);

    printf("===== 8. 越界访问应当被挡住 =====\n");
    printf("  GetElem(&L, %d) = %s\n", L.length,
           GetElem(&L, L.length) == NULL ? "NULL（已拦截）" : "不该出现");

    printf("===== 9. DestroyList =====\n");
    DestroyList(&L);
    printf("  length = %d, capacity = %d\n", L.length, L.capacity);

    return 0;
}
