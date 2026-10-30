/* ============================================================================
 * reverse_Main.c —— 中缀表达式转后缀表达式（逆波兰式）
 *
 *   Main.c       : 后缀式 "82/2+56*-"      -> 求值
 *   reverse_Main : 中缀式 "x/(i-j)*y"      -> 转后缀 "xij-/y*"   （本文件）
 *
 * 下面用 【❌ 原错误代码】/【✅ 正确代码】对照的方式，逐处标注问题与原因。
 * ============================================================================ */
#include <stdio.h>
#include <stdlib.h>

#define MAXSIZE 100

typedef struct {
    int* data;
    int top;
} Stack;

typedef enum {
    LEFT_PARENTHESIS,  // 左括号
    RIGHT_PARENTHESIS, // 右括号
    ADDITION,          // 加法
    SUBTRACTION,       // 减法
    MULTIPLICATION,    // 乘法
    DIVISION,          // 除法
    MODULUS,           // 取余
    END_OF_EXPRESSION, // 表达式结束符
    NUMBER,            // 数字
} ContentType;

/* 栈内优先级 / 栈外优先级。
 * 关键：下标就是 ContentType 的值（0..8），不是字符编码。
 * 优先级越高越"紧"，'(' 的栈内优先级 0 表示"绝不被优先级比较弹出"。 */
int in_stack[]  = { 0, 19, 12, 12, 13, 13, 13, 0};
int out_stack[] = {20, 19, 12, 12, 13, 13, 13, 0};

char expression[] = "x/(i-j)*y";

/* ---------------------------------------------------------------------------
 * 新增：ContentType -> 可见字符 的翻译表
 *
 * 这是修 Bug 3 / Bug 10 的关键。原代码把两个不同的"域"混用了：
 *   - 优先级表的下标必须是 ContentType（0..8）
 *   - 输出到屏幕的必须是可见字符（'+' = 43）
 * 混用的直接后果就是拿字符编码去索引只有 8 个元素的数组，越界读内存。
 * ------------------------------------------------------------------------- */
char typeToSymbol(int type) {
    switch (type) {
        case LEFT_PARENTHESIS:  return '(';
        case RIGHT_PARENTHESIS: return ')';
        case ADDITION:          return '+';
        case SUBTRACTION:       return '-';
        case MULTIPLICATION:    return '*';
        case DIVISION:          return '/';
        case MODULUS:           return '%';
        case END_OF_EXPRESSION: return '\0';
        default:                return '?';
    }
}

Stack* initStack() {
    Stack* stack = (Stack*)malloc(sizeof(Stack));
    /* ❌ 原代码：没检查 malloc 返回值，分配失败后下一行 stack->data 就是对
     *    NULL 解引用，直接崩溃。
     * ✅ 正确做法：分配失败返回 NULL，由调用方判断。 */
    if (stack == NULL) return NULL;

    stack->data = (int*)malloc(sizeof(int) * MAXSIZE);
    /* ❌ 原代码：同样漏了 data 的分配检查。 */
    if (stack->data == NULL) {
        free(stack);
        return NULL;
    }
    stack->top = -1;
    return stack;
}

/* 新增：判空与取栈顶。原代码通篇直接写 data[top]，没有任何边界保护。 */
int isEmpty(Stack* stack) {
    return stack->top < 0;
}
int stackTop(Stack* stack) {
    return stack->data[stack->top];
}

void push(Stack* stack, int data) {
    /* ❌ 原代码：stack->data[++stack->top] = data; 没有任何容量检查。
     *    一旦元素超过 MAXSIZE，就会写穿 malloc 出来的 100 个 int，
     *    破坏堆内存，表现为程序在不同位置随机崩溃。
     * ✅ 加上防御（也可改成 realloc 动态扩容）。 */
    if (stack->top >= MAXSIZE - 1) {
        printf("\n[警告] 栈已满，元素被丢弃\n");
        return;
    }
    stack->data[++stack->top] = data;
}

/* ❌ 原代码：
 *      void pop(Stack* stack) {
 *          printf("%c", (char) stack->data[stack->top--]);
 *      }
 *    两个问题：
 *    1) 把"弹栈"和"打印"焊死成一个动作，导致处理右括号时想要"只弹不打印"
 *       却无路可走 —— 参见 Bug 6。
 *    2) 栈里存的是 ContentType(0..8)，强制转 char 打印出来是控制字符/乱码，
 *       根本不是 '+'、'-'。
 * ✅ 拆成两个函数：popSilent 只管弹，popAndPrint 负责弹出并输出。 */
void popSilent(Stack* stack) {
    if (isEmpty(stack)) return;
    stack->top--;
}

/* 弹出栈顶并按字符打印到结果串里；栈底哨兵不参与输出 */
void popAndPrint(Stack* stack) {
    if (isEmpty(stack)) return;
    int type = stack->data[stack->top--];
    if (type == END_OF_EXPRESSION) return;
    printf("%c", typeToSymbol(type));
}

int getContentType(char* symbol, int* index) {
   *symbol = expression[*index];
   (*index)++;
   switch (*symbol) {
       case '(':
           return LEFT_PARENTHESIS;
       case ')':
           return RIGHT_PARENTHESIS;
       case '+':
           return ADDITION;
       case '-':
           return SUBTRACTION;
       case '*':
           return MULTIPLICATION;
       case '/':
           return DIVISION;
       case '%':
           return MODULUS;
       case '\0':
           return END_OF_EXPRESSION;
       default:
           return NUMBER;
   }
}

