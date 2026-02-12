/*
 * 08_heap.c
 * 
 * Min-heap implementation (priority queue).
 * Demonstrates heap operations and heap sort.
 */

#include <stdio.h>
#include <stdlib.h>

typedef struct MinHeap {
    int* array;
    int capacity;
    int size;
} MinHeap;

// Helper functions
int parent(int i) { return (i - 1) / 2; }
int left_child(int i) { return 2 * i + 1; }
int right_child(int i) { return 2 * i + 2; }

void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Create min heap
MinHeap* create_heap(int capacity) {
    MinHeap* heap = (MinHeap*)malloc(sizeof(MinHeap));
    heap->capacity = capacity;
    heap->size = 0;
    heap->array = (int*)malloc(capacity * sizeof(int));
    return heap;
}

// Print heap as array
void print_heap_array(MinHeap* heap) {
    printf("[");
    for (int i = 0; i < heap->size; i++) {
        printf("%d", heap->array[i]);
        if (i < heap->size - 1) printf(", ");
    }
    printf("]\n");
}

// Print heap as tree
void print_heap_tree(MinHeap* heap, int index, int level) {
    if (index >= heap->size) return;
    
    print_heap_tree(heap, right_child(index), level + 1);
    
    for (int i = 0; i < level; i++) {
        printf("    ");
    }
    printf("%d\n", heap->array[index]);
    
    print_heap_tree(heap, left_child(index), level + 1);
}

// Heapify down (restore heap property downward)
void heapify_down(MinHeap* heap, int index) {
    int smallest = index;
    int left = left_child(index);
    int right = right_child(index);
    
    if (left < heap->size && heap->array[left] < heap->array[smallest]) {
        smallest = left;
    }
    
    if (right < heap->size && heap->array[right] < heap->array[smallest]) {
        smallest = right;
    }
    
    if (smallest != index) {
        swap(&heap->array[index], &heap->array[smallest]);
        heapify_down(heap, smallest);
    }
}

// Heapify up (restore heap property upward)
void heapify_up(MinHeap* heap, int index) {
    while (index > 0 && heap->array[parent(index)] > heap->array[index]) {
        swap(&heap->array[parent(index)], &heap->array[index]);
        index = parent(index);
    }
}

// Insert element
void insert(MinHeap* heap, int value) {
    if (heap->size == heap->capacity) {
        printf("Heap is full!\n");
        return;
    }
    
    // Insert at end
    heap->array[heap->size] = value;
    heap->size++;
    
    // Heapify up to maintain heap property
    heapify_up(heap, heap->size - 1);
}

// Extract minimum (root)
int extract_min(MinHeap* heap) {
    if (heap->size == 0) {
        printf("Heap is empty!\n");
        return -1;
    }
    
    if (heap->size == 1) {
        heap->size--;
        return heap->array[0];
    }
    
    // Save root
    int root = heap->array[0];
    
    // Move last element to root
    heap->array[0] = heap->array[heap->size - 1];
    heap->size--;
    
    // Heapify down to maintain heap property
    heapify_down(heap, 0);
    
    return root;
}

// Peek minimum
int peek_min(MinHeap* heap) {
    if (heap->size == 0) {
        return -1;
    }
    return heap->array[0];
}

// Build heap from array
void build_heap(MinHeap* heap, int arr[], int n) {
    heap->size = n;
    for (int i = 0; i < n; i++) {
        heap->array[i] = arr[i];
    }
    
    // Heapify from last non-leaf node to root
    for (int i = (n / 2) - 1; i >= 0; i--) {
        heapify_down(heap, i);
    }
}

// Heap sort (in-place)
void heap_sort(int arr[], int n) {
    MinHeap* heap = create_heap(n);
    build_heap(heap, arr, n);
    
    // Extract min repeatedly
    for (int i = 0; i < n; i++) {
        arr[i] = extract_min(heap);
    }
    
    free(heap->array);
    free(heap);
}

// Free heap
void free_heap(MinHeap* heap) {
    free(heap->array);
    free(heap);
}

