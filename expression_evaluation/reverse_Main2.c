#include <stdio.h>
#include <stdlib.h>

#define MAXSIZE 100

typedef struct {
    int* data;
    int top;
} Stack;

typedef enum {
    LEFT_PARENTHESIS,
    RIGHT_PARENTHESIS, 
    ADDITION,        
    SUBTRACTION,      
    MULTIPLICATION,  
    DIVISION,          
    MODULUS,          
    END_OF_EXPRESSION, 
    NUMBER,            
} ContentType;
int in_stack[]  = { 0, 19, 12, 12, 13, 13, 13, 0};
int out_stack[] = {20, 19, 12, 12, 13, 13, 13, 0};

char expression[] = "a%b+c/d";

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

void reverseExpression(Stack* stack) {
    char symbol;  
    int index = 0;
    push(stack, END_OF_EXPRESSION);
    ContentType contentType;  
    contentType = getContentType(&symbol, &index);

    while (contentType != END_OF_EXPRESSION) {
        if (contentType == NUMBER) {
            printf("%c", symbol);
        }
        else if (contentType == RIGHT_PARENTHESIS) {
            while (stack->data[stack->top] != LEFT_PARENTHESIS) {
                printf("%c", typeToSymbol(pop(stack)));
            }
            pop(stack);
        } else {
            while (out_stack[contentType] <= in_stack[stack->data[stack->top]]) {
                printf("%c", typeToSymbol(pop(stack)));
            }
            push(stack, contentType);
            }
    contentType = getContentType(&symbol, &index);
    }
    printf("%c", typeToSymbol(pop(stack)));
}

void freeStack(Stack* stack) {
    free(stack->data);
    free(stack);
}

int main() {
    Stack* stack = initStack();
    reverseExpression(stack);
    freeStack(stack);
    return 0;
}