void reverseExpression(Stack* stack) {
    char symbol;
    int index = 0;

    /* ❌ 原代码：
     *      stack->data[0] = END_OF_EXPRESSION;
     *    错误原因：只把哨兵写进了 data[0]，却没有同步 stack->top。
     *    initStack 里 top 仍是 -1，于是"数组里有值"而"栈认为自己是空的"。
     *    紧接着第一次 push 执行 ++top 变成 0，正好把哨兵覆盖掉，
     *    后面所有指望哨兵兜底的优先级比较全部失去依托。
     * ✅ 正确做法：哨兵必须真正"入栈"，让 top 与 data 保持一致。 */
    push(stack, END_OF_EXPRESSION);   /* 栈底哨兵，优先级 0，用于兜底比较 */

    ContentType contentType = getContentType(&symbol, &index);

    while (contentType != END_OF_EXPRESSION) {

        if (contentType == NUMBER) {
            /* ❌ 原代码：
             *      pop(stack);
             *    错误原因：操作数根本不该经过栈。而此刻要么 top = -1
             *    （哨兵没入栈），要么栈顶是哨兵，于是 data[-1] 越界读，
             *    或者把哨兵当成操作数打印出来。
             * ✅ 正确做法：操作数直接输出。
             *    注意：本实现和原代码一样只支持单字符操作数，
             *    若要支持 "12+3" 这类多数位数字，这里需要循环读取连续数字。 */
            printf("%c", symbol);
        }
        else if (contentType == RIGHT_PARENTHESIS) {
            /* ❌ 原代码：
             *      while (stack->data[stack->top] != '(') {
             *          pop(stack);
             *      } pop(stack);
             *    错误原因有三：
             *    1) 第二个 pop 会连 '(' 一起打印出来 —— 后缀表达式里不允许出现括号；
             *    2) 循环没有下溢保护，括号不匹配时 top 一路减成负数，越界读内存；
             *    3) data[top] 直接和 '(' 比较，前提是栈里存的是字符，
             *       而查优先级表时又要求栈里存的是 ContentType，前后不一致。
             * ✅ 正确做法：不断弹出并输出直到遇见 '('，再"静默"弹掉它，并加判空。 */
            while (!isEmpty(stack) && stackTop(stack) != LEFT_PARENTHESIS) {
                popAndPrint(stack);
            }
            popSilent(stack);          /* 弹掉 '('，但不输出 */
        }
        else if (contentType == LEFT_PARENTHESIS) {
            /* ❌ 原代码没有单独处理左括号，靠 else 分支里
             *    out_stack[...] > in_stack[栈顶] 侥幸压栈成功。
             *    但那个 else 分支里带着 pop 操作，而 '(' 是绝不能被弹出的。
             * ✅ 显式处理：左括号无条件入栈。 */
            push(stack, contentType);
        }
        else {
            /* ❌ 原代码：
             *      if(out_stack[MODULUS] > in_stack[stack->data[stack->top - '0']]) {
             *          push(stack, symbol);
             *      } else {
             *          pop(stack);
             *          push(stack, symbol);
             *      }
             *    错误原因（5 条，这一行是整个文件最严重的地方）：
             *    1) out_stack[MODULUS] 把"当前运算符的优先级"写死成 '%' 的 13，
             *       遇到 '+'、'-' 也按 13 去比，判断结果永远是错的。
             *       应为 out_stack[contentType]。
             *    2) stack->data[stack->top - '0']：top 是数组下标，减字符
             *       '0'(=48) 得到负数下标，直接越界读 in_stack[]。
             *    3) 即使把下标改成 stack->data[stack->top]，栈里存的是字符编码
             *       （'(' = 40，'/' = 47），拿它索引长度只有 8 的优先级表同样越界。
             *       必须先翻译成 ContentType 再查表。
             *    4) 只 pop 一次不够。算符优先的规则是"栈顶优先级 >= 当前优先级
             *       就【反复】弹出并输出"，只弹一个会让 a-b*c+d 这类表达式
             *       排出错误的后缀式（正确 abc*-d+，只弹一次会得到 abc*d+-）。
             *    5) push(stack, symbol) 压入的是字符，与优先级表的 ContentType
             *       域不一致，为下一次比较埋下隐患。
             * ✅ 正确做法：反复弹出所有优先级 >= 当前运算符的栈顶元素，
             *    再把当前运算符入栈。哨兵 in_stack = 0，天然让循环停下。 */
            while (!isEmpty(stack) &&
                   in_stack[stackTop(stack)] >= out_stack[contentType]) {
                popAndPrint(stack);
            }
            push(stack, contentType);
        }

        contentType = getContentType(&symbol, &index);
    }

    /* ❌ 原代码缺失这一步：
     *    循环读到 '\0' 就直接退出了，栈里剩余的运算符一个都没输出。
     *    例如 "x/(i-j)*y" 中最后的 '*' 永远不会被打印，后缀式是残缺的。
     * ✅ 正确做法：收尾时把栈中剩余运算符全部弹出输出（哨兵除外）。 */
    while (!isEmpty(stack) && stackTop(stack) != END_OF_EXPRESSION) {
        popAndPrint(stack);
    }
    popSilent(stack);      /* 弹掉栈底哨兵 */
    printf("\n");
}

void freeStack(Stack* stack) {
    if (stack == NULL) return;
    free(stack->data);
    free(stack);
}

int main() {
    Stack* stack = initStack();
    if (stack == NULL) {          /* 对应 initStack 里新增的失败返回 */
        printf("栈初始化失败\n");
        return 1;
    }

    printf("中缀表达式: %s\n", expression);
    printf("后缀表达式: ");
    reverseExpression(stack);     /* 内部打印结果并换行 */

    freeStack(stack);
    return 0;
}
