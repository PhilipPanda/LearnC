/*
 * 08_pointers.c
 * 
 * Pointers store memory addresses. They're the most important concept in C.
 */

#include <stdio.h>

// Function declarations
void swap(int *x, int *y);

int main(void) {
    // Regular variable
    int x = 42;
    printf("x = %d\n", x);
    printf("Address of x: %p\n", (void*)&x);  // &x = address of x
    
    // Pointer variable - stores an address
    int *ptr = &x;  // ptr points to x
    printf("\n=== Pointer Basics ===\n");
    printf("ptr (address it stores): %p\n", (void*)ptr);
    printf("*ptr (value at that address): %d\n", *ptr);
    
    // Change value through pointer
    *ptr = 100;  // *ptr = dereference (go to that address)
    printf("\nAfter *ptr = 100:\n");
    printf("x = %d\n", x);  // x changed too!
    
    // Why? Because ptr points to x's memory location
    // Changing *ptr changes the value at that location (which is x)
    
    // Pointer arithmetic
    printf("\n=== Pointer Arithmetic ===\n");
    int arr[5] = {10, 20, 30, 40, 50};
    int *p = arr;  // arr is the address of first element
    
    printf("*p = %d\n", *p);        // First element
    printf("*(p+1) = %d\n", *(p+1)); // Second element
    printf("*(p+2) = %d\n", *(p+2)); // Third element
    
    // Moving pointer
    p++;  // Move to next element
    printf("After p++: *p = %d\n", *p);
    
    // NULL pointer - points to nothing
    printf("\n=== NULL Pointers ===\n");
    int *null_ptr = NULL;  // Always initialize pointers!
    
    if (null_ptr == NULL) {
        printf("Pointer is NULL - don't dereference!\n");
    }
    
    // Common use: modify function parameters
    printf("\n=== Pointers in Functions ===\n");
    int a = 5;
    int b = 10;
    printf("Before swap: a=%d, b=%d\n", a, b);
    
    // Swap values (see function below)
    swap(&a, &b);  // Pass addresses
    printf("After swap: a=%d, b=%d\n", a, b);
    
    return 0;
}

// This function takes pointers, so it can modify the actual variables
void swap(int *x, int *y) {
    int temp = *x;
    *x = *y;
    *y = temp;
}

/*
 * Pointer operators:
 *   &var    - Address of var (returns pointer)
 *   *ptr    - Dereference ptr (get value at address)
 *   ptr++   - Move pointer to next element
 * 
 * Declaration:
 *   int *ptr;   - Pointer to int
 *   char *str;  - Pointer to char
 *   void *ptr;  - Generic pointer
 * 
 * Key concepts:
 *   int x = 5;       - x is a variable with value 5
 *   int *ptr = &x;   - ptr stores the address of x
 *   *ptr = 10;       - Go to address in ptr, set value to 10
 * 
 * Think of it like a house:
 *   x        - The house (has stuff inside)
 *   &x       - The address of the house
 *   *ptr     - Go to that address and look inside
 * 
 * Common mistakes:
 *   int *p;  *p = 5;     - WRONG: p not initialized
 *   int *p = NULL;       - RIGHT: initialize first
 *   if (p != NULL) *p = 5;  // Then check before using
 * 
 * Try:
 *   - Create pointers to different types
 *   - Modify variables through pointers
 *   - What happens if you print ptr vs *ptr?
 */
