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

char expression[] = "82/2+56*-";

// 栈初始化
Stack* initStack() {
    Stack* stack = (Stack*)malloc(sizeof(Stack));
    stack->data = (int*)malloc(sizeof(int) * MAXSIZE);
    stack->top = -1;
    return stack;
}

void push(Stack* stack, int data) {
    stack->data[++stack->top] = data;
}
int pop(Stack* stack) {
    return stack->data[stack->top--];
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
void evaluateExpression(Stack* stack) {
    char symbol;  // 当前字符
    int operand1, operand2;
    int index = 0;
    ContentType contentType;  // 当前字符的类型
    contentType = getContentType(&symbol, &index);

    while (contentType != END_OF_EXPRESSION) {
        if (contentType == NUMBER) {
            // 处理数字
            push(stack, (symbol - '0'));  // 将数字入栈
        }
        else {
            // 处理运算符
            operand2 = pop(stack);
            operand1 = pop(stack);
            switch (contentType) {
                case ADDITION:
                    push(stack, operand1 + operand2);
                    break;
                case SUBTRACTION:
                    push(stack, operand1 - operand2);
                    break;
                case MULTIPLICATION:
                    push(stack, operand1 * operand2);
                    break;
                case DIVISION:
                    push(stack, operand1 / operand2);
                    break;
                case MODULUS:
                    push(stack, operand1 % operand2);
                    break;
            }
        }
        contentType = getContentType(&symbol, &index);
    }
    printf("%d", pop(stack));
}

void freeStack(Stack* stack) {
    free(stack->data);
    free(stack);
}

int main() {
    Stack* stack = initStack();
    evaluateExpression(stack);
    freeStack(stack);
    return 0;
}