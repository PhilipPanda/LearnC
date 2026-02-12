/*
 * 01_memory_bugs.c
 * 
 * Demonstrates common memory bugs and how to detect them.
 * Compile with: gcc -g 01_memory_bugs.c -o 01_memory_bugs
 * Run with sanitizer: gcc -fsanitize=address -g 01_memory_bugs.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ===== Memory Leak =====
void example_memory_leak() {
    printf("\n=== Example 1: Memory Leak ===\n");
    
    // Allocate memory but never free it
    int* leaked = (int*)malloc(100 * sizeof(int));
    leaked[0] = 42;
    printf("Allocated memory at %p, value: %d\n", (void*)leaked, leaked[0]);
    
    // LEAK! We return without freeing
    printf("Memory is leaked - never freed!\n");
}

// ===== Double Free =====
void example_double_free(int trigger) {
    printf("\n=== Example 2: Double Free ===\n");
    
    int* ptr = (int*)malloc(sizeof(int));
    *ptr = 42;
    printf("Allocated: %p, value: %d\n", (void*)ptr, *ptr);
    
    free(ptr);
    printf("Freed once\n");
    
    if (trigger) {
        printf("WARNING: About to double free!\n");
        // Uncomment to trigger bug (will crash or corrupt)
        // free(ptr);  // DOUBLE FREE!
    }
    
    printf("Avoided double free by not uncommenting\n");
}

// ===== Use After Free =====
void example_use_after_free(int trigger) {
    printf("\n=== Example 3: Use After Free ===\n");
    
    int* ptr = (int*)malloc(sizeof(int));
    *ptr = 42;
    printf("Before free: %d\n", *ptr);
    
    free(ptr);
    printf("Freed memory\n");
    
    if (trigger) {
        printf("WARNING: About to use freed memory!\n");
        // Uncomment to trigger bug (undefined behavior)
        // printf("After free: %d\n", *ptr);  // USE AFTER FREE!
        // *ptr = 99;  // Writing to freed memory!
    }
    
    printf("Avoided use-after-free by not uncommenting\n");
}

// ===== Buffer Overflow =====
void example_buffer_overflow(int trigger) {
    printf("\n=== Example 4: Buffer Overflow ===\n");
    
    char* buffer = (char*)malloc(10);  // Only 10 bytes
    strcpy(buffer, "Short");  // OK - 6 bytes
    printf("Buffer: %s\n", buffer);
    
    if (trigger) {
        printf("WARNING: About to overflow buffer!\n");
        // Uncomment to trigger bug (writes past end)
        // strcpy(buffer, "This string is way too long!");  // OVERFLOW!
    }
    
    printf("Avoided overflow by not uncommenting\n");
    free(buffer);
}

// ===== Uninitialized Memory =====
void example_uninitialized() {
    printf("\n=== Example 5: Uninitialized Memory ===\n");
    
    int* uninit = (int*)malloc(sizeof(int));
    // Never initialized!
    printf("Uninitialized value (garbage): %d\n", *uninit);
    
    // Correct way
    int* init = (int*)malloc(sizeof(int));
    *init = 0;  // Initialize!
    printf("Initialized value: %d\n", *init);
    
    free(uninit);
    free(init);
}

// ===== Wrong Size Allocation =====
void example_wrong_size() {
    printf("\n=== Example 6: Wrong Size Allocation ===\n");
    
    // Common mistake: forgetting sizeof
    int* wrong = (int*)malloc(10);  // Only 10 bytes, not 10 ints!
    printf("Wrong allocation: %zu bytes (should be %zu)\n", 
           (size_t)10, 10 * sizeof(int));
    
    // This writes past the end!
    for (int i = 0; i < 10; i++) {
        wrong[i] = i;  // OVERFLOW after i=1 on 64-bit, i=2 on 32-bit
    }
    
    // Correct way
    int* correct = (int*)malloc(10 * sizeof(int));
    printf("Correct allocation: %zu bytes\n", 10 * sizeof(int));
    for (int i = 0; i < 10; i++) {
        correct[i] = i;
    }
    
    free(wrong);
    free(correct);
}

// ===== Memory Corruption via NULL =====
void example_null_pointer(int trigger) {
    printf("\n=== Example 7: NULL Pointer Dereference ===\n");
    
    int* ptr = (int*)malloc(sizeof(int));
    
    if (ptr == NULL) {
        printf("Allocation failed!\n");
        return;
    }
    
    *ptr = 42;
    printf("Value: %d\n", *ptr);
    free(ptr);
    ptr = NULL;  // Good practice!
    
    if (trigger) {
        printf("WARNING: About to dereference NULL!\n");
        // Uncomment to trigger crash
        // *ptr = 99;  // CRASH!
    }
    
    printf("Avoided NULL dereference by not uncommenting\n");
}

// ===== Dangling Pointer =====
char* example_dangling_pointer() {
    printf("\n=== Example 8: Returning Stack Memory ===\n");
    
    char buffer[100];  // Stack memory
    strcpy(buffer, "Hello");
    printf("Inside function: %s\n", buffer);
    
    // DANGER: Returning pointer to stack memory!
    // buffer will be invalid after function returns
    printf("This pointer will be invalid after return: %p\n", (void*)buffer);
    
    // Correct way: allocate on heap
    char* heap_buffer = (char*)malloc(100);
    strcpy(heap_buffer, "Hello from heap");
    return heap_buffer;  // Caller must free this!
}

// ===== Correct Memory Management =====
void example_correct_usage() {
    printf("\n=== Example 9: Correct Memory Management ===\n");
    
    // 1. Allocate
    int* data = (int*)malloc(10 * sizeof(int));
    
    // 2. Check for NULL
    if (data == NULL) {
        fprintf(stderr, "Allocation failed!\n");
        return;
    }
    
    // 3. Use the memory
    for (int i = 0; i < 10; i++) {
        data[i] = i * i;
    }
    
    printf("Data: ");
    for (int i = 0; i < 10; i++) {
        printf("%d ", data[i]);
    }
    printf("\n");
    
    // 4. Free when done
    free(data);
    
    // 5. Set to NULL (optional but safe)
    data = NULL;
    
    printf("All memory properly managed!\n");
}

int main(void) {
    printf("=== Memory Bugs Demonstration ===\n");
    printf("NOTE: Some bugs are commented out to prevent crashes\n");
    printf("Uncomment them to see actual bugs (use sanitizers!)\n");
    
    // Run examples (set to 0 to avoid triggering bugs)
    int trigger_bugs = 0;
    
    example_memory_leak();
    example_double_free(trigger_bugs);
    example_use_after_free(trigger_bugs);
    example_buffer_overflow(trigger_bugs);
    example_uninitialized();
    example_wrong_size();
    example_null_pointer(trigger_bugs);
    
    char* result = example_dangling_pointer();
    printf("\nReturned from function: %s (but allocated on heap)\n", result);
    free(result);  // Must free!
    
    example_correct_usage();
    
    printf("\n=== Demonstrations Complete ===\n");
    printf("\nTo detect bugs, compile with:\n");
    printf("  gcc -fsanitize=address -g 01_memory_bugs.c\n");
    printf("Or run with Valgrind:\n");
    printf("  valgrind --leak-check=full ./01_memory_bugs\n");
    
    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}

/*
 * How to Use This Example:
 * 
 * 1. Compile normally:
 *    gcc 01_memory_bugs.c -o bugs
 *    ./bugs
 *    (Shows safe version with explanations)
 * 
 * 2. Compile with AddressSanitizer:
 *    gcc -fsanitize=address -g 01_memory_bugs.c -o bugs
 *    (Set trigger_bugs = 1 and uncomment code to see bugs caught)
 * 
 * 3. Run with Valgrind:
 *    valgrind --leak-check=full ./bugs
 *    (Detects the memory leak even without uncommenting)
 * 
 * Common Bugs Demonstrated:
 * - Memory leak: Allocated but never freed
 * - Double free: Freeing same memory twice
 * - Use-after-free: Using memory after freeing
 * - Buffer overflow: Writing past allocation
 * - Uninitialized memory: Reading before writing
 * - Wrong size: malloc(10) instead of malloc(10*sizeof(int))
 * - NULL dereference: Using NULL pointer
 * - Dangling pointer: Returning stack memory
 * 
 * Prevention:
 * - Always free what you malloc
 * - Set pointers to NULL after free
 * - Check all allocations for NULL
 * - Use sizeof() correctly
 * - Initialize all memory
 * - Use sanitizers and Valgrind
 */
