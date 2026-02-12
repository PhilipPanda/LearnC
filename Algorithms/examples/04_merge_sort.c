/*
 * 04_merge_sort.c
 * 
 * Merge sort - divide and conquer sorting algorithm.
 * Guaranteed O(n log n) performance. Stable and predictable.
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

// Merge two sorted subarrays
void merge(int arr[], int left, int mid, int right, int show_steps) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    
    // Create temp arrays
    int *L = (int*)malloc(n1 * sizeof(int));
    int *R = (int*)malloc(n2 * sizeof(int));
    
    // Copy data
    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int i = 0; i < n2; i++) R[i] = arr[mid + 1 + i];
    
    if (show_steps) {
        printf("    Merging: [");
        for (int i = 0; i < n1; i++) printf("%d%s", L[i], i < n1-1 ? ", " : "");
        printf("] + [");
        for (int i = 0; i < n2; i++) printf("%d%s", R[i], i < n2-1 ? ", " : "");
        printf("]\n");
    }
    
    // Merge back into arr[]
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k++] = L[i++];
        } else {
            arr[k++] = R[j++];
        }
    }
    
    // Copy remaining
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
    
    if (show_steps) {
        printf("    Result:  [");
        for (int i = left; i <= right; i++) {
            printf("%d%s", arr[i], i < right ? ", " : "");
        }
        printf("]\n\n");
    }
    
    free(L);
    free(R);
}

// Recursive merge sort
void merge_sort_recursive(int arr[], int left, int right, int show_steps, int *level) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        
        if (show_steps) {
            (*level)++;
            printf("  Level %d: Split [", *level);
            for (int i = left; i <= right; i++) {
                printf("%d%s", arr[i], i < right ? ", " : "");
            }
            printf("]\n");
        }
        
        // Sort first and second halves
        merge_sort_recursive(arr, left, mid, show_steps, level);
        merge_sort_recursive(arr, mid + 1, right, show_steps, level);
        
        // Merge the sorted halves
        merge(arr, left, mid, right, show_steps);
        
        if (show_steps) (*level)--;
    }
}

void merge_sort(int arr[], int n, int show_steps) {
    if (show_steps) {
        printf("Starting Merge Sort (Divide and Conquer)...\n\n");
    }
    int level = 0;
    merge_sort_recursive(arr, 0, n - 1, show_steps, &level);
}

int main(void) {
    printf("=== Merge Sort ===\n\n");
    
    // Example 1: Small array with visualization
    printf("Example 1: Visualizing the divide-and-conquer process\n");
    printf("------------------------------------------------------\n");
    int arr1[] = {38, 27, 43, 3, 9, 82, 10};
    int n1 = sizeof(arr1) / sizeof(arr1[0]);
    
    printf("Original: ");
    print_array(arr1, n1);
    printf("\n");
    
    merge_sort(arr1, n1, 1);
    
    printf("Sorted: ");
    print_array(arr1, n1);
    
    // Example 2: Best vs worst case (same performance!)
    printf("\n\nExample 2: Guaranteed O(n log n) - always!\n");
    printf("-------------------------------------------\n");
    
    int arr2a[] = {1, 2, 3, 4, 5, 6, 7, 8};  // Already sorted
    int arr2b[] = {8, 7, 6, 5, 4, 3, 2, 1};  // Reverse sorted
    int n2 = 8;
    
    printf("Already sorted: ");
    print_array(arr2a, n2);
    clock_t start = clock();
    merge_sort(arr2a, n2, 0);
    clock_t end = clock();
    double time1 = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Time: %.6f seconds\n\n", time1);
    
    printf("Reverse sorted: ");
    print_array(arr2b, n2);
    start = clock();
    merge_sort(arr2b, n2, 0);
    end = clock();
    double time2 = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Time: %.6f seconds\n", time2);
    printf("(Both take similar time - guaranteed O(n log n)!)\n");
    
    // Example 3: Performance on larger data
    printf("\n\nExample 3: Sorting 10,000 elements\n");
    printf("-----------------------------------\n");
    
    int n3 = 10000;
    int *arr3 = (int*)malloc(n3 * sizeof(int));
    
    srand(time(NULL));
    for (int i = 0; i < n3; i++) {
        arr3[i] = rand() % 10000;
    }
    
    printf("Sorting %d random numbers...\n", n3);
    
    start = clock();
    merge_sort(arr3, n3, 0);
    end = clock();
    
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Sorted in %.4f seconds\n", time_taken);
    
    // Verify sorted
    int is_sorted = 1;
    for (int i = 0; i < n3 - 1; i++) {
        if (arr3[i] > arr3[i + 1]) {
            is_sorted = 0;
            break;
        }
    }
    printf("Verification: %s\n", is_sorted ? "SORTED ✓" : "NOT SORTED ✗");
    
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
 * How Merge Sort Works:
 * 
 * Divide and Conquer:
 * 1. Divide array in half
 * 2. Recursively sort each half
 * 3. Merge the two sorted halves
 * 
 * Example: [38, 27, 43, 3]
 * Split: [38, 27] [43, 3]
 * Split: [38] [27] [43] [3]
 * Merge: [27, 38] [3, 43]
 * Merge: [3, 27, 38, 43]
 * 
 * Complexity:
 * - Time: O(n log n) - ALWAYS! (best, average, worst)
 * - Space: O(n) - needs temporary arrays
 * - Stable: Yes
 * 
 * Why O(n log n)?
 * - log n levels (divide in half each time)
 * - O(n) work per level (merging)
 * - Total: n * log n
 * 
 * When to use:
 * - Need guaranteed O(n log n) performance
 * - Stability required
 * - External sorting (data doesn't fit in memory)
 * - Linked lists (doesn't need random access)
 * 
 * Advantages:
 * - Predictable performance
 * - Stable
 * - Good for external sorts
 * - Parallelizes well
 * 
 * Disadvantages:
 * - Needs O(n) extra space
 * - Slower than quicksort in practice (more copying)
 * - Not in-place
 * 
 * Used in:
 * - Java's Arrays.sort() for objects
 * - Python's sort() (Timsort is based on merge sort)
 * - External sorting algorithms
 * 
 * Try:
 * - Compare to quicksort on same data
 * - Measure actual memory usage
 * - Implement bottom-up (iterative) version
 * - Try on linked list
 */
