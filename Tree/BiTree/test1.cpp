#include <iostream>
#include <string>

using std::cout;
using std::endl;

typedef struct TreeNode
{
    char data;
    struct TreeNode *lchild;
    struct TreeNode *rchild;
} TreeNode;

//  1
int idx = 0;
void creatTree(TreeNode **root, const std::string &str)
{
    if (str[idx] == '#')
    {
        *root = nullptr;
        idx++;
        return;
    }
    else
    {
        *root = new TreeNode;
        (*root)->data = str[idx++];
        creatTree(&(*root)->lchild, str);
        creatTree(&(*root)->rchild, str);
    }
}
//  2
void preOrder(TreeNode *root)
{
    if (root == nullptr)  return;
    cout << root->data << " ";
    preOrder(root->lchild);
    preOrder(root->rchild);
}
//  3
void inOrder(TreeNode *root)
{
    if (root == nullptr)  return;
    inOrder(root->lchild);
    cout << root->data << " ";
    inOrder(root->rchild);
}
//  4
void postOrder(TreeNode *root)
{
    if (root == nullptr)  return;
    postOrder(root->lchild);
    postOrder(root->rchild);
    cout << root->data << " ";
}
//  5
//  利用递归求二叉树的深度
int BTDepth(TreeNode *root)
{
    if (root == nullptr)  return 0;
    else
    {
        int leftDepth = BTDepth(root->lchild);
        int rightDepth = BTDepth(root->rchild);
        return (leftDepth > rightDepth) ? leftDepth + 1 : rightDepth + 1;
    }
}
//  6
//  利用递归求二叉树的叶子结点个数
int getLeafCount(TreeNode* root)
{
    if (root == nullptr)  return 0;
    if (root->lchild == nullptr && root->rchild == nullptr)  return 1;
    else
    {
        return getLeafCount(root->lchild) + getLeafCount(root->rchild);
    }
}
//  7
//  利用递归求二叉树的结点个数
int getNodeCount(TreeNode* root)
{
    if (root == nullptr)  return 0;
    else
    {
        return getNodeCount(root->lchild) + getNodeCount(root->rchild) + 1;
    }
}
int main()
{
    TreeNode *root = nullptr;
    int op;
    while (std::cin >> op)
    {
        switch (op)
        {
        case 1:
        //  switch内部定义变量，必须加大括号，否则编译器报错
        {
            std::string str = "";
            std::cin >> str;
            creatTree(&root, str);
            break;
        }
        case 2:
            preOrder(root);
            cout << endl;
            break;
        case 3:
            inOrder(root);
            cout << endl;
            break;
        case 4:
            postOrder(root);
            cout << endl;
            break;
        case 5:
            cout << BTDepth(root) << endl;
            break;
        case 6:
            cout << getLeafCount(root) << endl;
            break;
        case 7:
            cout << getNodeCount(root) << endl;
            break;
        case 0:
            return 0;
        default:
            break;
        }
    }
}