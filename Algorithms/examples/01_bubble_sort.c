/*
 * 01_bubble_sort.c
 * 
 * Bubble sort - repeatedly swap adjacent elements if they're in wrong order.
 * Simplest sorting algorithm. Good for learning, terrible for performance.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Print array
void print_array(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

// Visual bar representation
void print_bars(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        printf("%2d |", arr[i]);
        for (int j = 0; j < arr[i]; j++) {
            printf("█");
        }
        printf("\n");
    }
}

// Bubble sort with visualization
void bubble_sort(int arr[], int n, int show_steps) {
    printf("Starting Bubble Sort...\n\n");
    
    for (int i = 0; i < n - 1; i++) {
        int swapped = 0;
        
        if (show_steps) {
            printf("Pass %d:\n", i + 1);
        }
        
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                // Swap
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
                swapped = 1;
                
                if (show_steps) {
                    printf("  Swapped %d and %d: ", temp, arr[j]);
                    print_array(arr, n);
                }
            }
        }
        
        if (show_steps) {
            printf("  After pass %d: ", i + 1);
            print_array(arr, n);
            printf("\n");
        }
        
        // Optimization: if no swaps, array is sorted
        if (!swapped) {
            if (show_steps) {
                printf("  No swaps! Array is sorted.\n");
            }
            break;
        }
    }
}

int main(void) {
    printf("=== Bubble Sort ===\n\n");
    
    // Small example with visualization
    printf("Example 1: Small array with steps\n");
    printf("----------------------------------\n");
    int arr1[] = {64, 34, 25, 12, 22, 11, 90};
    int n1 = sizeof(arr1) / sizeof(arr1[0]);
    
    printf("Original: ");
    print_array(arr1, n1);
    printf("\n");
    
    bubble_sort(arr1, n1, 1);  // Show steps
    
    printf("Sorted: ");
    print_array(arr1, n1);
    printf("\n");
    print_bars(arr1, n1);
    
    // Larger example with timing
    printf("\n\nExample 2: Larger array (1000 elements)\n");
    printf("----------------------------------------\n");
    
    int n2 = 1000;
    int *arr2 = (int*)malloc(n2 * sizeof(int));
    
    // Fill with random numbers
    srand(time(NULL));
    for (int i = 0; i < n2; i++) {
        arr2[i] = rand() % 1000;
    }
    
    printf("Sorting %d random numbers...\n", n2);
    
    clock_t start = clock();
    bubble_sort(arr2, n2, 0);  // No steps, just time it
    clock_t end = clock();
    
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Sorted in %.4f seconds\n", time_taken);
    printf("First 10: ");
    for (int i = 0; i < 10; i++) {
        printf("%d ", arr2[i]);
    }
    printf("\n");
    
    // Already sorted test
    printf("\n\nExample 3: Already sorted array\n");
    printf("--------------------------------\n");
    int arr3[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int n3 = sizeof(arr3) / sizeof(arr3[0]);
    
    printf("Original: ");
    print_array(arr3, n3);
    
    start = clock();
    bubble_sort(arr3, n3, 0);
    end = clock();
    
    time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Sorted in %.6f seconds (optimized - detected already sorted)\n", time_taken);
    
    free(arr2);
    
    printf("\n\nPress Enter to exit...");
    getchar();
    return 0;
}

/*
 * How Bubble Sort Works:
 * 
 * 1. Compare adjacent elements
 * 2. Swap if out of order
 * 3. Largest element "bubbles" to the end
 * 4. Repeat for remaining elements
 * 
 * Example: [5, 2, 8, 1]
 * Pass 1: [2, 5, 1, 8]  (8 in place)
 * Pass 2: [2, 1, 5, 8]  (5 in place)
 * Pass 3: [1, 2, 5, 8]  (sorted!)
 * 
 * Complexity:
 * - Time: O(n²) worst/average, O(n) best (already sorted)
 * - Space: O(1) - sorts in place
 * - Stable: Yes - equal elements keep order
 * 
 * When to use:
 * - Educational purposes
 * - Tiny datasets (< 10 elements)
 * - Nearly sorted data (with optimization)
 * 
 * Never use in production for large data!
 * 
 * Try:
 * - Change array sizes and measure time
 * - Remove the optimization and compare
 * - Try reverse sorted array (worst case)
 * - Compare to other sorting algorithms
 */
