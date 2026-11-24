#include <iostream>
#include <string>

struct TreeNode
{
    char data;
    TreeNode* left;
    TreeNode* right;

    // 构造函数：new 出一个结点时，保证三个字段都有确定的值
    explicit TreeNode(char ch)
        : data(ch), left(nullptr), right(nullptr) {}
};

constexpr int MAXSIZE = 8;
struct CircularQueue
{
    TreeNode** data_;
    int front_;
    int rear_;

    CircularQueue () : data_(new TreeNode*[MAXSIZE]), front_(0), rear_(0) {}
};

TreeNode* buildTree (const std::string& preorder, int& index)
{
    // 防御：序列已经读完。合法输入不会走到这里；
    // 加上它可以避免越界访问，坏输入时得到 nullptr 而不是崩溃。
    if (index >= static_cast<int>(preorder.size()))
        return nullptr;
    
    char ch = preorder[index++];

    if (ch == '#') {
        // 空标记：这棵子树是空树，递归到此结束
        return nullptr;
    }

    //  node 作为 当前子树 的根
    TreeNode* node = new TreeNode(ch);
    node->left = buildTree(preorder, index);
    node->right = buildTree(preorder, index);

    return node;  // 把建好的子树根返回给上一层
}

CircularQueue* InitQueue () { return new CircularQueue; }
void enQueue (CircularQueue* Q, TreeNode* root)
{
    Q->data_[Q->rear_] = root;
    Q->rear_ = ((Q->rear_) + 1) % MAXSIZE; 
}

void preOrder(TreeNode* root) 
{
    if (root == nullptr) {
        return;
    }
    std::cout << root->data << ' ';
    preOrder(root->left);
    preOrder(root->right);
}

int levelOrder(TreeNode* root)
{
    if (root == nullptr)
        return 0;

    CircularQueue* Q = InitQueue();
    enQueue(Q, root);

    int depth = 0;
    int count = 1; // 当前层待处理的节点数

    while (count > 0)
    {
        ++depth;
        int nextCount = 0;

        for (int i = 0; i < count; ++i)
        {
            TreeNode* node = Q->data_[Q->front_];
            Q->front_ = (Q->front_ + 1) % MAXSIZE;

            std::cout << node->data << ' ';

            if (node->left != nullptr)
            {
                enQueue(Q, node->left);
                ++nextCount;
            }
            if (node->right != nullptr)
            {
                enQueue(Q, node->right);
                ++nextCount;
            }
        }
        std::cout << '\n';
        count = nextCount;
    }
    std::cout << "===============" << '\n';
    delete[] Q->data_;
    delete Q;
    return depth;
}

int main ()
{
    std::string proorder {"ABDH#K###E##CFI###G#J##"};
    int index = 0;
    TreeNode* root = buildTree(proorder, index);

    preOrder(root);
    std::cout << '\n'
    << "=====================" <<std::endl;
    std::cout << levelOrder(root);
    
    return 0;
}