int main(void) {
    printf("=== Min-Heap (Priority Queue) ===\n\n");
    
    // Example 1: Building a heap
    printf("Example 1: Building a heap\n");
    printf("--------------------------\n");
    MinHeap* heap = create_heap(20);
    
    printf("Inserting: 10, 5, 20, 1, 15, 30, 25\n\n");
    int values[] = {10, 5, 20, 1, 15, 30, 25};
    for (int i = 0; i < 7; i++) {
        insert(heap, values[i]);
        printf("After inserting %d:\n", values[i]);
        printf("  Array: ");
        print_heap_array(heap);
        printf("  Min: %d\n\n", peek_min(heap));
    }
    
    printf("Heap tree structure:\n");
    print_heap_tree(heap, 0, 0);
    printf("\n(Smallest element is always at root!)\n");
    
    // Example 2: Extract minimum
    printf("\n\nExample 2: Extracting minimum\n");
    printf("-----------------------------\n");
    printf("Initial heap: ");
    print_heap_array(heap);
    printf("\n");
    
    for (int i = 0; i < 3; i++) {
        int min = extract_min(heap);
        printf("Extracted min: %d\n", min);
        printf("  Heap now: ");
        print_heap_array(heap);
        printf("\n");
    }
    
    // Example 3: Building heap from array
    printf("\n\nExample 3: Building heap from array\n");
    printf("------------------------------------\n");
    int arr[] = {9, 5, 6, 2, 3, 7, 1, 4, 8};
    int n = sizeof(arr) / sizeof(arr[0]);
    
    printf("Array: ");
    for (int i = 0; i < n; i++) printf("%d ", arr[i]);
    printf("\n");
    
    MinHeap* heap2 = create_heap(20);
    build_heap(heap2, arr, n);
    
    printf("After heapify:\n");
    printf("  Array: ");
    print_heap_array(heap2);
    printf("\n  Tree:\n");
    print_heap_tree(heap2, 0, 0);
    
    // Example 4: Priority queue simulation
    printf("\n\nExample 4: Priority queue simulation\n");
    printf("-------------------------------------\n");
    printf("Task priority queue (lower number = higher priority):\n\n");
    
    MinHeap* tasks = create_heap(10);
    
    printf("Adding tasks:\n");
    printf("  Task with priority 5\n");
    insert(tasks, 5);
    printf("  Task with priority 2\n");
    insert(tasks, 2);
    printf("  Task with priority 8\n");
    insert(tasks, 8);
    printf("  Task with priority 1\n");
    insert(tasks, 1);
    printf("  Task with priority 3\n");
    insert(tasks, 3);
    
    printf("\nProcessing tasks by priority:\n");
    while (tasks->size > 0) {
        int priority = extract_min(tasks);
        printf("  Processing task with priority %d\n", priority);
    }
    
    // Example 5: Heap sort
    printf("\n\nExample 5: Heap sort\n");
    printf("--------------------\n");
    int unsorted[] = {64, 34, 25, 12, 22, 11, 90};
    int len = sizeof(unsorted) / sizeof(unsorted[0]);
    
    printf("Unsorted: ");
    for (int i = 0; i < len; i++) printf("%d ", unsorted[i]);
    printf("\n");
    
    heap_sort(unsorted, len);
    
    printf("Sorted:   ");
    for (int i = 0; i < len; i++) printf("%d ", unsorted[i]);
    printf("\n(Heap sort is O(n log n) and in-place!)\n");
    
    // Example 6: Finding k smallest elements
    printf("\n\nExample 6: Finding 3 smallest elements\n");
    printf("---------------------------------------\n");
    int numbers[] = {7, 10, 4, 3, 20, 15, 8, 2};
    int num_count = sizeof(numbers) / sizeof(numbers[0]);
    int k = 3;
    
    printf("Numbers: ");
    for (int i = 0; i < num_count; i++) printf("%d ", numbers[i]);
    printf("\n");
    
    MinHeap* find_k = create_heap(num_count);
    build_heap(find_k, numbers, num_count);
    
    printf("\n%d smallest elements: ", k);
    for (int i = 0; i < k; i++) {
        printf("%d ", extract_min(find_k));
    }
    printf("\n");
    
    // Cleanup
    free_heap(heap);
    free_heap(heap2);
    free_heap(tasks);
    free_heap(find_k);
    
    printf("\n\nPress Enter to exit...");
    getchar();
    return 0;
}

