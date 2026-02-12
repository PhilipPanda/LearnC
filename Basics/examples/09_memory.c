/*
 * 09_memory.c
 * 
 * Dynamic memory allocation - create and free memory at runtime.
 */

#include <stdio.h>
#include <stdlib.h>  // malloc, free
#include <string.h>  // strcpy

int main(void) {
    // Allocate memory for 5 integers
    printf("=== malloc ===\n");
    int *numbers = malloc(5 * sizeof(int));
    
    // Always check if allocation succeeded
    if (numbers == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }
    
    // Use it like an array
    for (int i = 0; i < 5; i++) {
        numbers[i] = i * 10;
    }
    
    printf("Numbers: ");
    for (int i = 0; i < 5; i++) {
        printf("%d ", numbers[i]);
    }
    printf("\n");
    
    // Free memory when done
    free(numbers);
    numbers = NULL;  // Good practice
    
    // Allocate and initialize to zero
    printf("\n=== calloc ===\n");
    int *zeros = calloc(5, sizeof(int));  // Allocates and zeros memory
    if (zeros == NULL) {
        return 1;
    }
    
    printf("Calloc'd array: ");
    for (int i = 0; i < 5; i++) {
        printf("%d ", zeros[i]);  // All zeros
    }
    printf("\n");
    
    free(zeros);
    
    // Resize allocated memory
    printf("\n=== realloc ===\n");
    int *arr = malloc(3 * sizeof(int));
    if (arr == NULL) return 1;
    
    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;
    
    // Make it bigger
    arr = realloc(arr, 5 * sizeof(int));
    if (arr == NULL) return 1;
    
    arr[3] = 4;
    arr[4] = 5;
    
    printf("Resized array: ");
    for (int i = 0; i < 5; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
    
    free(arr);
    
    // Dynamic string
    printf("\n=== Dynamic Strings ===\n");
    char *message = malloc(50 * sizeof(char));
    if (message == NULL) return 1;
    
    strcpy(message, "Hello from heap memory!");
    printf("%s\n", message);
    
    free(message);
    
    return 0;
}

/*
 * Dynamic memory functions (need #include <stdlib.h>):
 * 
 * malloc(size)
 *   - Allocates 'size' bytes
 *   - Returns pointer or NULL if failed
 *   - Memory is uninitialized (contains garbage)
 * 
 * calloc(count, size)
 *   - Allocates count * size bytes
 *   - Initializes all bytes to zero
 *   - Slower than malloc
 * 
 * realloc(ptr, new_size)
 *   - Resizes previously allocated memory
 *   - May move the block to a new location
 *   - Returns new pointer (could be same or different)
 * 
 * free(ptr)
 *   - Releases memory back to system
 *   - Must free everything you malloc
 *   - Don't free twice (crash)
 *   - Don't use after free (crash)
 * 
 * Stack vs Heap:
 *   int x;           - Stack (automatic, limited size)
 *   int *x = malloc  - Heap (manual, large size)
 * 
 * Golden rules:
 *   1. Always check if malloc returns NULL
 *   2. Every malloc needs a matching free
 *   3. Don't use memory after freeing it
 *   4. Set pointers to NULL after freeing
 * 
 * Memory leaks happen when you:
 *   - Forget to free
 *   - Lose the pointer before freeing
 *   - Return from function without freeing
 * 
 * Try:
 *   - Allocate arrays of different sizes
 *   - What happens if you forget free? (memory leak)
 *   - Create a dynamic 2D array
 */
