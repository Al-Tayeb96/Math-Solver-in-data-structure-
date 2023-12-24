//Name :ALTAYEB ALFAYAA
//ID:1225025
//SECTION :3
//INS:Saadeddin Hroub
//DATE:20/12/2023,8:33PM

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#define MAX_EXPRESSION_LENGTH 100
#define MAX_EQUATIONS 10

typedef struct Node {
    char operator;
    double operand;
    struct Node* left;
    struct Node* right;
} Node;

typedef struct StackNode {
    Node* node;
    struct StackNode* next;
} StackNode;

void push(StackNode** stack, Node* node) {
    StackNode* temp = (StackNode*)malloc(sizeof(StackNode));
    if (temp == NULL) {
        printf("Out of space!");
        exit(EXIT_FAILURE);
    }
    temp->node = node;
    temp->next = *stack;
    *stack = temp;
}

Node* pop(StackNode** stack) {
    if (*stack == NULL) {
        printf("Empty Stack!");
        return NULL;
    }

    Node* node = (*stack)->node;
    StackNode* temp = *stack;
    *stack = (*stack)->next;
    free(temp);
    return node;
}

int is_operator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '%';
}

int get_priority(char op) {
    switch (op) {
    case '+':
    case '-':
        return 1;
    case '*':
    case '/':
    case '%':
        return 2;
    default:
        return 0;
    }
}

Node* create_node(const char* expression, int* i) {
    char value = expression[*i];
    if (isdigit(value)) {
        int num = value - '0';
        while (isdigit(expression[*i + 1])) {
            num = num * 10 + (expression[*i + 1] - '0');
            ++(*i);
        }

        Node* new_node = (Node*)malloc(sizeof(Node));
        if (new_node == NULL) {
            printf("Out of space!");
            exit(EXIT_FAILURE);
        }
        new_node->operator = '\0';  // Null character to indicate it's not an operator
        new_node->operand = num;
        new_node->left = NULL;
        new_node->right = NULL;
        return new_node;
    }
    else {
        Node* new_node = (Node*)malloc(sizeof(Node));
        if (new_node == NULL) {
            printf("Out of space!");
            exit(EXIT_FAILURE);
        }
        new_node->operator = value;
        new_node->operand = 0.0;  // Default value for non-operand nodes
        new_node->left = NULL;
        new_node->right = NULL;
        return new_node;
    }
}
// function builds an expression tree using two stacks (operand stack and operator stack) 
// to handle different elements of the expression while maintaining proper order of operations.
Node* build_expression_tree(const char* expression) {
    StackNode* operand_stack = NULL;
    StackNode* operator_stack = NULL;

    for (int i = 0; expression[i] != '\0'; ++i) {
        if (expression[i] == ' ') {
            continue;
        }// Skip spaces

        if (isdigit(expression[i]) || (expression[i] == '-' && (i == 0 || is_operator(expression[i - 1]) || expression[i - 1] == '('))) {
            // If the current character is a digit or a negative sign indicating a negative number
            Node* operand_node = create_node(expression, &i);
            push(&operand_stack, operand_node);
        }
        else if (is_operator(expression[i])) {
            // If the current character is an operator
            while (operator_stack != NULL &&
                get_priority(operator_stack->node->operator) >= get_priority(expression[i])) {
                Node* op_node = pop(&operator_stack);
                Node* right_operand = pop(&operand_stack);
                Node* left_operand = pop(&operand_stack);

                op_node->right = right_operand;
                op_node->left = left_operand;

                push(&operand_stack, op_node);
            }

            Node* operator_node = create_node(expression, &i);
            push(&operator_stack, operator_node);
        }
        else if (expression[i] == '(') {
            Node* operator_node = create_node(expression, &i);
            push(&operator_stack, operator_node);
        }
        else if (expression[i] == ')') {
            while (operator_stack != NULL && operator_stack->node->operator != '(') {
                Node* op_node = pop(&operator_stack);
                Node* right_operand = pop(&operand_stack);
                Node* left_operand = pop(&operand_stack);

                op_node->right = right_operand;
                op_node->left = left_operand;

                push(&operand_stack, op_node);
            }

            pop(&operator_stack);
        }
    }

    while (operator_stack != NULL) {
        Node* op_node = pop(&operator_stack);
        Node* right_operand = pop(&operand_stack);
        Node* left_operand = pop(&operand_stack);

        op_node->right = right_operand;
        op_node->left = left_operand;

        push(&operand_stack, op_node);
    }

    return pop(&operand_stack);
}

