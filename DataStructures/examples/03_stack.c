/*
 * 03_stack.c
 * 
 * Stack implementation (LIFO - Last In, First Out).
 * Shows both array-based and linked list-based implementations.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ===== Array-Based Stack =====

typedef struct ArrayStack {
    int* items;
    int top;
    int capacity;
} ArrayStack;

ArrayStack* create_array_stack(int capacity) {
    ArrayStack* stack = (ArrayStack*)malloc(sizeof(ArrayStack));
    stack->items = (int*)malloc(capacity * sizeof(int));
    stack->top = -1;
    stack->capacity = capacity;
    return stack;
}

int is_full_array(ArrayStack* stack) {
    return stack->top == stack->capacity - 1;
}

int is_empty_array(ArrayStack* stack) {
    return stack->top == -1;
}

void push_array(ArrayStack* stack, int item) {
    if (is_full_array(stack)) {
        printf("Stack overflow!\n");
        return;
    }
    stack->items[++stack->top] = item;
}

int pop_array(ArrayStack* stack) {
    if (is_empty_array(stack)) {
        printf("Stack underflow!\n");
        return -1;
    }
    return stack->items[stack->top--];
}

int peek_array(ArrayStack* stack) {
    if (is_empty_array(stack)) {
        return -1;
    }
    return stack->items[stack->top];
}

void free_array_stack(ArrayStack* stack) {
    free(stack->items);
    free(stack);
}

// ===== Linked List-Based Stack =====

typedef struct StackNode {
    int data;
    struct StackNode* next;
} StackNode;

typedef struct LinkedStack {
    StackNode* top;
    int size;
} LinkedStack;

LinkedStack* create_linked_stack() {
    LinkedStack* stack = (LinkedStack*)malloc(sizeof(LinkedStack));
    stack->top = NULL;
    stack->size = 0;
    return stack;
}

int is_empty_linked(LinkedStack* stack) {
    return stack->top == NULL;
}

void push_linked(LinkedStack* stack, int data) {
    StackNode* new_node = (StackNode*)malloc(sizeof(StackNode));
    new_node->data = data;
    new_node->next = stack->top;
    stack->top = new_node;
    stack->size++;
}

int pop_linked(LinkedStack* stack) {
    if (is_empty_linked(stack)) {
        printf("Stack underflow!\n");
        return -1;
    }
    
    StackNode* temp = stack->top;
    int data = temp->data;
    stack->top = stack->top->next;
    free(temp);
    stack->size--;
    return data;
}

int peek_linked(LinkedStack* stack) {
    if (is_empty_linked(stack)) {
        return -1;
    }
    return stack->top->data;
}

void free_linked_stack(LinkedStack* stack) {
    while (!is_empty_linked(stack)) {
        pop_linked(stack);
    }
    free(stack);
}

// ===== Stack Applications =====

// Check balanced parentheses
int is_balanced(const char* expr) {
    ArrayStack* stack = create_array_stack(100);
    
    for (int i = 0; expr[i] != '\0'; i++) {
        if (expr[i] == '(' || expr[i] == '{' || expr[i] == '[') {
            push_array(stack, expr[i]);
        } else if (expr[i] == ')' || expr[i] == '}' || expr[i] == ']') {
            if (is_empty_array(stack)) {
                free_array_stack(stack);
                return 0;
            }
            
            int top = pop_array(stack);
            if ((expr[i] == ')' && top != '(') ||
                (expr[i] == '}' && top != '{') ||
                (expr[i] == ']' && top != '[')) {
                free_array_stack(stack);
                return 0;
            }
        }
    }
    
    int result = is_empty_array(stack);
    free_array_stack(stack);
    return result;
}

// Reverse a string
void reverse_string(char* str) {
    ArrayStack* stack = create_array_stack(strlen(str));
    
    // Push all characters
    for (int i = 0; str[i] != '\0'; i++) {
        push_array(stack, str[i]);
    }
    
    // Pop and overwrite
    int i = 0;
    while (!is_empty_array(stack)) {
        str[i++] = pop_array(stack);
    }
    
    free_array_stack(stack);
}

int main(void) {
    printf("=== Stack (LIFO) ===\n\n");
    
    // Example 1: Array-based stack
    printf("Example 1: Array-based stack\n");
    printf("----------------------------\n");
    ArrayStack* array_stack = create_array_stack(5);
    
    printf("Pushing 10, 20, 30...\n");
    push_array(array_stack, 10);
    push_array(array_stack, 20);
    push_array(array_stack, 30);
    
    printf("Top element: %d\n", peek_array(array_stack));
    
    printf("Popping: %d\n", pop_array(array_stack));
    printf("Popping: %d\n", pop_array(array_stack));
    printf("Top now: %d\n", peek_array(array_stack));
    
    // Example 2: Stack overflow/underflow
    printf("\n\nExample 2: Testing limits\n");
    printf("-------------------------\n");
    ArrayStack* small_stack = create_array_stack(3);
    
    printf("Pushing 4 items into stack with capacity 3:\n");
    push_array(small_stack, 1);
    push_array(small_stack, 2);
    push_array(small_stack, 3);
    push_array(small_stack, 4);  // Overflow!
    
    printf("Popping all items:\n");
    while (!is_empty_array(small_stack)) {
        printf("  Popped: %d\n", pop_array(small_stack));
    }
    printf("Popping from empty stack:\n");
    pop_array(small_stack);  // Underflow!
    
    // Example 3: Linked list-based stack
    printf("\n\nExample 3: Linked list-based stack\n");
    printf("-----------------------------------\n");
    LinkedStack* linked_stack = create_linked_stack();
    
    printf("Pushing 100, 200, 300...\n");
    push_linked(linked_stack, 100);
    push_linked(linked_stack, 200);
    push_linked(linked_stack, 300);
    printf("Size: %d\n", linked_stack->size);
    
    printf("Popping: %d\n", pop_linked(linked_stack));
    printf("Top now: %d\n", peek_linked(linked_stack));
    printf("Size: %d\n", linked_stack->size);
    
    // Example 4: Balanced parentheses
    printf("\n\nExample 4: Balanced parentheses checker\n");
    printf("---------------------------------------\n");
    const char* expressions[] = {
        "(a + b)",
        "(a + b))",
        "((a + b) * c)",
        "{[()]}",
        "{[(])}",
        "((()))"
    };
    
    for (int i = 0; i < 6; i++) {
        int balanced = is_balanced(expressions[i]);
        printf("  '%s' is %s\n", expressions[i], 
               balanced ? "balanced" : "NOT balanced");
    }
    
    // Example 5: String reversal
    printf("\n\nExample 5: String reversal using stack\n");
    printf("--------------------------------------\n");
    char strings[][50] = {
        "hello",
        "stack",
        "racecar"
    };
    
    for (int i = 0; i < 3; i++) {
        char original[50];
        strcpy(original, strings[i]);
        reverse_string(strings[i]);
        printf("  '%s' -> '%s'\n", original, strings[i]);
    }
    
    // Cleanup
    free_array_stack(array_stack);
    free_array_stack(small_stack);
    free_linked_stack(linked_stack);
    
    printf("\n\nPress Enter to exit...");
    getchar();
    return 0;
}

/*
 * Stack Concepts:
 * 
 * LIFO (Last In, First Out):
 * - Like a stack of plates
 * - Add (push) on top
 * - Remove (pop) from top
 * - Can only access top element
 * 
 * Operations:
 * - push(item): Add to top - O(1)
 * - pop(): Remove and return top - O(1)
 * - peek(): Look at top without removing - O(1)
 * - isEmpty(): Check if empty - O(1)
 * 
 * Array vs Linked List Implementation:
 * 
 * Array-based:
 * + Fast (no allocation per operation)
 * + Cache-friendly
 * + Simple implementation
 * - Fixed size
 * - Wastes memory if too large
 * 
 * Linked list-based:
 * + Dynamic size (never full)
 * + Only uses needed memory
 * - Slower (malloc/free overhead)
 * - Less cache-friendly
 * 
 * Common Uses:
 * - Function call stack
 * - Undo/redo in editors
 * - Expression evaluation
 * - Backtracking algorithms
 * - DFS (depth-first search)
 * - Browser back button
 * - Syntax checking
 * 
 * Real-World Example:
 * Every function call uses the call stack:
 * 
 * main() calls foo()
 * foo() calls bar()
 * bar() returns to foo()
 * foo() returns to main()
 * 
 * Stack frames:
 * [main]
 * [main][foo]
 * [main][foo][bar]
 * [main][foo]
 * [main]
 * 
 * This is why infinite recursion causes "stack overflow" - the call stack runs out of space!
 */
