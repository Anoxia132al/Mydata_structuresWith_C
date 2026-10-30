#include <stdio.h>
#include <stdlib.h>

typedef struct TreeNode {
    char data;
    struct TreeNode* left;
    struct TreeNode* right;
    int ltag;
    int rtag;
} TreeNode;

char str[] = {"ABDH##I##EJ###CF##G##"};
int idx = 0;

TreeNode* pre;  // 记录上一个访问的节点

//  创建二叉树, 并设置线索
void createBiTree(TreeNode** root) {
    char ch;
    ch = str[idx++];
    if (ch == '#') {
        *root = NULL;
    }
    else {
        *root = (TreeNode*)malloc(sizeof(TreeNode)); 
        (*root)->data = ch;
        // 错误：ltag/rtag 没有统一初始化。只有孩子非空时才赋值，
        //       叶子结点（孩子为 NULL）的两个 tag 是 malloc 出来的垃圾值。
        //       正确做法：创建时一律置 0（表示 left/right 指向真实孩子），
        //       线索化时再把没有孩子的那些改成 1。
        (*root)->ltag = 0;
        (*root)->rtag = 0;

        createBiTree(&(*root)->left);
        // 错误：下面这种“孩子非空才置 0”的写法漏掉了孩子为空的情况，
        //       而且既然上面已统一初始化，这两段就是多余且易误导的。
        // if ((*root)->left != NULL) {
        //     (*root)->ltag = 0;
        // }
        createBiTree(&(*root)->right);
        // if ((*root)->right != NULL) {
        //     (*root)->rtag = 0;
        // }
    }
}
 //  中序线索化
void threading(TreeNode* root) {
    // 错误（原写法，已注释）：
    //   1. 缺少递归出口 root == NULL，空指针会直接解引用；
    //   2. 没有先递归左子树，不是“中序”；
    //   3. root->right == NULL 时执行 root->right = pre，把右线索指向前驱，
    //      但中序右线索应该指向“后继”，方向反了；
    //   4. 先改写 root->right，紧接着 threading(root->right)，
    //      会顺着刚建好的线索递归回已访问的结点 —— 死循环 / 栈溢出。
    // if (root->left == NULL) {
    //     root->left = pre;
    //     root->ltag = 1;
    // }
    // if (root->right == NULL) {
    //     root->right = pre;
    //     root->rtag = 1;
    // }
    // pre = root;
    // threading(root->right);

    if (root == NULL) {                 // 递归出口
        return;
    }

    threading(root->left);              // 中序：先线索化左子树

    if (root->left == NULL) {           // 左孩子为空：左线索指向前驱 pre
        root->left = pre;
        root->ltag = 1;
    }
    if (pre->right == NULL) {           // 前驱的右孩子为空：右线索指向当前结点（后继）
        pre->right = root;
        pre->rtag = 1;
    }
    pre = root;

    threading(root->right);             // 最后线索化右子树
}

void inOrderThreading(TreeNode* root, TreeNode** head) {
    *head = (TreeNode*)malloc(sizeof(TreeNode));
    (*head)->ltag = 0;
    (*head)->rtag = 1;
    (*head)->right = *head;

    if (root == NULL) {
        (*head)->left = *head;
    } else {
        (*head)->left = root;
        pre = *head;

        threading(root);

        // 错误：pre 是最后一个中序结点，它的右线索应指回头结点，
        //       所以该改的是 rtag，不是 ltag。
        //       pre->ltag = 1 会把最后一结点的“左孩子标记”改坏：
        //       若该结点有左子树，inOrder 中的 while (curr->ltag == 0)
        //       会提前停止，导致它的左子树整段访问不到。
        // pre->right = *head;
        // pre->ltag = 1;

        pre->right = *head;
        pre->rtag = 1;

        (*head)->right = pre;
    }
}

void inOrder(TreeNode* head) {
    TreeNode* curr = head->left;
    while (curr != head) {
        while (curr->ltag == 0) {
            curr = curr->left;
        }
        printf("%c ", curr->data);
        while (curr->rtag == 1 && curr->right != head) {
            curr = curr->right;  // 访问右线索
            printf("%c ", curr->data);
        }
        curr = curr->right;
    }
    printf("\n");
}

int main() {
    TreeNode* root = NULL;
    TreeNode* head;
    createBiTree(&root);
    inOrderThreading(root, &head);

    inOrder(head);
    return 0;
}