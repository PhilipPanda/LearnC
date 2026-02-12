# Stacks and Queues

## The Big Picture

Some problems have natural order constraints:

**Last In, First Out (LIFO):** Function call stack, undo/redo, backtracking. The most recent thing is handled first.

**First In, First Out (FIFO):** Print queue, task scheduling, breadth-first search. Things are handled in order of arrival.

These patterns are so common they have names: **Stack** and **Queue**.

Both can be implemented with arrays or linked lists. Each has tradeoffs.

## Stack

Think of a stack of plates. You add (push) plates on top. You remove (pop) from the top. You can't reach the bottom without removing everything above it.

### Operations

- **Push:** Add element to top
- **Pop:** Remove and return top element
- **Peek/Top:** Look at top without removing
- **IsEmpty:** Check if stack is empty

All operations are O(1).

### Array-Based Stack

Fixed size, simple implementation:

```c
typedef struct Stack {
    int* items;
    int top;
    int capacity;
} Stack;

Stack* create_stack(int capacity) {
    Stack* stack = (Stack*)malloc(sizeof(Stack));
    stack->items = (int*)malloc(capacity * sizeof(int));
    stack->top = -1;  // Empty stack
    stack->capacity = capacity;
    return stack;
}

int is_full(Stack* stack) {
    return stack->top == stack->capacity - 1;
}

int is_empty(Stack* stack) {
    return stack->top == -1;
}

void push(Stack* stack, int item) {
    if (is_full(stack)) {
        printf("Stack overflow!\n");
        return;
    }
    stack->items[++stack->top] = item;
}

int pop(Stack* stack) {
    if (is_empty(stack)) {
        printf("Stack underflow!\n");
        return -1;
    }
    return stack->items[stack->top--];
}

int peek(Stack* stack) {
    if (is_empty(stack)) {
        return -1;
    }
    return stack->items[stack->top];
}
```

**Pros:** Fast, cache-friendly, simple  
**Cons:** Fixed size, waste memory if too large

### Linked List-Based Stack

Dynamic size:

```c
typedef struct StackNode {
    int data;
    struct StackNode* next;
} StackNode;

typedef struct Stack {
    StackNode* top;
    int size;
} Stack;

void push(Stack* stack, int data) {
    StackNode* new_node = (StackNode*)malloc(sizeof(StackNode));
    new_node->data = data;
    new_node->next = stack->top;
    stack->top = new_node;
    stack->size++;
}

int pop(Stack* stack) {
    if (stack->top == NULL) {
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
```

**Pros:** No size limit, only uses needed memory  
**Cons:** Pointer overhead, slower allocation, less cache-friendly

### Stack Applications

**Function call stack:**
```c
void c() { printf("C\n"); }
void b() { c(); }
void a() { b(); }

a();  // Stack: a -> b -> c, then unwinds c -> b -> a
```

**Expression evaluation:**

Infix: `3 + 4 * 2`  
Postfix: `3 4 2 * +`

Evaluate postfix with stack:
1. Push 3
2. Push 4
3. Push 2
4. Pop 2 and 4, multiply, push 8
5. Pop 8 and 3, add, result: 11

**Balanced parentheses:**
```c
int is_balanced(char* expr) {
    Stack* stack = create_stack(100);
    
    for (int i = 0; expr[i]; i++) {
        if (expr[i] == '(') {
            push(stack, '(');
        } else if (expr[i] == ')') {
            if (is_empty(stack)) return 0;
            pop(stack);
        }
    }
    
    return is_empty(stack);
}
```

## Queue

Think of a line at the store. First person in line is served first. New arrivals join the back.

### Operations

- **Enqueue:** Add element to back
- **Dequeue:** Remove and return front element
- **Front/Peek:** Look at front without removing
- **IsEmpty:** Check if queue is empty

All operations are O(1).

### Array-Based Queue (Circular)

Use circular buffer to avoid shifting:

