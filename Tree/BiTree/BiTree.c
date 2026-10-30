#include <stdio.h>
#include <stdlib.h>

typedef struct TreeNode {
    char data;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

typedef struct {
    // 错误：队列元素应该是 TreeNode*，而不是 TreeNode。
    // 存值意味着每次入队都整结构体拷贝一份；而 maxDepth 里又用 &q->data[q->front]
    // 取数组内部槽位的地址当结点用，一旦循环队列绕回覆盖该槽位，node 的数据就被改写。
    // TreeNode* data;
    TreeNode** data;
    int front;
    int rear;
} Queue;

char str[] = {"ABDH#K###E##CFI###G#J##"};
int idx = 0;

void createBiTree(TreeNode** root) {
    char ch;
    ch = str[idx++];
    if (ch == '#') {
        *root = NULL;
    }
    else {
        *root = (TreeNode*)malloc(sizeof(TreeNode));
        (*root)->data = ch;
        createBiTree(&(*root)->left);
        createBiTree(&(*root)->right);
    }
}
void preOrder(TreeNode* root) {
    if (root == NULL) {
        return;
    }
    printf("%c ", root->data);
    preOrder(root->left);
    preOrder(root->right);
}
Queue* initQueue() {
    Queue* q = (Queue*)malloc(sizeof(Queue));
    // 错误：data 改成存指针后，元素类型和单个元素大小都要跟着改。
    // q->data = (TreeNode*)malloc(sizeof(TreeNode) * 100);
    q->data = (TreeNode**)malloc(sizeof(TreeNode*) * 100);
    q->front = 0;
    q->rear = 0;
    return q;
}
void equeue(Queue* q, TreeNode* node) {
    // 错误：*node 把整个结点拷贝进队列数组。
    // q->data[q->rear] = *node;
    q->data[q->rear] = node;
    q->rear = ((q->rear) + 1) % 100;
    // 注意：这里没有判满。结点数超过 100 时 rear 会追尾 front，
    // 覆盖掉还没出队的元素，深度会被静默算错。工程代码应判满（报错或扩容）。
}

//  层序遍历：计算最大深度
int maxDepth(TreeNode* root) {
    if (root == NULL) {
        return 0;
    }
    int depth = 0;
    Queue* q = initQueue();
    equeue(q, root);  //  入队根节点
    int count = 1;  

    //  while 循环：遍历当前层的所有结点
    while (q->front != q->rear) {
        // 错误：&q->data[...] 取的是队列数组内部槽位的地址，不是树结点的地址。
        // 队列绕回时该槽位会被新元素覆盖，node->left/right 跟着被改掉。
        // TreeNode* node = &q->data[q->front];
        TreeNode* node = q->data[q->front];
        q->front = ((q->front) + 1) % 100;

        if (node->left != NULL) {
            equeue(q, node->left);
        }
        if (node->right != NULL) {
            equeue(q, node->right);
        }
        if (--count == 0) {
            depth++;
            // 错误：front/rear 是模 100 的循环下标，rear 绕回 0 后 rear-front 为负，
            // count 变负数就永远等不到 0，depth 不再增加，while 却继续空转。
            // count = q->rear - q->front;  //  更新下一层的结点数
            count = (q->rear - q->front + 100) % 100;  // 循环队列求长度
        }
    }
    free(q->data);
    free(q);
    return depth;
}
int main() {
    TreeNode* root = NULL;
    createBiTree(&root);

    preOrder(root);
    printf("\n");
    printf("%d", maxDepth(root));
    return 0;
}