/*
 * 02_selection_sort.c
 * 
 * Selection sort - find minimum element and swap to front.
 * Minimizes number of swaps (useful when swaps are expensive).
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void print_array(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void selection_sort(int arr[], int n, int show_steps) {
    int swap_count = 0;
    int comparison_count = 0;
    
    printf("Starting Selection Sort...\n\n");
    
    for (int i = 0; i < n - 1; i++) {
        int min_idx = i;
        
        // Find minimum in unsorted portion
        for (int j = i + 1; j < n; j++) {
            comparison_count++;
            if (arr[j] < arr[min_idx]) {
                min_idx = j;
            }
        }
        
        // Swap if needed
        if (min_idx != i) {
            int temp = arr[i];
            arr[i] = arr[min_idx];
            arr[min_idx] = temp;
            swap_count++;
            
            if (show_steps) {
                printf("Pass %d: Found min=%d at index %d, swapped with index %d\n",
                       i + 1, arr[i], min_idx, i);
                printf("  Array: ");
                print_array(arr, n);
            }
        } else if (show_steps) {
            printf("Pass %d: Min already at index %d (no swap needed)\n", i + 1, i);
            printf("  Array: ");
            print_array(arr, n);
        }
    }
    
    printf("\nTotal comparisons: %d\n", comparison_count);
    printf("Total swaps: %d\n", swap_count);
}

int main(void) {
    printf("=== Selection Sort ===\n\n");
    
    // Example 1: Small array with visualization
    printf("Example 1: Small array with steps\n");
    printf("----------------------------------\n");
    int arr1[] = {64, 25, 12, 22, 11};
    int n1 = sizeof(arr1) / sizeof(arr1[0]);
    
    printf("Original: ");
    print_array(arr1, n1);
    printf("\n");
    
    selection_sort(arr1, n1, 1);
    
    printf("\nSorted: ");
    print_array(arr1, n1);
    
    // Example 2: Compare swap counts
    printf("\n\nExample 2: Comparing swap counts\n");
    printf("---------------------------------\n");
    
    int arr2[] = {5, 4, 3, 2, 1};  // Worst case for many sorts
    int n2 = sizeof(arr2) / sizeof(arr2[0]);
    
    printf("Reverse sorted array: ");
    print_array(arr2, n2);
    printf("\n");
    
    selection_sort(arr2, n2, 0);
    
    printf("\nSorted: ");
    print_array(arr2, n2);
    printf("Note: Only %d swaps needed (one per element at most)\n", n2 - 1);
    
    // Example 3: Timing on larger dataset
    printf("\n\nExample 3: Performance test (1000 elements)\n");
    printf("--------------------------------------------\n");
    
    int n3 = 1000;
    int *arr3 = (int*)malloc(n3 * sizeof(int));
    
    srand(time(NULL));
    for (int i = 0; i < n3; i++) {
        arr3[i] = rand() % 1000;
    }
    
    printf("Sorting %d random numbers...\n", n3);
    
    clock_t start = clock();
    selection_sort(arr3, n3, 0);
    clock_t end = clock();
    
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Sorted in %.4f seconds\n", time_taken);
    
    printf("First 10: ");
    for (int i = 0; i < 10; i++) {
        printf("%d ", arr3[i]);
    }
    printf("\n");
    
    free(arr3);
    
    printf("\n\nPress Enter to exit...");
    getchar();
    return 0;
}

/*
 * How Selection Sort Works:
 * 
 * 1. Find minimum in unsorted portion
 * 2. Swap it with first unsorted element
 * 3. Move boundary between sorted/unsorted right
 * 4. Repeat until done
 * 
 * Example: [64, 25, 12, 22, 11]
 * Find min (11): [11, 25, 12, 22, 64]
 * Find min (12): [11, 12, 25, 22, 64]
 * Find min (22): [11, 12, 22, 25, 64]
 * Find min (25): [11, 12, 22, 25, 64]
 * 
 * Complexity:
 * - Time: O(n²) - always (even if sorted)
 * - Space: O(1) - in place
 * - Stable: No (can swap equal elements past each other)
 * 
 * Key insight: Makes at most n-1 swaps!
 * 
 * When to use:
 * - Minimize number of swaps
 * - Small datasets
 * - Memory writes are expensive
 * 
 * Advantage over bubble sort:
 * - Fewer swaps (bubble sort can make O(n²) swaps)
 * 
 * Try:
 * - Compare swap count to bubble sort
 * - Time on already sorted vs reverse sorted
 * - Implement as stable version
 */
