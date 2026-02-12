/*
 * 05_tracking_allocator.c
 * 
 * Wrapper around malloc/free to track allocations and detect leaks.
 * Useful for finding where memory is allocated and never freed.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ===== Tracking Allocator =====

typedef struct Allocation {
    void* ptr;
    size_t size;
    const char* file;
    int line;
    struct Allocation* next;
} Allocation;

typedef struct {
    Allocation* allocations;
    size_t total_allocated;
    size_t total_freed;
    size_t current_usage;
    size_t peak_usage;
    size_t allocation_count;
    size_t free_count;
} MemoryTracker;

MemoryTracker g_tracker = {0};

// Tracked malloc
void* tracked_malloc(size_t size, const char* file, int line) {
    void* ptr = malloc(size);
    
    if (ptr) {
        // Add to tracking list
        Allocation* alloc = (Allocation*)malloc(sizeof(Allocation));
        alloc->ptr = ptr;
        alloc->size = size;
        alloc->file = file;
        alloc->line = line;
        alloc->next = g_tracker.allocations;
        g_tracker.allocations = alloc;
        
        // Update stats
        g_tracker.total_allocated += size;
        g_tracker.current_usage += size;
        g_tracker.allocation_count++;
        
        if (g_tracker.current_usage > g_tracker.peak_usage) {
            g_tracker.peak_usage = g_tracker.current_usage;
        }
    }
    
    return ptr;
}

// Tracked free
void tracked_free(void* ptr, const char* file, int line) {
    if (ptr == NULL) return;
    
    // Find in tracking list
    Allocation** current = &g_tracker.allocations;
    while (*current) {
        if ((*current)->ptr == ptr) {
            Allocation* to_remove = *current;
            *current = (*current)->next;
            
            // Update stats
            g_tracker.total_freed += to_remove->size;
            g_tracker.current_usage -= to_remove->size;
            g_tracker.free_count++;
            
            free(to_remove);
            free(ptr);
            return;
        }
        current = &(*current)->next;
    }
    
    // Not found - double free or never tracked!
    printf("WARNING: Freeing untracked pointer %p at %s:%d\n", ptr, file, line);
    printf("  (Possible double free or wrong pointer)\n");
}

// Print current allocations
void print_allocations() {
    printf("\n=== Current Allocations ===\n");
    
    if (g_tracker.allocations == NULL) {
        printf("(none)\n");
        return;
    }
    
    int count = 0;
    Allocation* current = g_tracker.allocations;
    while (current) {
        printf("  %p: %zu bytes at %s:%d\n", 
               current->ptr, current->size, current->file, current->line);
        count++;
        current = current->next;
    }
    printf("Total: %d allocations\n", count);
}

// Print memory statistics
void print_memory_stats() {
    printf("\n=== Memory Statistics ===\n");
    printf("Total allocated: %zu bytes\n", g_tracker.total_allocated);
    printf("Total freed:     %zu bytes\n", g_tracker.total_freed);
    printf("Current usage:   %zu bytes\n", g_tracker.current_usage);
    printf("Peak usage:      %zu bytes\n", g_tracker.peak_usage);
    printf("Allocation count: %zu\n", g_tracker.allocation_count);
    printf("Free count:       %zu\n", g_tracker.free_count);
    printf("Leaked:          %zu bytes\n", 
           g_tracker.total_allocated - g_tracker.total_freed);
}

// Check for memory leaks
void check_leaks() {
    printf("\n=== Leak Detection ===\n");
    
    if (g_tracker.allocations == NULL) {
        printf("No leaks detected!\n");
        return;
    }
    
    printf("MEMORY LEAKS DETECTED:\n");
    Allocation* current = g_tracker.allocations;
    size_t total_leaked = 0;
    int leak_count = 0;
    
    while (current) {
        printf("  Leak: %zu bytes at %s:%d\n", 
               current->size, current->file, current->line);
        total_leaked += current->size;
        leak_count++;
        current = current->next;
    }
    
    printf("\nTotal: %d leaks, %zu bytes lost\n", leak_count, total_leaked);
}

// Macros for automatic file/line tracking
#define MALLOC(size) tracked_malloc(size, __FILE__, __LINE__)
#define FREE(ptr) tracked_free(ptr, __FILE__, __LINE__)

// ===== Example Functions =====

void example_clean_code() {
    printf("\n=== Example 1: Clean Code (No Leaks) ===\n");
    
    int* data = (int*)MALLOC(100 * sizeof(int));
    printf("Allocated 100 integers\n");
    
    for (int i = 0; i < 100; i++) {
        data[i] = i;
    }
    
    FREE(data);
    printf("Freed memory\n");
    
    print_memory_stats();
}

void example_with_leak() {
    printf("\n=== Example 2: Code With Leak ===\n");
    
    char* str1 = (char*)MALLOC(50);
    strcpy(str1, "This will be freed");
    
    char* str2 = (char*)MALLOC(100);
    strcpy(str2, "This will be leaked!");
    
    printf("Allocated two strings\n");
    print_allocations();
    
    FREE(str1);
    // Forgot to free str2!
    
    printf("\nAfter freeing str1:\n");
    print_allocations();
}

void example_complex_structure() {
    printf("\n=== Example 3: Complex Structure ===\n");
    
    typedef struct {
        char* name;
        int* values;
        size_t count;
    } Data;
    
    Data* data = (Data*)MALLOC(sizeof(Data));
    data->name = (char*)MALLOC(50);
    data->values = (int*)MALLOC(10 * sizeof(int));
    data->count = 10;
    
    strcpy(data->name, "Test Data");
    for (size_t i = 0; i < data->count; i++) {
        data->values[i] = i * 10;
    }
    
    printf("Created complex structure\n");
    print_allocations();
    
    // Proper cleanup: free in reverse order
    FREE(data->values);
    FREE(data->name);
    FREE(data);
    
    printf("\nAfter cleanup:\n");
    print_allocations();
}

int main(void) {
    printf("=== Tracking Allocator ===\n");
    printf("Wrapper to track allocations and find leaks\n");
    
    // Example 1: Clean code
    example_clean_code();
    
    // Example 2: Intentional leak
    example_with_leak();
    
    // Example 3: Complex structure
    example_complex_structure();
    
    // Final report
    print_memory_stats();
    check_leaks();
    
    printf("\n\n=== How to Use This ===\n");
    printf("Replace malloc/free with MALLOC/FREE macros\n");
    printf("Automatically tracks:\n");
    printf("  - Where memory was allocated (file:line)\n");
    printf("  - How much memory allocated\n");
    printf("  - What hasn't been freed (leaks)\n");
    printf("  - Peak memory usage\n");
    printf("\nAt program exit, call check_leaks() to find leaks!\n");
    
    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}

/*
 * Tracking Allocator Explained:
 * 
 * Wraps malloc/free to record:
 * - Pointer address
 * - Allocation size
 * - Source file and line
 * - When allocated
 * 
 * Uses a linked list to track all active allocations:
 * 
 * allocations -> [ptr1, 100 bytes, main.c:10] -> [ptr2, 50 bytes, foo.c:25] -> NULL
 * 
 * On malloc:
 * - Actually malloc the memory
 * - Create tracking node
 * - Add to linked list
 * - Update statistics
 * 
 * On free:
 * - Find in tracking list
 * - Remove from list
 * - Update statistics
 * - Actually free the memory
 * 
 * Advantages:
 * - Find leaks at runtime
 * - See exact allocation locations
 * - Track memory usage patterns
 * - Detect double frees
 * - No special tools needed
 * 
 * Disadvantages:
 * - Overhead per allocation (tracking node)
 * - Slower than raw malloc/free
 * - Only use in debug builds
 * 
 * Usage:
 * 
 * #ifdef DEBUG
 *   #define malloc(size) tracked_malloc(size, __FILE__, __LINE__)
 *   #define free(ptr) tracked_free(ptr, __FILE__, __LINE__)
 * #endif
 * 
 * At program exit:
 *   check_leaks();  // Prints any leaked memory
 * 
 * Real implementations (valgrind, ASan) are more sophisticated,
 * but this shows the core concept!
 */