```c
typedef struct Queue {
    int* items;
    int front;
    int rear;
    int size;
    int capacity;
} Queue;

Queue* create_queue(int capacity) {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->items = (int*)malloc(capacity * sizeof(int));
    queue->front = 0;
    queue->rear = -1;
    queue->size = 0;
    queue->capacity = capacity;
    return queue;
}

int is_full(Queue* queue) {
    return queue->size == queue->capacity;
}

int is_empty(Queue* queue) {
    return queue->size == 0;
}

void enqueue(Queue* queue, int item) {
    if (is_full(queue)) {
        printf("Queue overflow!\n");
        return;
    }
    queue->rear = (queue->rear + 1) % queue->capacity;  // Wrap around
    queue->items[queue->rear] = item;
    queue->size++;
}

int dequeue(Queue* queue) {
    if (is_empty(queue)) {
        printf("Queue underflow!\n");
        return -1;
    }
    int item = queue->items[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;  // Wrap around
    queue->size--;
    return item;
}
```

**Circular buffer visualization:**
```
Capacity: 5
[10][20][30][ ][ ]
 ^          ^
front      rear

After dequeue:
[ ][20][30][ ][ ]
    ^      ^
  front   rear

After enqueue 40, 50:
[50][20][30][40][ ]
    ^          ^
  front       rear

After enqueue 60 (wraps around):
[50][20][30][40][60]
    ^               ^
  front            rear
```

### Linked List-Based Queue

Track front and rear pointers:

```c
typedef struct QueueNode {
    int data;
    struct QueueNode* next;
} QueueNode;

typedef struct Queue {
    QueueNode* front;
    QueueNode* rear;
    int size;
} Queue;

void enqueue(Queue* queue, int data) {
    QueueNode* new_node = (QueueNode*)malloc(sizeof(QueueNode));
    new_node->data = data;
    new_node->next = NULL;
    
    if (queue->rear == NULL) {
        queue->front = queue->rear = new_node;
    } else {
        queue->rear->next = new_node;
        queue->rear = new_node;
    }
    queue->size++;
}

int dequeue(Queue* queue) {
    if (queue->front == NULL) {
        printf("Queue underflow!\n");
        return -1;
    }
    
    QueueNode* temp = queue->front;
    int data = temp->data;
    queue->front = queue->front->next;
    
    if (queue->front == NULL) {
        queue->rear = NULL;  // Queue is now empty
    }
    
    free(temp);
    queue->size--;
    return data;
}
```

## Priority Queue

Not strictly FIFO - elements have priority:

```c
// Simple implementation: keep sorted
void enqueue_priority(Queue* queue, int data, int priority) {
    // Insert in sorted order by priority
}
```

Better implementation: use a heap (see heap example).

## Deque (Double-Ended Queue)

Insert and remove from both ends:

```c
void push_front(Deque* deque, int data);
void push_back(Deque* deque, int data);
int pop_front(Deque* deque);
int pop_back(Deque* deque);
```

Useful for sliding window problems, palindrome checking.

## When to Use What

**Stack:**
- Function calls
- Undo/redo
- Backtracking (DFS, maze solving)
- Expression parsing
- Browser history

**Queue:**
- Task scheduling
- Breadth-first search
- Print spooling
- Buffering (keyboard, I/O)
- Handling requests

**Priority Queue:**
- Dijkstra's algorithm
- Huffman coding
- Task scheduling with priorities
- Event simulation

## Common Patterns

**Stack for recursion elimination:**

Recursive DFS:
```c
void dfs(Node* node) {
    if (!node) return;
    process(node);
    dfs(node->left);
    dfs(node->right);
}
```

Iterative with stack:
```c
void dfs_iterative(Node* root) {
    Stack* stack = create_stack(100);
    push(stack, root);
    
    while (!is_empty(stack)) {
        Node* node = pop(stack);
        process(node);
        if (node->right) push(stack, node->right);
        if (node->left) push(stack, node->left);
    }
}
```

**Queue for level-order traversal:**
```c
void level_order(Node* root) {
    Queue* queue = create_queue(100);
    enqueue(queue, root);
    
    while (!is_empty(queue)) {
        Node* node = dequeue(queue);
        process(node);
        if (node->left) enqueue(queue, node->left);
        if (node->right) enqueue(queue, node->right);
    }
}
```

## Summary

Stacks and queues constrain access patterns to match problem requirements. Stack for last-in-first-out. Queue for first-in-first-out. Simple concept, powerful applications.

Choose array implementation for speed and fixed size. Choose linked list for dynamic size and flexibility.
