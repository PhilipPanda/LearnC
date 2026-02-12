/*
 * 03_insertion_sort.c
 * 
 * Insertion sort - build sorted array by inserting each element in correct position.
 * Efficient for small or nearly sorted data.
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

void print_with_marker(int arr[], int n, int sorted_end) {
    for (int i = 0; i < n; i++) {
        if (i == sorted_end) printf("| ");
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void insertion_sort(int arr[], int n, int show_steps) {
    printf("Starting Insertion Sort...\n\n");
    
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;
        
        if (show_steps) {
            printf("Insert %d into sorted portion: ", key);
            print_with_marker(arr, n, i);
        }
        
        // Shift elements greater than key to right
        int shifts = 0;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
            shifts++;
        }
        arr[j + 1] = key;
        
        if (show_steps) {
            printf("  After %d shifts: ", shifts);
            print_array(arr, n);
            printf("\n");
        }
    }
}

int main(void) {
    printf("=== Insertion Sort ===\n\n");
    
    // Example 1: Small array with steps
    printf("Example 1: Building sorted array step-by-step\n");
    printf("----------------------------------------------\n");
    int arr1[] = {12, 11, 13, 5, 6};
    int n1 = sizeof(arr1) / sizeof(arr1[0]);
    
    printf("Original: ");
    print_array(arr1, n1);
    printf("(| marks boundary between sorted and unsorted)\n\n");
    
    insertion_sort(arr1, n1, 1);
    
    printf("Sorted: ");
    print_array(arr1, n1);
    
    // Example 2: Nearly sorted (best case)
    printf("\n\nExample 2: Nearly sorted array (best case)\n");
    printf("-------------------------------------------\n");
    int arr2[] = {1, 2, 3, 4, 5, 6, 8, 7};  // Only last element out of place
    int n2 = sizeof(arr2) / sizeof(arr2[0]);
    
    printf("Original: ");
    print_array(arr2, n2);
    
    clock_t start = clock();
    insertion_sort(arr2, n2, 0);
    clock_t end = clock();
    
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Sorted in %.6f seconds (very fast for nearly sorted!)\n", time_taken);
    printf("Result: ");
    print_array(arr2, n2);
    
    // Example 3: Reverse sorted (worst case)
    printf("\n\nExample 3: Reverse sorted array (worst case)\n");
    printf("---------------------------------------------\n");
    int arr3[] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    int n3 = sizeof(arr3) / sizeof(arr3[0]);
    
    printf("Original: ");
    print_array(arr3, n3);
    
    start = clock();
    insertion_sort(arr3, n3, 0);
    end = clock();
    
    time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Sorted in %.6f seconds (slower - many shifts needed)\n", time_taken);
    printf("Result: ");
    print_array(arr3, n3);
    
    // Example 4: Performance test
    printf("\n\nExample 4: Performance on random data (1000 elements)\n");
    printf("------------------------------------------------------\n");
    
    int n4 = 1000;
    int *arr4 = (int*)malloc(n4 * sizeof(int));
    
    srand(time(NULL));
    for (int i = 0; i < n4; i++) {
        arr4[i] = rand() % 1000;
    }
    
    printf("Sorting %d random numbers...\n", n4);
    
    start = clock();
    insertion_sort(arr4, n4, 0);
    end = clock();
    
    time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Sorted in %.4f seconds\n", time_taken);
    
    printf("First 10: ");
    for (int i = 0; i < 10; i++) {
        printf("%d ", arr4[i]);
    }
    printf("\n");
    
    free(arr4);
    
    printf("\n\nPress Enter to exit...");
    getchar();
    return 0;
}

/*
 * How Insertion Sort Works:
 * 
 * Like sorting playing cards:
 * 1. Start with one card (already sorted)
 * 2. Pick next card
 * 3. Insert it into correct position in sorted hand
 * 4. Repeat
 * 
 * Example: [12, 11, 13, 5, 6]
 * [12 | 11, 13, 5, 6] -> Insert 11
 * [11, 12 | 13, 5, 6] -> Insert 13
 * [11, 12, 13 | 5, 6] -> Insert 5
 * [5, 11, 12, 13 | 6] -> Insert 6
 * [5, 6, 11, 12, 13]
 * 
 * Complexity:
 * - Time: O(n²) worst/average, O(n) best (nearly sorted)
 * - Space: O(1) - in place
 * - Stable: Yes
 * - Adaptive: Fast on nearly sorted data!
 * 
 * When to use:
 * - Small datasets (< 50 elements)
 * - Nearly sorted data (very fast!)
 * - Online sorting (data arrives over time)
 * - As part of hybrid algorithms (Timsort)
 * 
 * Key advantages:
 * - Simple implementation
 * - Efficient for small data
 * - Adaptive to input
 * - Stable
 * - Online
 * 
 * Used in practice:
 * - Python's Timsort uses it for small runs
 * - C++ Introsort uses it for small partitions
 * 
 * Try:
 * - Compare time on sorted vs random vs reverse sorted
 * - Find the crossover point where merge/quick sort wins
 * - Implement binary insertion sort variant
 */