/*
 * Heap Concepts:
 * 
 * What is a Heap?
 * - Complete binary tree with heap property
 * - Min-heap: Parent ≤ children
 * - Max-heap: Parent ≥ children
 * - Efficient for priority queues
 * 
 * Heap Property (Min-Heap):
 *        1
 *       / \
 *      2   3
 *     / \
 *    4   5
 * 
 * - Root is minimum element
 * - Every parent ≤ its children
 * - No ordering between siblings
 * 
 * Array Representation:
 * [1, 2, 3, 4, 5]
 * 
 * Index relationships:
 * - Parent of i: (i-1)/2
 * - Left child of i: 2*i + 1
 * - Right child of i: 2*i + 2
 * 
 * Why array? Complete tree fills level by level, no wasted space!
 * 
 * Operations:
 * 
 * 1. Insert (O(log n)):
 *    - Add at end (maintain complete tree)
 *    - Heapify up (bubble up)
 * 
 * 2. Extract Min (O(log n)):
 *    - Remove root (min element)
 *    - Move last element to root
 *    - Heapify down (bubble down)
 * 
 * 3. Peek Min (O(1)):
 *    - Just return root
 * 
 * 4. Build Heap (O(n)):
 *    - Start from last non-leaf
 *    - Heapify down each node
 * 
 * Heapify Up:
 * - Compare with parent
 * - Swap if violates heap property
 * - Repeat until root or property satisfied
 * 
 * Heapify Down:
 * - Compare with children
 * - Swap with smaller child (min-heap)
 * - Repeat until leaf or property satisfied
 * 
 * Complexity:
 * - Insert: O(log n)
 * - Extract min/max: O(log n)
 * - Peek min/max: O(1)
 * - Build heap: O(n)
 * - Space: O(n)
 * 
 * Why O(log n)?
 * - Height of complete binary tree is log n
 * - Heapify up/down traverses height
 * 
 * Priority Queue:
 * - Heap is perfect for priority queues
 * - Insert with priority
 * - Extract highest priority (min or max)
 * - Fast operations
 * 
 * Applications:
 * 
 * 1. Priority Queue:
 *    - Task scheduling
 *    - Event simulation
 *    - Dijkstra's algorithm
 * 
 * 2. Heap Sort:
 *    - Build heap: O(n)
 *    - Extract n times: O(n log n)
 *    - Total: O(n log n)
 *    - In-place sorting!
 * 
 * 3. K Smallest/Largest:
 *    - Build heap: O(n)
 *    - Extract k times: O(k log n)
 *    - Total: O(n + k log n)
 * 
 * 4. Median Maintenance:
 *    - Two heaps (max-heap and min-heap)
 *    - Running median in O(log n)
 * 
 * Heap vs BST:
 * 
 * Heap:
 * + Faster min/max (O(1) vs O(log n))
 * + Better for priority queue
 * + Simple array implementation
 * - Can't search for arbitrary element
 * - No ordering except min/max
 * 
 * BST:
 * + Can search any element O(log n)
 * + Ordered traversal
 * + Range queries
 * - Slower min/max (must traverse)
 * - More complex (pointers)
 * 
 * Max-Heap vs Min-Heap:
 * - Max-heap: Largest at root
 * - Min-heap: Smallest at root
 * - Same operations, opposite comparisons
 * - Use max-heap for "top k largest"
 * - Use min-heap for "top k smallest"
 * 
 * Real-World Uses:
 * - Operating system task scheduling
 * - Dijkstra's shortest path
 * - Huffman coding
 * - Kth largest element problems
 * - Merge k sorted lists
 * - Event-driven simulation
 * 
 * Try:
 * - Implement max-heap variant
 * - Heap sort on large dataset
 * - K largest elements problem
 * - Merge k sorted arrays using heap
 * - Running median problem (two heaps)
 */