//function recursively evaluates an expression tree. If the current node is an operand, it returns the operand's value. 
// If it's an operator, it recursively evaluates the left and right operands and performs
//  the corresponding operation. Error handling is included for division by zero, modulo by zero, and unknown operators.
double evaluate_expression_tree(Node* root) {
    if (root == NULL) {
        return 0.0;
    }
    // Check if the current node represents an operand (leaf node)

    if (root->operator == '\0') {
        return root->operand;
    }
    else {
        // If it's an operator, recursively evaluate the left and right operands
        double left_operand = evaluate_expression_tree(root->left);
        double right_operand = evaluate_expression_tree(root->right);

        switch (root->operator) {
        case '+':
            return left_operand + right_operand;
        case '-':
            return left_operand - right_operand;
        case '*':
            return left_operand * right_operand;
        case '/':
            if (right_operand != 0.0) {
                return (int)(left_operand / right_operand);
            }
            else {
                printf("Error: Division by zero\n");
                exit(EXIT_FAILURE);
            }
        case '%':
            if (right_operand != 0.0) {
                return fmod(left_operand, right_operand);
            }
            else {
                printf("Error: Modulo by zero\n");
                exit(EXIT_FAILURE);
            }
        default:
            printf("Error: Unknown operator '%c'\n", root->operator);
            exit(EXIT_FAILURE);
        }
    }
}
// function recursively prints the infix expression of an expression tree to a specified output file
void print_postfix(Node* root, FILE* output) {
    if (root != NULL) {
        if (is_operator(root->operator)) {
            fprintf(output, "(");
        }

        print_postfix(root->left, output);
        if (!is_operator(root->operator)) {
            fprintf(output, "%.0f", root->operand);
        }
        else {
            fprintf(output, "%c", root->operator);
        }
        print_postfix(root->right, output);

        if (is_operator(root->operator)) {
            fprintf(output, ")");
        }
    }
}

void free_tree(Node* root) {
    if (root != NULL) {
        free_tree(root->left);
        free_tree(root->right);
        free(root);
    }
}
//function read equations from input file 
void readEquationsFromFile(const char* fileName, char equations[][MAX_EXPRESSION_LENGTH], int* equationCount) {
    FILE* file = fopen(fileName, "r");
    if (file == NULL) {
        printf("Error opening file: %s\n", fileName);
        exit(EXIT_FAILURE);
    }

    *equationCount = 0;

    while (fgets(equations[*equationCount], MAX_EXPRESSION_LENGTH, file) != NULL) {
        size_t length = strlen(equations[*equationCount]);
        if (equations[*equationCount][length - 1] == '\n') {
            equations[*equationCount][length - 1] = '\0';
        }

        (*equationCount)++;

        if (*equationCount >= MAX_EQUATIONS) {
            printf("Maximum number of equations reached.\n");
            break;
        }
    }

    fclose(file);
}

void printEquations(char equations[][MAX_EXPRESSION_LENGTH], int equationCount) {
    printf("Equations:\n");
    for (int i = 0; i < equationCount; ++i) {
        printf("%s\n", equations[i]);
    }
}

//function save resultes in to output file inclodeing the equation and results and the postfix
void saveToOutputFile(char equations[][MAX_EXPRESSION_LENGTH], double results[], int equationCount, const char* outputFileName) {
    FILE* outputFile = fopen(outputFileName, "w");

    if (outputFile == NULL) {
        printf("Error opening output file: %s\n", outputFileName);
        exit(EXIT_FAILURE);
    }

    fprintf(outputFile, "Equations and Results:\n");
    for (int i = 0; i < equationCount; ++i) {
        fprintf(outputFile, "%s = %.0f ----> Postfix: ", equations[i], results[i]);
        print_postfix(build_expression_tree(equations[i]), outputFile);
        fprintf(outputFile, "\n");
        free_tree(build_expression_tree(equations[i]));  
    }

    fclose(outputFile);
}


// Function prototypes
int is_operator(char c);
int get_priority(char op);
Node* build_expression_tree(const char* expression);
double evaluate_expression_tree(Node* root);
void print_postfix(Node* root, FILE* output);
void free_tree(Node* root);
void readEquationsFromFile(const char* fileName, char equations[][MAX_EXPRESSION_LENGTH], int* equationCount);
void printEquations(char equations[][MAX_EXPRESSION_LENGTH], int equationCount);
void saveToOutputFile(char equations[][MAX_EXPRESSION_LENGTH], double results[], int equationCount, const char* outputFileName);

int main() {
    char equations[MAX_EQUATIONS][MAX_EXPRESSION_LENGTH];
    double results[MAX_EQUATIONS];
    int equationCount = 0;

    while (1) {
        printf("\nMenu:\n");
        printf("1. Read equations\n");
        printf("2. Print equations\n");
        printf("3. Evaluate using Expression tree\n");
        printf("4. Print postfix expressions\n");
        printf("5. Save to output file\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");

        int choice;
        scanf_s("%d", &choice);

        switch (choice) {
        case 1:
            readEquationsFromFile("equations.txt", equations, &equationCount);
            printf("Done!!.\n");
            break;
        case 2:
            printEquations(equations, equationCount);
            printf("Done!!.\n");
            break;
        case 3:
            for (int i = 0; i < equationCount; ++i) {
                Node* root = build_expression_tree(equations[i]);
                results[i] = evaluate_expression_tree(root);
                free_tree(root);
            }
            printf("Equations evaluated.\n");
            break;
        case 4:
            for (int i = 0; i < equationCount; ++i) {
                Node* root = build_expression_tree(equations[i]);
                printf("postfix expression %d: ", i + 1);
                print_postfix(root, stdout);  
                printf("\n");
                free_tree(root);
            }
            printf("Done!!.\n");

            break;
        case 5:
            saveToOutputFile(equations, results, equationCount, "output.txt");
            printf("Done!!.\n");
            break;
        case 6:
            return 0;
        default:
            printf("Invalid choice. Please enter a valid option.\n");
        }
    }

    return 0;
}
