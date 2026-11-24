# C++ 二叉树学习指南

> 本文以本目录下的 [BiTree.c](BiTree.c)、[BiThrTree.c](BiThrTree.c)、[test1.cpp](test1.cpp) 为基础，把二叉树最常用的一套结构和算法从头讲一遍。  
> 重点不是“背代码”，而是搞清楚：**每个结构为什么存在、它负责哪一步、换成别的写法会出什么问题**。
>
> 示例输入（先序序列，`#` 表示空结点）：
>
> ```text
> ABDH#K###E##CFI###G#J##
> ```
>
> 本文代码使用 C++11 及以上标准，可直接用 `g++ -std=c++17` 编译。

---

## 目录

1. [先建立全局图：每个结构负责什么](#1-先建立全局图每个结构负责什么)
2. [二叉树的基本概念](#2-二叉树的基本概念)
3. [结点结构：TreeNode](#3-结点结构treenode)
4. [建树：从先序序列还原二叉树](#4-建树从先序序列还原二叉树)
5. [四种遍历](#5-四种遍历)
6. [递归统计：深度、结点数、叶子数](#6-递归统计深度结点数叶子数)
7. [内存管理：树的销毁](#7-内存管理树的销毁)
8. [进阶：中序线索二叉树](#8-进阶中序线索二叉树)
9. [完整示例与运行结果](#9-完整示例与运行结果)
10. [复杂度与常见坑](#10-复杂度与常见坑)
11. [练习题](#11-练习题)
12. [一页速查](#12-一页速查)

---

## 1. 先建立全局图：每个结构负责什么

先看本文出现的所有“角色”，后面每一节都会展开：

| 结构 / 工具 | 它是什么 | 负责什么 | 所在章节 |
| --- | --- | --- | --- |
| `TreeNode` | 结构体 | 表示一个结点：存数据，并指向左、右两个孩子 | 第 3 节 |
| `buildTree` | 递归函数 | 把“先序序列 + `#`”解析成一棵真正的树 | 第 4 节 |
| 递归调用栈 | 语言运行时机制 | 自动保存“回到上一层”的现场，实现树上的分治 | 第 5.1 节 |
| `std::stack<TreeNode*>` | STL 容器 | 非递归深度优先遍历时，显式保存“回去的路” | 第 5.2 节 |
| `CircularQueue` | 自己写的类 | 层序遍历时的先进先出缓冲区 | 第 5.3 节 |
| `TreeNode*&` | 引用参数 | 让函数能修改调用者手里的根指针（如置空） | 第 4、7 节 |
| `ThrNode` + `head` | 结构体 + 哨兵结点 | 把空指针改造成前驱/后继线索，遍历时不用栈 | 第 8 节 |

一句话总结它们的关系：

- **树是递归定义的**，所以最自然的处理方式是递归；
- **递归是隐式地用栈**，所以非递归遍历要把这个栈显式写出来；
- **层序遍历按“层”推进**，所以用先进先出的队列；
- **线索树**则是把原本浪费的空指针利用起来，把“栈里记的东西”直接存进树里。

---

## 2. 二叉树的基本概念

### 2.1 递归定义

二叉树要么是**空树**，要么由一个**根结点**加上两棵互不相交的**左子树、右子树**组成，并且左右子树本身也都是二叉树。

这个定义直接决定了代码的写法：处理一棵树，就是处理“根 + 左子树 + 右子树”。

```
二叉树
├── 空树（nullptr）
└── 根结点
    ├── 左子树（也是二叉树）
    └── 右子树（也是二叉树）
```

### 2.2 几个术语

| 术语 | 含义 |
| --- | --- |
| 根（root） | 最上面的结点，整棵树唯一的入口 |
| 叶子（leaf） | 左右孩子都为空的结点 |
| 内部结点 | 至少有一个孩子的结点 |
| 深度 / 高度 | 本文约定：根所在层为第 1 层，空树深度为 0；深度 = 最长路径上的结点数 |
| 子树 | 某个结点及其所有后代组成的树 |

> 注意：二叉树的“左”和“右”是有顺序的。只有左孩子的结点，和只有右孩子的结点，是两棵不同的二叉树。

### 2.3 为什么用链式存储

二叉树有两种常见存法：

**顺序存储（数组）**：把结点按完全二叉树的编号放进数组，下标 `i` 的左孩子是 `2i+1`，右孩子是 `2i+2`（从 0 开始）。

- 优点：下标计算直接，缓存友好。
- 缺点：只适合**完全二叉树**。如果树很稀疏（比如一条链），数组里会出现大量空位，浪费空间。

**链式存储（指针）**：每个结点单独用 `new` 分配，结点里存左右孩子的地址。

- 优点：形状再奇怪也只占实际结点数的空间。
- 缺点：每个结点多两个指针；结点在内存中不连续，访问局部性差。

本文采用链式存储，因为它是二叉树最通用、最贴近“递归定义”的方式。

```
        root
          │
          ▼
     ┌───────────┐
     │ A │ left  │──► ...
     │   │ right │──► ...
     └───────────┘
```

---

## 3. 结点结构：TreeNode

### 3.1 代码

```cpp
struct TreeNode {
    char data;        // 数据域：这个结点存的值
    TreeNode* left;   // 左孩子指针：nullptr 表示没有左子树
    TreeNode* right;  // 右孩子指针：nullptr 表示没有右子树

    // 构造函数：new 出一个结点时，保证三个字段都有确定的值
    explicit TreeNode(char ch)
        : data(ch), left(nullptr), right(nullptr) {}

    // 一个结点只能有一个“所有者”，禁止拷贝，避免两个对象共享孩子指针
    TreeNode(const TreeNode&) = delete;
    TreeNode& operator=(const TreeNode&) = delete;
};
```

### 3.2 每个字段的功能

| 成员 | 功能 | 关键点 |
| --- | --- | --- |
| `data` | 存结点值 | 本文用 `char` 方便观察遍历顺序；实际项目可换成 `int`、`std::string` 或模板 `T` |
| `left` | 指向左子树根 | 为 `nullptr` 时表示左子树为空 |
| `right` | 指向右子树根 | 为 `nullptr` 时表示右子树为空 |
| 构造函数 | 初始化三个字段 | 防止 `left/right` 是随机地址（野指针） |

**`nullptr` 是整棵空树的表示。** 因此“一棵二叉树”在代码里就是一个 `TreeNode*`：

```cpp
TreeNode* root = nullptr;  // 空树
```

### 3.3 结构功能卡

> **`TreeNode` 的职责**
>
> - 它是一个**数据容器 + 两条边**：`data` 存值，`left/right` 存“去哪里找孩子”。
> - 它**不知道自己的父亲是谁**。所以普通遍历只能往下走；想往上走，要么用栈记住来路，要么用线索（第 8 节）。
> - 每个结点由 `new` 单独分配，结点的生命周期由程序员管理（第 7 节）。

### 3.4 和 C 版本的区别

在 [BiTree.c](BiTree.c) 里，结点是用 `malloc` 创建的：

```c
TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
node->data = ch;
node->left = NULL;
node->right = NULL;
```

C++ 的 `new TreeNode(ch)` 更安全：

1. 自动计算大小，不用写 `sizeof`；
2. 自动调用构造函数，不会漏初始化；
3. 失败时抛异常，而不是返回 `NULL`；
4. 配对使用 `delete`，而不是 `free`。

---

## 4. 建树：从先序序列还原二叉树

### 4.1 为什么“先序 + #”能唯一确定一棵树

先序遍历的顺序是：**根 → 左子树 → 右子树**。在读字符串时，第一个字符一定是根；读完根之后，接下来的一段是左子树，再接下来的一段是右子树。

但只有先序还不够：如果不知道左子树到哪里结束，就无法划分。`#` 的作用就是**空指针标记**，它明确告诉程序：“这里是一棵空子树，不要再往下读了”。

于是可以写出递归语法：

```text
Tree := '#'                 // 空树
      | 字符 Tree Tree       // 根 + 左子树 + 右子树
```

例如：

```text
A B D H # K # # # E # # C F I # # # G # J # #
```

按照语法解析，得到的树是：

```text
              A
            /   \
           B     C
          / \   / \
         D   E F   G
        /     /     \
       H     I       J
        \
         K
```

这棵树的四种遍历结果（后文会逐一验证）：

| 遍历方式 | 结果 |
| --- | --- |
| 先序（根左右） | `A B D H K E C F I G J` |
| 中序（左根右） | `H K D B E A I F C G J` |
| 后序（左右根） | `K H D E B I F J G C A` |
| 层序（一层一层） | `A` / `B C` / `D E F G` / `H I J` / `K` |

> 小检查：一棵有 `n` 个非空结点的二叉树，空指针域有 `n+1` 个，所以合法的先序 `#` 序列里应该有 `n+1` 个 `#`。本例 11 个结点，恰好 12 个 `#`。

### 4.2 建树代码

```cpp
#include <string>

// 参数：
//   preorder —— 先序序列，只读，用 const& 避免复制
//   index    —— 当前读到第几个字符；用引用让所有递归调用共享同一个游标
TreeNode* buildTree(const std::string& preorder, int& index) {
    // 防御：序列已经读完。合法输入不会走到这里；
    // 加上它可以避免越界访问，坏输入时得到 nullptr 而不是崩溃。
    if (index >= static_cast<int>(preorder.size())) {
        return nullptr;
    }

    char ch = preorder[index];  // 取出当前字符
    ++index;                    // 消费掉它：以后不再看这个位置

    if (ch == '#') {
        // 空标记：这棵子树是空树，递归到此结束
        return nullptr;
    }

    // 1. 根：创建当前结点
    TreeNode* node = new TreeNode(ch);

    // 2. 左子树：接下来的一段字符一定属于左子树
    node->left = buildTree(preorder, index);

    // 3. 右子树：左子树吃完后，剩下的第一段属于右子树
    node->right = buildTree(preorder, index);

    return node;  // 把建好的子树根返回给上一层
}
```

### 4.3 逐行解释关键点

#### 为什么返回 `TreeNode*`，而不是 `void buildTree(TreeNode** root)`？

两种写法等价。C 版本常用二级指针，是因为 C 没有“引用”：

```cpp
// 与上面等价的 C++ 写法（不推荐，但能帮助理解）
void buildTree(TreeNode** root, const std::string& preorder, int& index) {
    char ch = preorder[index++];
    if (ch == '#') {
        *root = nullptr;
        return;
    }
    *root = new TreeNode(ch);
    buildTree(&(*root)->left, preorder, index);
    buildTree(&(*root)->right, preorder, index);
}
```

`TreeNode**` 的意义是“让函数能修改调用者的指针变量”。返回值的写法把这件事交给 `return`，读起来更顺：

```cpp
node->left = buildTree(...);  // 返回值直接赋给左孩子
```

#### 为什么 `index` 必须传引用？

`index` 记录“整条序列读到哪了”，所有递归层次必须共用同一个位置。

```cpp
// 错误：传值。每一层都拿到 index 的副本，
// 子调用里 ++ 不会影响父调用，所有结点会重复读同一个字符。
TreeNode* buildTreeWrong(const std::string& s, int index) { ... }

// 正确：传引用。任何一层修改 index，其他层都能看见。
TreeNode* buildTree(const std::string& s, int& index) { ... }
```

有的人会用全局变量：

```cpp
int idx = 0;  // 不推荐
```

它虽然能工作，但缺点明显：

- 建第二棵树前必须手动重置 `idx`，否则会从上一棵树的末尾继续读；
- 多个函数共享隐藏状态，调试困难；
- 不可重入，多线程下会互相干扰。

> 你的 [test1.cpp](test1.cpp) 里 `creatTree` 使用了全局 `idx`。连续执行两次 `case 1` 时，第二次不会从 0 开始，这正是一个可以改进的点。把 `idx` 改成局部变量再传引用，就自然解决了。

#### 为什么 `preorder` 用 `const std::string&`？

- `&`：传引用，不复制整个字符串；
- `const`：函数只是读，不会修改输入，既表达意图，也让编译器帮你检查。

### 4.4 手工模拟一次建树

以 `A B # # C # #` 为例（对应 `A` 的左孩子 `B`、右孩子 `C`）：

| 步骤 | 调用 | 读到字符 | 发生的事情 |
| --- | --- | --- | --- |
| 1 | `buildTree(root)` | `A` | 创建 `A`，`index` 变成 1 |
| 2 | `buildTree(A->left)` | `B` | 创建 `B`，`index` 变成 2 |
| 3 | `buildTree(B->left)` | `#` | 返回 `nullptr`，`index` 变成 3 |
| 4 | `buildTree(B->right)` | `#` | 返回 `nullptr`，`index` 变成 4 |
| 5 | 回到 `A` | — | `A->left = B` |
| 6 | `buildTree(A->right)` | `C` | 创建 `C`，`index` 变成 5 |
| 7 | `buildTree(C->left)` | `#` | 返回 `nullptr` |
| 8 | `buildTree(C->right)` | `#` | 返回 `nullptr` |
| 9 | 回到 `A` | — | `A->right = C`，整棵树完成 |

注意：**`#` 也会消耗一个字符**。它不是“跳过”，而是代表一次真实的空子树递归。

### 4.5 建树结构功能卡

> **`buildTree` 的职责**
>
> - 它是先序序列的**递归下降解析器**：读根 → 建左 → 建右。
> - `index` 是解析器的“读指针”；`#` 是语法里的终止符。
> - 返回值是“这棵子树的根”，父结点用 `left/right` 接住它。
> - 时间复杂度 `O(n)`（每个字符处理一次），额外空间是递归栈 `O(h)`。

---

## 5. 四种遍历

遍历就是**按某种顺序访问每个结点一次**。“访问”不一定是打印，也可以是计数、修改、收集到数组、比较等。本文为了直观，统一用打印。

### 5.1 递归遍历：一副骨架，三个访问时机

#### 代码

```cpp
#include <iostream>

// 先序：根 → 左 → 右
void preOrder(TreeNode* root) {
    if (root == nullptr) {
        return;                 // 递归出口：空树直接返回
    }
    std::cout << root->data << ' ';  // ① 访问根
    preOrder(root->left);            // ② 访问左子树
    preOrder(root->right);           // ③ 访问右子树
}

// 中序：左 → 根 → 右
void inOrder(TreeNode* root) {
    if (root == nullptr) {
        return;
    }
    inOrder(root->left);             // ① 访问左子树
    std::cout << root->data << ' ';  // ② 访问根
    inOrder(root->right);            // ③ 访问右子树
}

// 后序：左 → 右 → 根
void postOrder(TreeNode* root) {
    if (root == nullptr) {
        return;
    }
    postOrder(root->left);           // ① 访问左子树
    postOrder(root->right);          // ② 访问右子树
    std::cout << root->data << ' ';  // ③ 访问根
}
```

#### 核心理解：结点会被“经过”三次

递归处理一个结点时，会有三个时刻：

```text
进入这个结点 ──► 左子树处理完 ──► 右子树处理完 ──► 返回上一层
     │                  │                 │
   前序访问            中序访问          后序访问
```

- **前序**：刚进入结点就访问（先处理根）；
- **中序**：左子树回来后访问（根夹在左右之间）；
- **后序**：右子树回来后访问（最后处理根）。

三种遍历的代码骨架完全一样，区别只是 `cout` 放在哪一行。理解了这一点，就不需要死记。

#### 调用栈示例

以 `A`（左孩子 `B`，右孩子 `C`）为例，执行 `preOrder(A)`：

```text
preOrder(A)
├─ 打印 A
├─ preOrder(B)
│  ├─ 打印 B
│  ├─ preOrder(nullptr) → 返回
│  └─ preOrder(nullptr) → 返回
└─ preOrder(C)
   ├─ 打印 C
   ├─ preOrder(nullptr) → 返回
   └─ preOrder(nullptr) → 返回
```

输出：`A B C`。

函数调用栈里保存着每一层的局部变量和返回地址。递归深度等于树的高度 `h`，所以空间复杂度是 `O(h)`。对于一条链一样的树，`h = n`，递归可能把线程栈撑爆（第 10 节会讲）。

#### 三种遍历的典型用途

| 遍历 | 输出特点 | 常见用途 |
| --- | --- | --- |
| 先序 | 根在最前 | 复制一棵树、序列化、打印目录结构、前缀表达式 |
| 中序 | 根在中间 | 二叉搜索树中得到有序序列、中缀表达式 |
| 后序 | 根在最后 | 释放整棵树、计算子树结果、后缀表达式、动态规划式统计 |

#### 结构功能卡

> **递归遍历的职责**
>
> - 递归函数本身就是“当前子树的处理器”；
> - 递归调用栈是**隐式的栈**，保存“左子树处理完后该回到哪里”；
> - 三种遍历的差异只在**访问根的时机**，不在递归结构。

---

### 5.2 非递归遍历：把隐式栈显式写出来

递归好用，但深度过大时有栈溢出风险。非递归遍历用 `std::stack<TreeNode*>` 手动保存“还没处理完的祖先”，把递归栈从线程栈搬到堆上。

> **`std::stack` 的功能**
>
> - 后进先出（LIFO）容器；
> - `push` 入栈、`top` 看栈顶、`pop` 弹出、`empty` 判空；
> - 在树里，栈中保存的是**结点指针**，不是结点本身。存指针才能保持结点身份，也避免整结构体拷贝。

#### 先序非递归

```cpp
#include <stack>

void preOrderIterative(TreeNode* root) {
    std::stack<TreeNode*> st;
    if (root != nullptr) {
        st.push(root);
    }

    while (!st.empty()) {
        TreeNode* node = st.top();
        st.pop();

        // 出栈即访问：先序是“根最先”
        std::cout << node->data << ' ';

        // 栈是后进先出：先压右孩子，再压左孩子，
        // 左孩子就会先出栈，从而满足“根 → 左 → 右”。
        if (node->right != nullptr) {
            st.push(node->right);
        }
        if (node->left != nullptr) {
            st.push(node->left);
        }
    }
}
```

为什么先压右再压左？因为栈顶先出。如果先压左再压右，右孩子会先被处理，顺序就变成了“根 → 右 → 左”。

#### 中序非递归

中序稍微绕一点：不能一上来就访问根，必须先把整条左链走到底。

```cpp
void inOrderIterative(TreeNode* root) {
    std::stack<TreeNode*> st;
    TreeNode* curr = root;

    while (curr != nullptr || !st.empty()) {
        // 一路向左，把沿途结点压栈。
        // 这些结点都是“自己还没访问、左子树也还没处理完”的祖先。
        while (curr != nullptr) {
            st.push(curr);
            curr = curr->left;
        }

        // 左边走到头了，栈顶就是当前最左的结点，可以访问它
        curr = st.top();
        st.pop();
        std::cout << curr->data << ' ';

        // 左子树和根都处理完了，转向右子树。
        // 如果右子树为空，下一轮会直接弹出它的父结点 ——
        // 父结点正是当前结点的中序后继。
        curr = curr->right;
    }
}
```

用 `A(B, C)` 模拟：

1. `curr = A`，压 `A`，走到 `B`；压 `B`，走到空；
2. 弹出 `B`，打印 `B`；`curr = B->right = nullptr`；
3. 栈非空，弹出 `A`，打印 `A`；`curr = C`；
4. 压 `C`，走到空；弹出 `C`，打印 `C`。

输出 `B A C`，正确。

#### 后序非递归

后序是“左 → 右 → 根”，难点在于：弹出栈顶后不能立刻访问，必须先确认右子树是否处理完。

```cpp
void postOrderIterative(TreeNode* root) {
    std::stack<TreeNode*> st;
    TreeNode* curr = root;
    TreeNode* lastVisited = nullptr;  // 上一个真正被访问（输出）的结点

    while (curr != nullptr || !st.empty()) {
        // 和前两种一样，先把左链压到底
        while (curr != nullptr) {
            st.push(curr);
            curr = curr->left;
        }

        TreeNode* top = st.top();

        // 右子树存在，且不是“刚刚访问过的那个结点”：
        // 说明右子树还没处理，先去右边。
        if (top->right != nullptr && top->right != lastVisited) {
            curr = top->right;
        } else {
            // 右子树为空，或者右子树刚刚处理完：现在可以访问根了
            std::cout << top->data << ' ';
            lastVisited = top;
            st.pop();
        }
    }
}
```

`lastVisited` 的作用：从右子树返回后，栈顶还是同一个结点。如果没有它，程序会再次进入右子树，陷入死循环。记录“上一个输出的是谁”，就能判断“右孩子是不是已经处理完了”。

用 `A(B, C)` 模拟：

1. 压 `A`、`B`，`B` 无右孩子 → 打印 `B`，`lastVisited = B`；
2. 栈顶 `A`，右孩子 `C` 不为空且不等于 `lastVisited` → 转向 `C`；
3. 压 `C`，`C` 无右孩子 → 打印 `C`，`lastVisited = C`；
4. 栈顶 `A`，右孩子 `C == lastVisited` → 打印 `A`。

输出 `B C A`，正确。

> 后序还有“双栈法”：用一个栈生成“根 → 右 → 左”，再整体反转。思路不同，结果一样，可以作为练习。

#### 结构功能卡

> **显式栈的职责**
>
> - 保存“已经走过、但还没处理完”的祖先结点；
> - 把递归的“返回地址”变成栈里的指针；
> - 三种非递归遍历的差别，是**什么时候访问、什么时候转向右子树**。

---

### 5.3 层序遍历：队列的舞台

层序遍历（广度优先遍历）按“离根的距离”一层一层访问：

```text
第 1 层：A
第 2 层：B C
第 3 层：D E F G
第 4 层：H I J
第 5 层：K
```

它需要**先进先出**：先发现的结点先处理，处理它时把它的孩子放到队尾。这个“排队”结构就是队列。

#### 为什么不用递归？

递归天然是深度优先的：它会一头扎进左子树，直到叶子才回来。层序要求“雨露均沾”，必须换一种推进方式。

#### 自己实现循环队列

下面这个 `CircularQueue` 与 [BiTree.c](BiTree.c) 中的队列思路一致，但做了两处改进：**元素类型是 `TreeNode*`**，**满了会自动扩容**。

```cpp
class CircularQueue {
public:
    explicit CircularQueue(int capacity = 8)
        : data_(new TreeNode*[capacity]),  // 存指针数组，不存结点本身
          capacity_(capacity),
          front_(0),
          rear_(0) {}

    ~CircularQueue() {
        delete[] data_;
    }

    // 队列拥有这块数组，禁止拷贝，避免两个对象重复 delete[]
    CircularQueue(const CircularQueue&) = delete;
    CircularQueue& operator=(const CircularQueue&) = delete;

    bool empty() const {
        return front_ == rear_;
    }

    int size() const {
        // 不能直接写 rear_ - front_：循环后差值可能是负数。
        // 加一个 capacity_ 再取模，才能得到正确长度。
        return (rear_ - front_ + capacity_) % capacity_;
    }

    void push(TreeNode* node) {
        // 循环队列牺牲一个空位来区分“空”和“满”：
        // 当 rear 的下一格就是 front 时，认为满了。
        if ((rear_ + 1) % capacity_ == front_) {
            grow();  // 满了就扩容，保证不会覆盖未出队的元素
        }
        data_[rear_] = node;
        rear_ = (rear_ + 1) % capacity_;  // 到末尾后绕回 0
    }

    TreeNode* pop() {
        if (empty()) {
            return nullptr;
        }
        TreeNode* node = data_[front_];
        front_ = (front_ + 1) % capacity_;  // 同样绕回
        return node;
    }

private:
    void grow() {
        int count = size();
        int newCapacity = capacity_ * 2;
        TreeNode** newData = new TreeNode*[newCapacity];

        // 把旧队列按“从 front 开始的逻辑顺序”搬到新数组的开头
        for (int i = 0; i < count; ++i) {
            newData[i] = data_[(front_ + i) % capacity_];
        }

        delete[] data_;
        data_ = newData;
        capacity_ = newCapacity;
        front_ = 0;    // 新数组从 0 开始
        rear_ = count; // 下一个写入位置
    }

    TreeNode** data_;  // 指针数组：每个元素是一个 TreeNode*
    int capacity_;     // 数组槽位总数
    int front_;        // 队首下标：第一个元素所在位置
    int rear_;         // 队尾下标：下一个元素要写入的位置
};
```

#### 每个字段的功能

| 字段 / 方法 | 功能 | 说明 |
| --- | --- | --- |
| `data_` | 存元素 | 类型是 `TreeNode**`，即“指针的数组”；每个槽位存一个结点地址 |
| `capacity_` | 数组容量 | 实际最多存 `capacity_-1` 个元素（牺牲一格判满） |
| `front_` | 队首 | 指向最早入队、下一个要出队的元素 |
| `rear_` | 队尾 | 指向下一个空槽位，新元素写在这里 |
| `empty()` | 判空 | `front_ == rear_` |
| `size()` | 求长度 | `(rear_ - front_ + capacity_) % capacity_` |
| `push()` | 入队 | 先判满，必要时扩容，再写 `rear_` 并后移 |
| `pop()` | 出队 | 取出 `front_`，再后移；空队列返回 `nullptr` |
| `grow()` | 扩容 | 按逻辑顺序搬到新数组，重置 `front_ = 0` |

#### 一个数字例子

设 `capacity_ = 8`，当前 `front_ = 6`，`rear_ = 2`。数组的 8 个槽位中，实际存着 4 个元素：

| 下标 | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 是否存元素 | 是 | 是 | 否 | 否 | 否 | 否 | 是 | 是 |
| 角色 | 第三个 | 逻辑末尾 | `rear_` | — | — | — | `front_`（第一个） | 第二个 |

- `front_ = 6` 指向第一个元素；
- `rear_ = 2` 指向下一个空槽位；
- 队列的逻辑顺序是 `6 → 7 → 0 → 1`（到末尾后绕回开头）；
- 逻辑末尾是下标 1 的元素，下标 2 是空位。

长度：

```text
(2 - 6 + 8) % 8 = 4
```

如果直接写 `rear_ - front_`，得到 `-4`，就错了。

#### 层序遍历代码

```cpp
void levelOrder(TreeNode* root) {
    if (root == nullptr) {
        return;
    }

    CircularQueue q;
    q.push(root);

    while (!q.empty()) {
        // 先记下“当前层”的结点数。
        // 因为在遍历当前层时，又会把下一层的孩子入队；
        // 不提前固定 levelSize，就分不清哪些结点属于哪一层。
        int levelSize = q.size();

        for (int i = 0; i < levelSize; ++i) {
            TreeNode* node = q.pop();
            std::cout << node->data << ' ';

            // 按从左到右的顺序把孩子加入队尾
            if (node->left != nullptr) {
                q.push(node->left);
            }
            if (node->right != nullptr) {
                q.push(node->right);
            }
        }

        std::cout << '\n';  // 一层结束，换行
    }
}
```

输出：

```text
A
B C
D E F G
H I J
K
```

#### 工程中的写法

自己实现队列适合学习，但实际项目通常直接用 STL：

```cpp
#include <queue>

std::queue<TreeNode*> q;
q.push(root);
while (!q.empty()) {
    TreeNode* node = q.front();
    q.pop();
    // ...
}
```

`std::queue` 会自动管理内存，不用手写 `front/rear` 和扩容。

#### 结构功能卡：队列

> **`CircularQueue` 的职责**
>
> - 提供先进先出（FIFO）的缓冲区，让结点按“发现顺序”处理；
> - 存的是 `TreeNode*`，不是 `TreeNode`。这样队列里放的是“结点的地址”，树结点本身不会被复制或覆盖；
> - `front/rear` 用取模实现循环复用，避免数组前面空出一大片；
> - `size()` 必须在每层开始时取一次，才能按层换行。

> **特别提醒：不要把队列元素写成 `TreeNode`**
>
> 你的 [BiTree.c](BiTree.c) 之前把队列元素定义成 `TreeNode`，并用 `&q->data[q->front]` 取数组槽位的地址当结点用。这样有两个问题：
>
> 1. 每次入队都会复制整个 `TreeNode`，多做了无意义的拷贝；
> 2. 循环队列绕回后，同一个槽位会被新元素覆盖，之前取出的“结点指针”指向的内容就变了，`left/right` 会被悄悄改掉。
>
> 正确做法是存 `TreeNode*`，并且从队列里取出的也应该是这个指针，而不是数组槽位的地址。

---

## 6. 递归统计：深度、结点数、叶子数

树上的统计题有一个统一套路：

> **整棵树的结果 = 左子树的结果 + 右子树的结果 + 当前结点自己的贡献**

再加上一个空树的边界值。写递归时先问自己三个问题：

1. **空树返回什么？**（递归出口）
2. **左右子树分别返回什么？**（子问题）
3. **怎么把两个结果和当前结点合并？**

### 6.1 树的深度

```cpp
#include <algorithm>

int treeDepth(TreeNode* root) {
    if (root == nullptr) {
        return 0;  // 空树深度为 0
    }

    int leftDepth = treeDepth(root->left);
    int rightDepth = treeDepth(root->right);

    // 左右子树中更深的那一个，加上当前这一层
    return std::max(leftDepth, rightDepth) + 1;
}
```

对于示例树：

```text
K 是叶子：深度 1
H 的右孩子是 K，左孩子空：深度 2
D 的左孩子是 H，右孩子空：深度 3
E 是叶子：深度 1
B 的左右是 D、E：深度 max(3,1)+1 = 4
I 是叶子：1；F 的左孩子是 I：2
J 是叶子：1；G 的右孩子是 J：2
C 的左右是 F、G：深度 max(2,2)+1 = 3
A 的左右是 B、C：深度 max(4,3)+1 = 5
```

所以 `treeDepth(root) == 5`。

> 注意：深度统计是**后序**思路——先算出左右子树，再合并。虽然它没有像后序遍历那样“打印”，但计算顺序完全一样。

### 6.2 结点总数

```cpp
int nodeCount(TreeNode* root) {
    if (root == nullptr) {
        return 0;
    }
    // 当前结点 1 个 + 左子树 + 右子树
    return 1 + nodeCount(root->left) + nodeCount(root->right);
}
```

示例树返回 `11`。

### 6.3 叶子结点数

```cpp
int leafCount(TreeNode* root) {
    if (root == nullptr) {
        return 0;  // 空树没有叶子
    }
    if (root->left == nullptr && root->right == nullptr) {
        return 1;  // 左右都空，当前就是叶子
    }
    // 不是叶子：叶子全在左右子树里
    return leafCount(root->left) + leafCount(root->right);
}
```

示例树的叶子是 `K、E、I、J`，返回 `4`。

> **顺序很重要**：必须先判断 `root == nullptr`，再访问 `root->left/right`。如果先判断叶子再判空，遇到空指针就会崩溃。
>
> 另外，叶子必须“左右都为空”。只判断 `left == nullptr` 会把“只有右孩子”的结点误认为叶子。

### 6.4 结构功能卡

> **递归统计的职责**
>
> - 每层只关心“当前结点 + 两个子问题的结果”；
> - 空树提供单位元（深度 0、结点数 0、叶子数 0）；
> - 统计顺序是后序：孩子的结果先出来，再合并到父结点。

---

## 7. 内存管理：树的销毁

### 7.1 为什么必须后序删除

树是由 `new` 一块块分配出来的。释放时必须**先释放孩子，再释放父结点**：

- 如果先 `delete root`，父结点对象就被销毁了，之后再访问 `root->left/right` 就是“释放后使用”（use-after-free）；同时孩子结点还没有被释放，又会造成内存泄漏；
- 后序顺序“左 → 右 → 根”保证删除父结点时，孩子已经全部回收。

```cpp
void destroyTree(TreeNode*& root) {
    if (root == nullptr) {
        return;
    }

    destroyTree(root->left);   // 先删左子树
    destroyTree(root->right);  // 再删右子树
    delete root;               // 最后删根
    root = nullptr;            // 防止调用者手里留下悬空指针
}
```

### 7.2 为什么参数是 `TreeNode*&`

```cpp
void destroyTree(TreeNode* root);   // 值传递
void destroyTree(TreeNode*& root);  // 引用传递
```

- 值传递：函数内 `delete root` 能释放内存，但调用者手里的 `root` 仍然指向那块已经释放的内存，成为**悬空指针**。如果之后又 `delete` 一次，就是重复释放。
- 引用传递：`root = nullptr` 修改的是调用者的变量，删除后调用者的根指针也会变成空，状态一致。

如果你希望保留原来的根指针不动，也可以只传 `TreeNode*`，但调用者必须自己记住“这棵树已经不能用了”。在 `destroyTree` 这种“销毁”语义下，传引用更安全。

### 7.3 内存管理清单

- 每个 `new` 必须恰好对应一个 `delete`；
- `new[]` 对应 `delete[]`（例如 `CircularQueue` 的数组）；
- 删除后把指针置 `nullptr`；
- 不要删除仍然被其他地方引用的结点；
- 不要复制一棵“裸指针树”：默认拷贝只复制指针，两棵树会共享同一批结点，最后会双重释放。

关于最后一点，下面这种写法是危险的：

```cpp
TreeNode a('A');
TreeNode b = a;  // 编译失败：本文已经 delete 了拷贝构造
                 // 如果允许拷贝，b.left 和 a.left 会指向同一个孩子，造成双重释放
```

因此本文的 `TreeNode` 禁止了拷贝。如果确实需要复制一棵树，应写一个**深拷贝**函数，递归地为每个结点 `new` 新对象。

### 7.4 结构功能卡

> **`destroyTree(TreeNode*&)` 的职责**
>
> - 用后序遍历回收所有结点；
> - 用引用参数把调用者的根指针同步置空；
> - 保证“先孩子后父亲”，避免访问已释放的内存。

---

## 8. 进阶：中序线索二叉树

### 8.1 动机：空指针别浪费

一棵有 `n` 个结点的二叉树，有 `n+1` 个空指针域。例如示例树有 11 个结点、12 个空指针。普通遍历完全无视这些空位，还需要额外的栈或递归。

**线索化（threading）**的想法是：

> 既然这些指针空着，不如让它们指向**中序遍历中的前驱或后继**。这样遍历时顺着线索走，就不需要栈了。

为了区分“这个指针指向真实孩子”还是“指向线索”，每个结点增加两个标志位 `ltag`、`rtag`。

### 8.2 标志位的语义

| 标志 | 值 | 指针的含义 |
| --- | --- | --- |
| `ltag` | `false` (0) | `left` 指向真实的左孩子 |
| `ltag` | `true` (1) | `left` 指向中序**前驱** |
| `rtag` | `false` (0) | `right` 指向真实的右孩子 |
| `rtag` | `true` (1) | `right` 指向中序**后继** |

> 不同教材可能用 0/1 的相反约定；关键是**全篇统一**。本文用 `bool`，`true` 表示“这是线索，不是孩子”。

### 8.3 结点的结构

```cpp
struct ThrNode {
    char data;
    ThrNode* left;
    ThrNode* right;
    bool ltag;  // true: left 是前驱线索；false: left 是左孩子
    bool rtag;  // true: right 是后继线索；false: right 是右孩子

    explicit ThrNode(char ch)
        : data(ch), left(nullptr), right(nullptr),
          ltag(false), rtag(false) {}  // 初始都当作“真实孩子”
};
```

**初始化非常重要。** 如果 `ltag/rtag` 是 `malloc` 出来的垃圾值，遍历时程序可能把随机地址当成线索，直接跳到非法内存。你之前在 [BiThrTree.c](BiThrTree.c) 里补上的初始化，正是为了避免这个问题。

### 8.4 用一个小例子理解线索

以 `A(B, C)` 为例，中序序列是 `B A C`。线索化后：

```text
              head
             /    \
       left /      \ right
           ▼        ▼
           A        C        （head.right 指向最后一个中序结点）
          / \
     left/   \right
        ▼     ▼
        B     C
```

详细的指针关系：

```text
head.left   ──► A            （真实入口）
head.right  ──► C            （中序最后一个结点）
B.left      ──► head         （B 是中序第一个结点，前驱是 head；ltag = true）
B.right     ──► A            （B 的后继是 A；rtag = true）
A.left      ──► B            （真实左孩子；ltag = false）
A.right     ──► C            （真实右孩子；rtag = false）
C.left      ──► A            （C 的前驱是 A；ltag = true）
C.right     ──► head         （C 是最后一个结点，后继是 head；rtag = true）
```

上方的树形图只画了 `head` 的两条边，`B.left` 和 `C.right` 回指 `head` 的线索没有画出来，以下面的指针关系为准。

注意：

- `B` 没有左孩子，所以 `B.left` 不再为 `nullptr`，而是指向 `head`；
- `C` 没有右孩子，所以 `C.right` 指向 `head`；
- `A` 的两个孩子都是真实的，所以 `A` 的 `ltag/rtag` 都是 `false`，指针含义不变。

### 8.5 头结点 `head` 的作用

`head` 是一个**不存有效数据的哨兵结点**，它的作用是让整棵线索树形成一个“循环链表”：

- `head.left` 指向根（空树时指向自己）；
- `head.right` 指向中序最后一个结点（空树时指向自己）；
- 中序第一个结点的左线索指向 `head`；
- 中序最后一个结点的右线索指向 `head`。

这样遍历时只需要判断“是否回到了 `head`”，不用在两端写一堆 `nullptr` 特判。

### 8.6 建立中序线索树

建树过程与普通树相同，只是结点类型换成 `ThrNode`：

```cpp
ThrNode* buildThrTree(const std::string& preorder, int& index) {
    if (index >= static_cast<int>(preorder.size())) {
        return nullptr;
    }
    char ch = preorder[index++];
    if (ch == '#') {
        return nullptr;
    }
    ThrNode* node = new ThrNode(ch);
    node->left = buildThrTree(preorder, index);
    node->right = buildThrTree(preorder, index);
    return node;
}
```

中序线索化的核心递归：

```cpp
// pre 始终指向“刚刚访问过的中序前驱”
void threadInOrder(ThrNode* node, ThrNode*& pre) {
    if (node == nullptr) {
        return;
    }

    // 中序：先处理左子树
    threadInOrder(node->left, pre);

    // 1. 当前结点没有左孩子：左指针改指前驱
    if (node->left == nullptr) {
        node->left = pre;
        node->ltag = true;
    }

    // 2. 前驱没有右孩子：前驱的右指针改指当前结点（即前驱的后继）
    if (pre->right == nullptr) {
        pre->right = node;
        pre->rtag = true;
    }

    // 3. 当前结点成为下一轮的“前驱”
    pre = node;

    // 4. 最后处理右子树
    threadInOrder(node->right, pre);
}
```

逐条解释：

1. **先递归左子树**：保证 `node` 被处理时，左子树内部已经线索化完毕，`pre` 正好是 `node` 的中序前驱。
2. **填左线索**：如果 `node` 没有左孩子，就让 `node->left` 指向前驱 `pre`。这是“前驱线索”。
3. **填右线索**：如果前驱 `pre` 没有右孩子，就让 `pre->right` 指向当前结点 `node`。因为 `node` 是 `pre` 的后继。注意这里修改的是**前驱**，不是当前结点。
4. **更新 `pre`**：当前结点处理完，成为下一次的“上一个结点”。
5. **递归右子树**：右子树内部继续同样的事情。

> **为什么递归右子树不会顺着刚建好的线索绕回去？**
>
> 填右线索这一步只修改 `pre->right`，不修改当前 `node->right`。当前结点的右指针要么是真实右孩子，要么是 `nullptr`；如果为空，递归 `threadInOrder(nullptr)` 立即返回。所以不会进入环。

### 8.7 创建带 head 的线索树

```cpp
ThrNode* createInOrderThreadTree(ThrNode* root) {
    // 1. 创建头结点。data 无意义，只作为哨兵。
    ThrNode* head = new ThrNode('#');
    head->ltag = false;  // head.left 将指向根，是“真实链接”
    head->rtag = true;   // head.right 是线索（指向最后一个结点，或指向自己）
    head->right = head;  // 空树时，右线索回指自己

    if (root == nullptr) {
        head->left = head;  // 空线索树：left 也回指自己
    } else {
        head->left = root;
        ThrNode* pre = head;        // 第一个结点的前驱是 head
        threadInOrder(root, pre);   // pre 最终停在最后一个中序结点

        // 2. 最后一个结点的右线索回指 head，形成环
        pre->right = head;
        pre->rtag = true;

        // 3. head.right 指向最后一个结点
        head->right = pre;
    }

    return head;
}
```

### 8.8 线索树的中序遍历（不用栈）

```cpp
void inOrderThreaded(ThrNode* head) {
    ThrNode* curr = head->left;  // 从根开始

    while (curr != head) {
        // 1. 一路向左，找到当前子树的“最左结点”
        while (!curr->ltag) {
            curr = curr->left;
        }

        // 2. 访问最左结点
        std::cout << curr->data << ' ';

        // 3. 顺着右线索连续访问后继；
        //    遇到 head 就停（最后一个结点的右线索指向 head）
        while (curr->rtag && curr->right != head) {
            curr = curr->right;
            std::cout << curr->data << ' ';
        }

        // 4. 如果 rtag 为 false，说明 right 是真实右孩子，进入右子树
        curr = curr->right;
    }
}
```

逐步理解：

- **往左下走**：`ltag == false` 表示 `left` 是真实左孩子，可以继续往左；`ltag == true` 表示 `left` 是前驱线索，不能再走，当前就是最左结点。
- **访问它**，然后看 `rtag`：如果 `rtag == true`，`right` 就是中序后继，可以直接跳过去继续访问；如果 `rtag == false`，说明有真实右子树，需要进入右子树，再由下一轮外层循环找到它的最左结点。
- 最后的 `curr = curr->right` 是统一的“下一步”：要么进入真实右子树，要么在最后一个结点处回到 `head`，结束循环。

对示例树，输出同样是：

```text
H K D B E A I F C G J
```

复杂度：

- 时间 `O(n)`：每个结点最多被访问常数次；
- 额外空间 `O(1)`：不需要栈或递归（不计输出）。

### 8.9 线索树的销毁

线索化之后，不能再用普通递归直接按 `left/right` 删除——那些指针可能指向 `head` 或前驱，会形成环。正确做法是**只沿真实孩子递归**：

```cpp
void destroyThrNodes(ThrNode* node) {
    if (node == nullptr) {
        return;
    }
    // 只有 tag 为 false 才是真实孩子；为 true 时是线索，不能跟过去
    if (!node->ltag) {
        destroyThrNodes(node->left);
    }
    if (!node->rtag) {
        destroyThrNodes(node->right);
    }
    delete node;
}

void destroyThreadedTree(ThrNode* head) {
    if (head == nullptr) {
        return;
    }
    if (head->left != head) {   // 非空树
        destroyThrNodes(head->left);
    }
    delete head;
}
```

### 8.10 结构功能卡：线索树

> **`ThrNode` + `head` 的职责**
>
> - `ThrNode` 在普通结点上增加 `ltag/rtag`，让空指针可以表示前驱/后继；
> - `head` 是哨兵，把中序序列首尾接成环，遍历时只判断“是否回到 head”；
> - 中序遍历不需要栈，额外空间 `O(1)`；
> - 线索化的代价是每个结点多两个标志位，并且**建树后不能再用普通递归遍历**（会顺着线索绕圈）。

---

## 9. 完整示例与运行结果

把第 3～8 节的代码按顺序放进同一个 `.cpp`，再加上下面的 `main`，即可编译运行（还需要 `#include <iostream>`、`<string>`、`<stack>`、`<algorithm>`）：

```cpp
int main() {
    const std::string input = "ABDH#K###E##CFI###G#J##";

    int index = 0;
    TreeNode* root = buildTree(input, index);

    std::cout << "先序：";
    preOrder(root);
    std::cout << '\n';

    std::cout << "中序：";
    inOrder(root);
    std::cout << '\n';

    std::cout << "后序：";
    postOrder(root);
    std::cout << '\n';

    std::cout << "先序（非递归）：";
    preOrderIterative(root);
    std::cout << '\n';

    std::cout << "中序（非递归）：";
    inOrderIterative(root);
    std::cout << '\n';

    std::cout << "后序（非递归）：";
    postOrderIterative(root);
    std::cout << '\n';

    std::cout << "层序：\n";
    levelOrder(root);

    std::cout << "深度：" << treeDepth(root) << '\n';
    std::cout << "结点数：" << nodeCount(root) << '\n';
    std::cout << "叶子数：" << leafCount(root) << '\n';

    destroyTree(root);  // 释放整棵树

    // 线索树演示
    int thrIndex = 0;
    ThrNode* thrRoot = buildThrTree(input, thrIndex);
    ThrNode* head = createInOrderThreadTree(thrRoot);

    std::cout << "线索树中序：";
    inOrderThreaded(head);
    std::cout << '\n';

    destroyThreadedTree(head);

    return 0;
}
```

编译命令：

```bash
g++ -std=c++17 -Wall -Wextra main.cpp -o main
```

预期输出：

```text
先序：A B D H K E C F I G J
中序：H K D B E A I F C G J
后序：K H D E B I F J G C A
先序（非递归）：A B D H K E C F I G J
中序（非递归）：H K D B E A I F C G J
后序（非递归）：K H D E B I F J G C A
层序：
A
B C
D E F G
H I J
K
深度：5
结点数：11
叶子数：4
线索树中序：H K D B E A I F C G J
```

如果递归版和三个非递归版的输出完全一致，说明遍历逻辑是正确的。这也是很好的自测方法。

---

## 10. 复杂度与常见坑

### 10.1 复杂度总表

设结点数为 `n`，树高为 `h`，最大宽度（某一层最多结点数）为 `w`。

| 操作 | 时间复杂度 | 额外空间 | 说明 |
| --- | --- | --- | --- |
| 建树 | `O(n)` | `O(h)` | 每个字符处理一次，递归栈高 `h` |
| 先序 / 中序 / 后序（递归） | `O(n)` | `O(h)` | 每结点访问一次，栈深 `h` |
| 先序 / 中序 / 后序（非递归） | `O(n)` | `O(h)` | 显式栈，最坏 `h = n` |
| 层序遍历 | `O(n)` | `O(w)` | 队列中最多同时存相邻两层的一部分，量级仍是 `O(w)` |
| 求深度 | `O(n)` | `O(h)` | 必须遍历所有结点 |
| 求结点数 / 叶子数 | `O(n)` | `O(h)` | 后序统计 |
| 销毁整棵树 | `O(n)` | `O(h)` | 后序删除 |
| 中序线索化 | `O(n)` | `O(h)` | 递归建立线索 |
| 线索树中序遍历 | `O(n)` | `O(1)` | 不需要栈 |

注意：

- 平衡树 `h = O(log n)`；退化成链时 `h = O(n)`，递归有栈溢出风险。
- 非递归遍历的栈在堆上，不受线程栈大小限制，但极端深树仍然会占用大量堆内存。

### 10.2 常见坑

#### 坑 1：队列存 `TreeNode` 而不是 `TreeNode*`

```cpp
// 错误：数组槽位存的是结点副本
TreeNode data[100];

// 正确：数组槽位存的是结点地址
TreeNode* data[100];
```

问题：

- 入队复制整个结构体，浪费；
- 用 `&q->data[q->front]` 把数组槽位当地址，循环队列绕回后槽位会被覆盖，树结点数据被改写。

正确做法：队列元素类型是 `TreeNode*`，存地址；取出后直接使用指针。

#### 坑 2：循环队列长度算成负数

```cpp
count = q->rear - q->front;  // 错误
```

`rear` 绕回 0 后可能小于 `front`，差值为负。应写：

```cpp
count = (q->rear - q->front + q->capacity) % q->capacity;
```

#### 坑 3：队列不判满

固定数组的循环队列必须判满，否则 `rear` 追上 `front` 会覆盖未出队元素，层序结果被静默算错。要么判满报错，要么像本文的 `grow()` 一样扩容。

#### 坑 4：建树用全局 `index` 且不重置

```cpp
int idx = 0;  // 全局
```

第二次建树时 `idx` 还停在上一次的末尾，读出越界或错误字符。改用局部变量 + 引用参数；如果非要用全局，至少每次建树前 `idx = 0`。

#### 坑 5：`#` 序列越界

如果输入不是一个合法的先序序列（例如 `#` 太少），递归会一直往下读，直到越界。本文的 `buildTree` 加了 `index >= size` 的防御；严格模式下还可以在建完后检查 `index == size`，确认没有多余字符。

#### 坑 6：忘记判断 `nullptr`

```cpp
void preOrder(TreeNode* root) {
    // 错误：没有出口，空指针继续解引用
    std::cout << root->data;
    preOrder(root->left);
    preOrder(root->right);
}
```

任何递归都必须先处理空树。叶子判断则要同时检查左右孩子。

#### 坑 7：把“只有一个孩子”误判为叶子

```cpp
if (root->left == nullptr) return 1;  // 错误：忽略了右孩子
```

叶子条件是：

```cpp
if (root->left == nullptr && root->right == nullptr) return 1;
```

#### 坑 8：后序非递归缺少 `lastVisited`

如果弹出栈顶后不判断右子树是否处理过，就会反复进入右子树，死循环。`lastVisited` 记录上一个访问的结点，用于判断“右孩子是不是刚处理完”。

#### 坑 9：线索标志未初始化

`ThrNode` 的 `ltag/rtag` 如果是随机值，遍历时可能把普通指针当线索，跳到非法地址。创建结点时一律初始化为 `false`（真实孩子），线索化时再按需改成 `true`。

#### 坑 10：线索树的右线索方向写反

- 左线索指**前驱**；
- 右线索指**后继**。

在线索化代码里，`node->left = pre` 是前驱；`pre->right = node` 是后继。不要写成 `node->right = pre`。

另外，最后一个结点的右线索应回指 `head`，修改的是 `rtag`，不是 `ltag`：

```cpp
pre->right = head;
pre->rtag = true;   // 正确
// pre->ltag = true; // 错误：会破坏最后一个结点的左孩子信息
```

#### 坑 11：线索化后还用普通递归遍历

普通 `preOrder/inOrder/postOrder` 看到的是 `left/right` 指针，不会检查 `ltag/rtag`。线索指针指向前驱/后继，可能形成环，递归会无限循环或栈溢出。线索树必须用线索遍历函数。

#### 坑 12：浅拷贝导致双重释放

如果类里只有裸指针却没有禁用拷贝，编译器生成的默认拷贝只会复制指针，两个 `TreeNode` 对象会共享同一批孩子。两个对象析构时会对同一块内存 `delete` 两次。解决方式：

- 禁止拷贝（本文的 `= delete`）；
- 或者实现深拷贝：递归 `new` 每个结点；
- 或者用 `std::unique_ptr` 管理所有权。

#### 坑 13：递归太深导致栈溢出

一条链状的树递归深度为 `n`。如果 `n` 很大（例如几十万），递归遍历可能崩溃。可以改用非递归遍历，或把问题改成迭代/显式栈。

---

## 11. 练习题

建议先自己写，再和前面的代码对照。

### 练习 1：统计度为 1 的结点

度为 1 = 只有一个孩子。提示：当前结点贡献 `1` 当且仅当“左空右不空”或“左不空右空”，然后加上左右子树的结果。

### 练习 2：交换每个结点的左右子树

提示：对每个结点交换 `left` 和 `right`，再递归处理两棵子树。先序、后序都可以，但要注意交换后递归的是原来的哪一边。

### 练习 3：求第 k 层的结点个数

提示：把“当前在第几层”作为参数往下传；到达第 `k` 层时返回 1，超过或空树返回 0。也可以层序遍历，数第 `k` 层的 `levelSize`。

### 练习 4：判断两棵树是否相同

提示：结构相同且对应结点的值相同。空树情况要匹配：

```text
两棵都空 → true
一棵空一棵不空 → false
都不空 → 值相等 且 左子树相同 且 右子树相同
```

### 练习 5：判断一棵二叉树是否完全二叉树

提示：完全二叉树按层序编号时，空结点只能出现在最后一层，且集中在右侧。可以用层序遍历，把 `nullptr` 也入队，遇到第一个 `nullptr` 后，后面不能再出现非空结点。注意队列元素要能存 `nullptr`。

### 练习 6：由前序 + 中序序列重建二叉树

输入不含 `#`，假设结点值互不重复，例如：

```text
前序：A B D H K E C F I G J
中序：H K D B E A I F C G J
```

提示：

- 前序第一个是根；
- 在中序里找到根的位置，左边是左子树的中序，右边是右子树的中序；
- 用长度切分前序，递归建左右子树；
- 用一个哈希表记录“值 → 中序下标”，可以把每次查找从 `O(n)` 降到 `O(1)`。

### 练习 7：后序非递归的“双栈法”

提示：用栈 1 按“根 → 右 → 左”的顺序生成结点，压入栈 2；最后把栈 2 依次弹出，就是“左 → 右 → 根”。

### 练习 8：在线索树中找前驱和后继

提示：

- 找 `node` 的中序后继：
  - 若 `rtag == true`，`right` 就是后继；
  - 若 `rtag == false`，后继是右子树的最左结点。
- 找中序前驱：
  - 若 `ltag == true`，`left` 就是前驱；
  - 若 `ltag == false`，前驱是左子树的最右结点。

### 练习 9：Morris 中序遍历

不用递归、不用栈，也不事先线索化，在遍历过程中临时利用叶子结点的空右指针建立线索，遍历完再恢复。提示：

- 对当前结点，先找左子树的最右结点；
- 如果它的右指针为空，让它指向当前结点，然后进入左子树；
- 如果它已经指向当前结点，说明左子树遍历完了，恢复为空，访问当前结点，再进入右子树。

这是把“线索”思想和“不修改树结构”结合起来的一种 `O(1)` 空间遍历。

---

## 12. 一页速查

### 结构职责

| 结构 | 一句话职责 |
| --- | --- |
| `TreeNode` | 存一个值 + 左右两条边；空树用 `nullptr` |
| `buildTree` | 按“根 → 左 → 右”的递归语法解析先序串，`#` 是空树 |
| 递归调用栈 | 隐式保存“回到哪一层”，实现分治 |
| `std::stack` | 非递归 DFS 的显式“来路记录” |
| `CircularQueue` | 层序 BFS 的 FIFO 缓冲区，存 `TreeNode*` |
| `ThrNode` | 用 `ltag/rtag` 标记指针是孩子还是线索 |
| `head` | 线索树的哨兵，把中序序列首尾接成环 |

### 遍历口诀

```text
前序：根 左 右   —— 进入就访问
中序：左 根 右   —— 左子树回来再访问
后序：左 右 根   —— 右子树回来再访问
层序：一层一层   —— 队列
```

### 写递归的三问

1. 空树返回什么？
2. 左右子树分别返回什么？
3. 当前结点怎么和两个结果合并？

### 建树与销毁

```text
建树：先序 + #，index 传引用，先根后左再右
销毁：后序删除，参数用 TreeNode*&，删除后置 nullptr
```

### 线索树

```text
没有左孩子 → left 指前驱，ltag = true
没有右孩子 → right 指后继，rtag = true
遍历：左链走到底 → 访问 → 顺右线索走 → 有真实右孩子就进去
```

---

> **最后建议**：二叉树的所有算法都可以回到两句话——
>
> 1. **树是递归的，先写好空树出口，再假设左右子树已经处理好。**
> 2. **需要“记住来路”就用栈，需要“按层推进”就用队列。**
>
> 先把本文的示例树在纸上画出来，手动模拟三种遍历和建树过程；能不看代码写出遍历，再去做练习题，效果最好。
