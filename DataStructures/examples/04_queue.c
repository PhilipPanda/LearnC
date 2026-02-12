/*
 * 04_queue.c
 * 
 * Queue implementation (FIFO - First In, First Out).
 * Shows circular array and linked list implementations.
 */

#include <stdio.h>
#include <stdlib.h>

// ===== Circular Array Queue =====

typedef struct ArrayQueue {
    int* items;
    int front;
    int rear;
    int size;
    int capacity;
} ArrayQueue;

ArrayQueue* create_array_queue(int capacity) {
    ArrayQueue* queue = (ArrayQueue*)malloc(sizeof(ArrayQueue));
    queue->items = (int*)malloc(capacity * sizeof(int));
    queue->front = 0;
    queue->rear = -1;
    queue->size = 0;
    queue->capacity = capacity;
    return queue;
}

int is_full_array(ArrayQueue* queue) {
    return queue->size == queue->capacity;
}

int is_empty_array(ArrayQueue* queue) {
    return queue->size == 0;
}

void enqueue_array(ArrayQueue* queue, int item) {
    if (is_full_array(queue)) {
        printf("Queue overflow!\n");
        return;
    }
    
    queue->rear = (queue->rear + 1) % queue->capacity;  // Wrap around
    queue->items[queue->rear] = item;
    queue->size++;
}

