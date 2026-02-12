/*
 * 05_quick_sort.c
 * 
 * Quick sort - fast, in-place sorting using divide and conquer.
 * Most commonly used general-purpose sorting algorithm.
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

void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Partition array around pivot (last element)
int partition(int arr[], int low, int high, int show_steps) {
    int pivot = arr[high];  // Choose last element as pivot
    int i = low - 1;        // Index of smaller element
    
    if (show_steps) {
        printf("    Partitioning with pivot=%d: ", pivot);
    }
    
    for (int j = low; j < high; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    
    if (show_steps) {
        printf("[");
        for (int k = low; k <= high; k++) {
            if (k == i + 1) printf("|%d| ", arr[k]);
            else printf("%d ", arr[k]);
        }
        printf("]\n");
    }
    
    return i + 1;
}

// Quick sort recursive
void quick_sort_recursive(int arr[], int low, int high, int show_steps, int *level) {
    if (low < high) {
        if (show_steps) {
            (*level)++;
            printf("  Level %d: Sorting [", *level);
            for (int i = low; i <= high; i++) {
                printf("%d%s", arr[i], i < high ? ", " : "");
            }
            printf("]\n");
        }
        
        // Partition and get pivot position
        int pi = partition(arr, low, high, show_steps);
        
        // Recursively sort elements before and after partition
        quick_sort_recursive(arr, low, pi - 1, show_steps, level);
        quick_sort_recursive(arr, pi + 1, high, show_steps, level);
        
        if (show_steps) (*level)--;
    }
}

void quick_sort(int arr[], int n, int show_steps) {
    if (show_steps) {
        printf("Starting Quick Sort...\n\n");
    }
    int level = 0;
    quick_sort_recursive(arr, 0, n - 1, show_steps, &level);
}

int main(void) {
    printf("=== Quick Sort ===\n\n");
    
    // Example 1: Small array with visualization
    printf("Example 1: Visualizing partition and recursion\n");
    printf("-----------------------------------------------\n");
    int arr1[] = {10, 7, 8, 9, 1, 5};
    int n1 = sizeof(arr1) / sizeof(arr1[0]);
    
    printf("Original: ");
    print_array(arr1, n1);
    printf("\n");
    
    quick_sort(arr1, n1, 1);
    
    printf("\nSorted: ");
    print_array(arr1, n1);
    
    // Example 2: Average case performance
    printf("\n\nExample 2: Performance on random data\n");
    printf("--------------------------------------\n");
    
    int n2 = 10000;
    int *arr2 = (int*)malloc(n2 * sizeof(int));
    
    srand(time(NULL));
    for (int i = 0; i < n2; i++) {
        arr2[i] = rand() % 10000;
    }
    
    printf("Sorting %d random numbers...\n", n2);
    
    clock_t start = clock();
    quick_sort(arr2, n2, 0);
    clock_t end = clock();
    
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Sorted in %.4f seconds\n", time_taken);
    
    printf("First 10: ");
    for (int i = 0; i < 10; i++) {
        printf("%d ", arr2[i]);
    }
    printf("\n");
    
    free(arr2);
    
    // Example 3: Worst case (already sorted)
    printf("\n\nExample 3: Worst case - already sorted\n");
    printf("---------------------------------------\n");
    
    int arr3[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int n3 = sizeof(arr3) / sizeof(arr3[0]);
    
    printf("Already sorted: ");
    print_array(arr3, n3);
    
    start = clock();
    quick_sort(arr3, n3, 0);
    end = clock();
    
    time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Sorted in %.6f seconds\n", time_taken);
    printf("(Slower because poor pivot choice leads to O(n²) behavior)\n");
    
    printf("\n\nPress Enter to exit...");
    getchar();
    return 0;
}

/*
 * How Quick Sort Works:
 * 
 * 1. Pick a pivot element (we use last element)
 * 2. Partition: move elements < pivot left, > pivot right
 * 3. Pivot is now in final sorted position
 * 4. Recursively sort left and right partitions
 * 
 * Example: [10, 7, 8, 9, 1, 5] pivot=5
 * Partition: [1] 5 [10, 7, 8, 9]  (5 in final position!)
 * Left: [1] (sorted)
 * Right: [10, 7, 8, 9] pivot=9
 *   Partition: [7, 8] 9 [10]
 *   Left: [7, 8] pivot=8 -> [7] 8 []
 *   Right: [10] (sorted)
 * Result: [1, 5, 7, 8, 9, 10]
 * 
 * Complexity:
 * - Time: O(n log n) average, O(n²) worst
 * - Space: O(log n) recursion stack
 * - Stable: No
 * 
 * Why average O(n log n)?
 * - log n levels on average (balanced partitions)
 * - O(n) work per level (partitioning)
 * 
 * Worst case O(n²):
 * - Poor pivot (smallest or largest every time)
 * - Already sorted with last element pivot
 * - Creates unbalanced partitions
 * 
 * When to use:
 * - General-purpose sorting
 * - Large datasets
 * - Average case matters
 * - In-place sorting needed
 * - Most common choice!
 * 
 * Advantages:
 * - Very fast in practice
 * - In-place (O(log n) stack)
 * - Cache-friendly
 * - Used in most standard libraries
 * 
 * Disadvantages:
 * - Not stable
 * - Worst case O(n²) (rare)
 * - Recursive (stack overhead)
 * 
 * Improvements:
 * - Random pivot (avoids worst case)
 * - Median-of-three pivot
 * - Hybrid with insertion sort for small partitions
 * - Introsort (switch to heapsort if too deep)
 * 
 * Try:
 * - Implement random pivot selection
 * - Compare to merge sort on same data
 * - Measure worst case on sorted data
 * - Implement iterative version (using stack)
 */
