/*
 * 06_arrays.c
 * 
 * Arrays are fixed-size collections of the same type.
 */

#include <stdio.h>

int main(void) {
    // Declare and initialize
    int numbers[5] = {10, 20, 30, 40, 50};
    
    printf("=== Array Basics ===\n");
    printf("First element: %d\n", numbers[0]);
    printf("Third element: %d\n", numbers[2]);
    
    // Arrays are zero-indexed: 0, 1, 2, 3, 4
    
    // Modify elements
    numbers[2] = 999;
    printf("After changing: %d\n", numbers[2]);
    
    // Loop through array
    printf("\n=== Print All Elements ===\n");
    for (int i = 0; i < 5; i++) {
        printf("numbers[%d] = %d\n", i, numbers[i]);
    }
    
    // Partial initialization
    int partial[10] = {1, 2, 3};  // Rest are 0
    printf("\n=== Partial Init ===\n");
    for (int i = 0; i < 10; i++) {
        printf("%d ", partial[i]);
    }
    printf("\n");
    
    // Multi-dimensional arrays
    printf("\n=== 2D Array ===\n");
    int matrix[3][3] = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };
    
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            printf("%d ", matrix[row][col]);
        }
        printf("\n");
    }
    
    // Arrays and pointers
    printf("\n=== Arrays are Pointers ===\n");
    int arr[3] = {100, 200, 300};
    int *ptr = arr;  // arr is the address of first element
    
    printf("arr[0] = %d\n", arr[0]);
    printf("*ptr = %d\n", *ptr);      // Same thing
    printf("*(ptr+1) = %d\n", *(ptr+1));  // Second element
    
    return 0;
}

/*
 * Array syntax:
 *   type name[size];              - Declaration
 *   int arr[5] = {1,2,3,4,5};    - Initialize
 *   arr[0]                       - Access first element
 *   arr[size-1]                  - Last element
 * 
 * Important:
 *   - Size must be known at compile time (or use malloc)
 *   - No bounds checking - arr[100] on a 5-element array = crash
 *   - Array name is a pointer to first element
 * 
 * 2D arrays:
 *   int grid[rows][cols];
 *   grid[y][x] to access element
 * 
 * Try:
 *   - Create different sized arrays
 *   - Try accessing invalid indices (what happens?)
 *   - Loop through 2D array
 *   - Calculate sum/average of array elements
 */