int dequeue_array(ArrayQueue* queue) {
    if (is_empty_array(queue)) {
        printf("Queue underflow!\n");
        return -1;
    }
    
    int item = queue->items[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;  // Wrap around
    queue->size--;
    return item;
}

int peek_array(ArrayQueue* queue) {
    if (is_empty_array(queue)) {
        return -1;
    }
    return queue->items[queue->front];
}

void print_queue_array(ArrayQueue* queue) {
    if (is_empty_array(queue)) {
        printf("(empty)\n");
        return;
    }
    
    printf("Front -> ");
    int count = queue->size;
    int index = queue->front;
    for (int i = 0; i < count; i++) {
        printf("%d ", queue->items[index]);
        index = (index + 1) % queue->capacity;
    }
    printf("-> Rear\n");
}

void free_array_queue(ArrayQueue* queue) {
    free(queue->items);
    free(queue);
}

// ===== Linked List Queue =====

typedef struct QueueNode {
    int data;
    struct QueueNode* next;
} QueueNode;

typedef struct LinkedQueue {
    QueueNode* front;
    QueueNode* rear;
    int size;
} LinkedQueue;

LinkedQueue* create_linked_queue() {
    LinkedQueue* queue = (LinkedQueue*)malloc(sizeof(LinkedQueue));
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
    return queue;
}

int is_empty_linked(LinkedQueue* queue) {
    return queue->front == NULL;
}

void enqueue_linked(LinkedQueue* queue, int data) {
    QueueNode* new_node = (QueueNode*)malloc(sizeof(QueueNode));
    new_node->data = data;
    new_node->next = NULL;
    
    if (queue->rear == NULL) {
        // Empty queue
        queue->front = queue->rear = new_node;
    } else {
        queue->rear->next = new_node;
        queue->rear = new_node;
    }
    queue->size++;
}

int dequeue_linked(LinkedQueue* queue) {
    if (is_empty_linked(queue)) {
        printf("Queue underflow!\n");
        return -1;
    }
    
    QueueNode* temp = queue->front;
    int data = temp->data;
    queue->front = queue->front->next;
    
    if (queue->front == NULL) {
        // Queue is now empty
        queue->rear = NULL;
    }
    
    free(temp);
    queue->size--;
    return data;
}

int peek_linked(LinkedQueue* queue) {
    if (is_empty_linked(queue)) {
        return -1;
    }
    return queue->front->data;
}

void print_queue_linked(LinkedQueue* queue) {
    if (is_empty_linked(queue)) {
        printf("(empty)\n");
        return;
    }
    
    printf("Front -> ");
    QueueNode* current = queue->front;
    while (current != NULL) {
        printf("%d ", current->data);
        current = current->next;
    }
    printf("-> Rear\n");
}

void free_linked_queue(LinkedQueue* queue) {
    while (!is_empty_linked(queue)) {
        dequeue_linked(queue);
    }
    free(queue);
}

// ===== Application: Task Scheduler Simulation =====

void simulate_task_queue() {
    LinkedQueue* tasks = create_linked_queue();
    
    printf("Task Scheduler Simulation:\n");
    printf("-------------------------\n");
    
    printf("Adding tasks 101, 102, 103...\n");
    enqueue_linked(tasks, 101);
    enqueue_linked(tasks, 102);
    enqueue_linked(tasks, 103);
    print_queue_linked(tasks);
    
    printf("\nProcessing tasks in FIFO order:\n");
    while (!is_empty_linked(tasks)) {
        int task = dequeue_linked(tasks);
        printf("  Processing task %d...\n", task);
    }
    
    free_linked_queue(tasks);
}

int main(void) {
    printf("=== Queue (FIFO) ===\n\n");
    
    // Example 1: Circular array queue
    printf("Example 1: Circular array queue\n");
    printf("-------------------------------\n");
    ArrayQueue* array_queue = create_array_queue(5);
    
    printf("Enqueuing 10, 20, 30...\n");
    enqueue_array(array_queue, 10);
    enqueue_array(array_queue, 20);
    enqueue_array(array_queue, 30);
    print_queue_array(array_queue);
    
    printf("\nDequeuing: %d\n", dequeue_array(array_queue));
    printf("Queue now: ");
    print_queue_array(array_queue);
    
    printf("\nFront element: %d\n", peek_array(array_queue));
    
    // Example 2: Circular wrapping
    printf("\n\nExample 2: Demonstrating circular buffer\n");
    printf("----------------------------------------\n");
    ArrayQueue* circular = create_array_queue(4);
    
    printf("Capacity: 4\n\n");
    printf("Enqueue 1, 2, 3:\n");
    enqueue_array(circular, 1);
    enqueue_array(circular, 2);
    enqueue_array(circular, 3);
    printf("  Front index: %d, Rear index: %d\n", circular->front, circular->rear);
    
    printf("\nDequeue twice:\n");
    dequeue_array(circular);
    dequeue_array(circular);
    printf("  Front index: %d, Rear index: %d\n", circular->front, circular->rear);
    
    printf("\nEnqueue 4, 5 (wraps around!):\n");
    enqueue_array(circular, 4);
    enqueue_array(circular, 5);
    printf("  Front index: %d, Rear index: %d\n", circular->front, circular->rear);
    printf("  ");
    print_queue_array(circular);
    
    // Example 3: Queue overflow
    printf("\n\nExample 3: Testing capacity limits\n");
    printf("-----------------------------------\n");
    ArrayQueue* small = create_array_queue(3);
    
    printf("Enqueuing 4 items into queue with capacity 3:\n");
    enqueue_array(small, 1);
    enqueue_array(small, 2);
    enqueue_array(small, 3);
    enqueue_array(small, 4);  // Overflow!
    
    // Example 4: Linked list queue
    printf("\n\nExample 4: Linked list queue\n");
    printf("----------------------------\n");
    LinkedQueue* linked_queue = create_linked_queue();
    
    printf("Enqueuing 100, 200, 300...\n");
    enqueue_linked(linked_queue, 100);
    enqueue_linked(linked_queue, 200);
    enqueue_linked(linked_queue, 300);
    print_queue_linked(linked_queue);
    printf("Size: %d\n", linked_queue->size);
    
    printf("\nDequeuing: %d\n", dequeue_linked(linked_queue));
    printf("Queue now: ");
    print_queue_linked(linked_queue);
    printf("Size: %d\n", linked_queue->size);
    
    // Example 5: Application
    printf("\n\nExample 5: Real-world application\n");
    printf("---------------------------------\n");
    simulate_task_queue();
    
    // Cleanup
    free_array_queue(array_queue);
    free_array_queue(circular);
    free_array_queue(small);
    free_linked_queue(linked_queue);
    
    printf("\n\nPress Enter to exit...");
    getchar();
    return 0;
}

/*
 * Queue Concepts:
 * 
 * FIFO (First In, First Out):
 * - Like a line at the store
 * - Add (enqueue) at rear
 * - Remove (dequeue) from front
 * - Fair ordering (first come, first served)
 * 
 * Operations:
 * - enqueue(item): Add to rear - O(1)
 * - dequeue(): Remove and return front - O(1)
 * - peek(): Look at front without removing - O(1)
 * - isEmpty(): Check if empty - O(1)
 * 
 * Circular Buffer (Array Implementation):
 * 
 * Without wrapping:
 * [1][2][3][ ][ ]
 *  ^        ^
 *  F        R
 * 
 * After dequeue twice and enqueue 4, 5:
 * [ ][ ][3][4][5]
 *        ^      ^
 *        F      R
 * 
 * Front moves right, rear keeps moving.
 * Eventually rear reaches end - can't add even if space at start!
 * 
 * With circular wrapping:
 * [5][ ][3][4][ ]
 *  ^     ^
 *  R     F
 * 
 * Rear wraps to start! Efficient use of space.
 * 
 * Formula: (index + 1) % capacity
 * 
 * Array vs Linked List:
 * 
 * Circular Array:
 * + Fast (no allocation)
 * + Cache-friendly
 * + Predictable memory
 * - Fixed size
 * - Wastes slots when not full
 * 
 * Linked List:
 * + Dynamic size
 * + Never wastes space
 * + Never "full"
 * - malloc/free overhead
 * - Less cache-friendly
 * - More memory per element (pointers)
 * 
 * Common Uses:
 * - Task scheduling
 * - Print spooler
 * - BFS (breadth-first search)
 * - Buffering (keyboard, network)
 * - Message queues
 * - Request handling
 * - Level-order tree traversal
 * 
 * Real-World Examples:
 * 
 * 1. Operating System:
 *    - Process scheduling
 *    - I/O request handling
 * 
 * 2. Networking:
 *    - Packet buffering
 *    - Request queues
 * 
 * 3. Gaming:
 *    - Input event queue
 *    - Animation frame queue
 * 
 * Queue vs Stack:
 * - Stack: Last in, first out (recursive, undo)
 * - Queue: First in, first out (fair, ordered)
 */
