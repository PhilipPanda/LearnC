/*
 * 04_stack_allocator.c
 * 
 * Stack allocator - LIFO allocator with scoped lifetimes.
 * Must free in reverse order of allocation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ===== Stack Allocator =====

typedef struct {
    size_t offset;  // Offset to start of allocation
    size_t size;    // Size of allocation
} StackHeader;

typedef struct {
    char* buffer;
    size_t size;
    size_t used;
} StackAllocator;

// Create stack allocator
StackAllocator* stack_create(size_t size) {
    StackAllocator* stack = (StackAllocator*)malloc(sizeof(StackAllocator));
    if (stack == NULL) return NULL;
    
    stack->buffer = (char*)malloc(size);
    if (stack->buffer == NULL) {
        free(stack);
        return NULL;
    }
    
    stack->size = size;
    stack->used = 0;
    
    return stack;
}

// Allocate from stack (O(1))
void* stack_alloc(StackAllocator* stack, size_t size) {
    // Add header to track allocation
    size_t total_size = sizeof(StackHeader) + size;
    
    // Align to 8 bytes
    total_size = (total_size + 7) & ~7;
    
    if (stack->used + total_size > stack->size) {
        return NULL;  // Out of space
    }
    
    // Store header
    StackHeader* header = (StackHeader*)(stack->buffer + stack->used);
    header->offset = stack->used;
    header->size = total_size;
    
    stack->used += total_size;
    
    // Return memory after header
    return (char*)header + sizeof(StackHeader);
}

// Free from stack (O(1)) - MUST be in reverse order!
void stack_free(StackAllocator* stack, void* ptr) {
    if (ptr == NULL) return;
    
    // Get header
    StackHeader* header = (StackHeader*)((char*)ptr - sizeof(StackHeader));
    
    // Verify this is top of stack
    if (header->offset + header->size != stack->used) {
        printf("ERROR: Must free in reverse order!\n");
        printf("  Expected offset: %zu, got: %zu\n", 
               stack->used - header->size, header->offset);
        return;
    }
    
    // Pop from stack
    stack->used = header->offset;
}

// Get current stack position (for checkpoints)
size_t stack_get_marker(StackAllocator* stack) {
    return stack->used;
}

// Free everything after marker
void stack_free_to_marker(StackAllocator* stack, size_t marker) {
    if (marker <= stack->used) {
        stack->used = marker;
    }
}

// Print stats
void stack_print_stats(StackAllocator* stack) {
    printf("Stack Stats:\n");
    printf("  Total size: %zu bytes\n", stack->size);
    printf("  Used: %zu bytes (%.1f%%)\n", 
           stack->used, (stack->used * 100.0) / stack->size);
    printf("  Free: %zu bytes\n", stack->size - stack->used);
}

// Destroy stack
void stack_destroy(StackAllocator* stack) {
    if (stack) {
        free(stack->buffer);
        free(stack);
    }
}

// ===== Example: Recursive Function =====

int factorial_with_stack(StackAllocator* stack, int n, int depth) {
    // Allocate temporary buffer for this recursion level
    char* temp = (char*)stack_alloc(stack, 100);
    if (temp == NULL) {
        printf("Stack full!\n");
        return -1;
    }
    
    snprintf(temp, 100, "Recursion depth %d: factorial(%d)", depth, n);
    printf("  %s\n", temp);
    
    int result;
    if (n <= 1) {
        result = 1;
    } else {
        result = n * factorial_with_stack(stack, n - 1, depth + 1);
    }
    
    // Free in reverse order (LIFO)
    stack_free(stack, temp);
    
    return result;
}

// ===== Example: Nested Scopes =====

void nested_scope_example() {
    printf("\n=== Nested Scope Example ===\n");
    
    StackAllocator* stack = stack_create(4096);
    
    printf("Outer scope:\n");
    int* outer = (int*)stack_alloc(stack, 10 * sizeof(int));
    for (int i = 0; i < 10; i++) {
        outer[i] = i;
    }
    printf("  Allocated outer (%zu bytes)\n", 10 * sizeof(int));
    stack_print_stats(stack);
    
    printf("\nMiddle scope:\n");
    char* middle = (char*)stack_alloc(stack, 50);
    strcpy(middle, "Middle allocation");
    printf("  Allocated middle: '%s'\n", middle);
    stack_print_stats(stack);
    
    printf("\nInner scope:\n");
    float* inner = (float*)stack_alloc(stack, 20 * sizeof(float));
    for (int i = 0; i < 20; i++) {
        inner[i] = i * 1.5f;
    }
    printf("  Allocated inner (%zu bytes)\n", 20 * sizeof(float));
    stack_print_stats(stack);
    
    // Free in reverse order (LIFO)
    printf("\nFreeing inner:\n");
    stack_free(stack, inner);
    stack_print_stats(stack);
    
    printf("\nFreeing middle:\n");
    stack_free(stack, middle);
    stack_print_stats(stack);
    
    // outer still valid!
    printf("  outer[5] = %d (still valid!)\n", outer[5]);
    
    printf("\nFreeing outer:\n");
    stack_free(stack, outer);
    stack_print_stats(stack);
    
    stack_destroy(stack);
}

// ===== Example: Checkpoint/Restore =====

void checkpoint_example() {
    printf("\n=== Checkpoint Example ===\n");
    
    StackAllocator* stack = stack_create(4096);
    
    // Phase 1: Initial allocations
    printf("Phase 1:\n");
    int* data1 = (int*)stack_alloc(stack, 50 * sizeof(int));
    printf("  Allocated data1\n");
    stack_print_stats(stack);
    
    // Save checkpoint
    size_t checkpoint = stack_get_marker(stack);
    printf("\nCheckpoint saved at offset %zu\n", checkpoint);
    
    // Phase 2: Temporary allocations
    printf("\nPhase 2 (temporary):\n");
    char* temp1 = (char*)stack_alloc(stack, 100);
    char* temp2 = (char*)stack_alloc(stack, 200);
    char* temp3 = (char*)stack_alloc(stack, 150);
    printf("  Allocated temp1, temp2, temp3\n");
    stack_print_stats(stack);
    
    // Restore to checkpoint (frees everything after it)
    printf("\nRestoring to checkpoint:\n");
    stack_free_to_marker(stack, checkpoint);
    stack_print_stats(stack);
    
    // data1 still valid!
    data1[0] = 42;
    printf("  data1[0] = %d (still valid!)\n", data1[0]);
    
    stack_destroy(stack);
}

int main(void) {
    printf("=== Stack (LIFO) Allocator ===\n");
    printf("Like arena, but can free in reverse order\n");
    
    // Example 1: Basic usage
    printf("\n=== Example 1: Basic Usage ===\n");
    StackAllocator* stack = stack_create(1024);
    
    printf("Created stack allocator with 1024 bytes\n");
    stack_print_stats(stack);
    
    int* a = (int*)stack_alloc(stack, sizeof(int));
    *a = 10;
    printf("\nAllocated 'a': %d\n", *a);
    stack_print_stats(stack);
    
    int* b = (int*)stack_alloc(stack, sizeof(int));
    *b = 20;
    printf("\nAllocated 'b': %d\n", *b);
    stack_print_stats(stack);
    
    int* c = (int*)stack_alloc(stack, sizeof(int));
    *c = 30;
    printf("\nAllocated 'c': %d\n", *c);
    stack_print_stats(stack);
    
    // Free in reverse order
    printf("\nFreeing 'c':\n");
    stack_free(stack, c);
    stack_print_stats(stack);
    
    printf("\nFreeing 'b':\n");
    stack_free(stack, b);
    stack_print_stats(stack);
    
    printf("\nFreeing 'a':\n");
    stack_free(stack, a);
    stack_print_stats(stack);
    
    stack_destroy(stack);
    
    // Example 2: Recursive function
    printf("\n\n=== Example 2: Recursive Function ===\n");
    StackAllocator* rec_stack = stack_create(8192);
    
    printf("Computing factorial(5) with stack allocator:\n");
    int result = factorial_with_stack(rec_stack, 5, 0);
    printf("Result: 5! = %d\n", result);
    
    printf("\nStack after recursion:\n");
    stack_print_stats(rec_stack);
    printf("(All temp allocations freed automatically!)\n");
    
    stack_destroy(rec_stack);
    
    // Example 3: Nested scopes
    nested_scope_example();
    
    // Example 4: Checkpoints
    checkpoint_example();
    
    printf("\n\n=== Summary ===\n");
    printf("Stack allocator advantages:\n");
    printf("  - Very fast (just bump pointer)\n");
    printf("  - Can free individual allocations (in reverse order)\n");
    printf("  - Checkpoint/restore for batch free\n");
    printf("  - Natural for recursion and scoped lifetimes\n");
    printf("\nLimitations:\n");
    printf("  - Must free in LIFO order\n");
    printf("  - Fixed maximum size\n");
    printf("\nBest for:\n");
    printf("  - Recursive algorithms\n");
    printf("  - Scoped temporary allocations\n");
    printf("  - Parser call stacks\n");
    printf("  - Expression evaluation\n");
    
    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}

/*
 * Stack Allocator Explained:
 * 
 * Like a real call stack:
 * - Push: Allocate (bump pointer up)
 * - Pop: Free (bump pointer down)
 * - Must free in reverse order
 * 
 * Structure:
 * ┌────────────────────────────┐
 * │AAA│BBB│CCC│░░░░░░░░░░░░░░░│
 * └────────────────────────────┘
 *            ↑ top pointer
 * 
 * Allocate C: Move pointer right
 * Free C: Move pointer left to where B ends
 * Free B: Move pointer left to where A ends
 * 
 * Must maintain LIFO order!
 * 
 * Use Cases:
 * 
 * 1. Recursive Functions:
 *    Each call allocates temp data
 *    Returns free in reverse order naturally
 * 
 * 2. Scoped Allocations:
 *    {
 *        void* a = stack_alloc(stack, size);
 *        // Use a
 *        stack_free(stack, a);
 *    }
 * 
 * 3. Checkpoints:
 *    size_t mark = stack_get_marker(stack);
 *    // Do work with temporary allocations
 *    stack_free_to_marker(stack, mark);
 * 
 * Advantages:
 * - Fast (like arena)
 * - Can free individual (unlike arena)
 * - No fragmentation
 * - Natural lifetime management
 * 
 * Disadvantages:
 * - LIFO constraint
 * - Fixed size
 * - More complex than arena
 */
