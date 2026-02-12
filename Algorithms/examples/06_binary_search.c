/*
 * 06_binary_search.c
 * 
 * Binary search - efficient search in sorted arrays.
 * O(log n) performance by eliminating half the search space each step.
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

void print_search_range(int arr[], int left, int right, int mid) {
    printf("  [");
    for (int i = left; i <= right; i++) {
        if (i == mid) {
            printf("|%d| ", arr[i]);
        } else {
            printf("%d ", arr[i]);
        }
    }
    printf("]\n");
}

// Binary search iterative
int binary_search(int arr[], int n, int target, int show_steps) {
    int left = 0;
    int right = n - 1;
    int step = 0;
    
    while (left <= right) {
        step++;
        int mid = left + (right - left) / 2;  // Prevent overflow
        
        if (show_steps) {
            printf("Step %d: ", step);
            print_search_range(arr, left, right, mid);
            printf("        Checking arr[%d]=%d vs target=%d\n", mid, arr[mid], target);
        }
        
        if (arr[mid] == target) {
            if (show_steps) {
                printf("        FOUND at index %d!\n", mid);
            }
            return mid;
        }
        
        if (arr[mid] < target) {
            if (show_steps) {
                printf("        Target > mid, search right half\n\n");
            }
            left = mid + 1;
        } else {
            if (show_steps) {
                printf("        Target < mid, search left half\n\n");
            }
            right = mid - 1;
        }
    }
    
    if (show_steps) {
        printf("        NOT FOUND\n");
    }
    return -1;
}

// Binary search recursive
int binary_search_recursive(int arr[], int left, int right, int target) {
    if (left > right) return -1;
    
    int mid = left + (right - left) / 2;
    
    if (arr[mid] == target) return mid;
    if (arr[mid] < target) return binary_search_recursive(arr, mid + 1, right, target);
    return binary_search_recursive(arr, left, mid - 1, target);
}

// Find first occurrence of target
int binary_search_first(int arr[], int n, int target) {
    int left = 0, right = n - 1;
    int result = -1;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        
        if (arr[mid] == target) {
            result = mid;
            right = mid - 1;  // Keep searching left
        } else if (arr[mid] < target) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    
    return result;
}

// Linear search for comparison
int linear_search(int arr[], int n, int target) {
    for (int i = 0; i < n; i++) {
        if (arr[i] == target) return i;
    }
    return -1;
}

int main(void) {
    printf("=== Binary Search ===\n\n");
    
    // Example 1: Visualize the search process
    printf("Example 1: Searching with visualization\n");
    printf("----------------------------------------\n");
    int arr1[] = {2, 5, 8, 12, 16, 23, 38, 45, 56, 67, 78};
    int n1 = sizeof(arr1) / sizeof(arr1[0]);
    
    printf("Array: ");
    print_array(arr1, n1);
    printf("\nSearching for 23:\n");
    int result = binary_search(arr1, n1, 23, 1);
    printf("\n");
    
    printf("Searching for 50 (not in array):\n");
    binary_search(arr1, n1, 50, 1);
    
    // Example 2: Binary vs Linear search speed
    printf("\n\nExample 2: Binary vs Linear search (100,000 elements)\n");
    printf("------------------------------------------------------\n");
    
    int n2 = 100000;
    int *arr2 = (int*)malloc(n2 * sizeof(int));
    
    // Create sorted array
    for (int i = 0; i < n2; i++) {
        arr2[i] = i * 2;  // Even numbers
    }
    
    int target = 123456;  // Search for this
    
    // Binary search
    clock_t start = clock();
    int pos1 = binary_search(arr2, n2, target, 0);
    clock_t end = clock();
    double time_binary = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    // Linear search
    start = clock();
    int pos2 = linear_search(arr2, n2, target);
    end = clock();
    double time_linear = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("Searching for %d in %d elements:\n", target, n2);
    printf("  Binary search: %.6f seconds (found at %d)\n", time_binary, pos1);
    printf("  Linear search: %.6f seconds (found at %d)\n", time_linear, pos2);
    printf("  Binary is %.0fx faster!\n", time_linear / time_binary);
    
    free(arr2);
    
    // Example 3: Finding duplicates
    printf("\n\nExample 3: Finding duplicates\n");
    printf("------------------------------\n");
    int arr3[] = {1, 2, 3, 3, 3, 4, 5, 6, 7, 8};
    int n3 = sizeof(arr3) / sizeof(arr3[0]);
    
    printf("Array with duplicates: ");
    print_array(arr3, n3);
    
    int any_pos = binary_search(arr3, n3, 3, 0);
    int first_pos = binary_search_first(arr3, n3, 3);
    
    printf("Any occurrence of 3: index %d\n", any_pos);
    printf("First occurrence of 3: index %d\n", first_pos);
    
    // Example 4: Recursive vs Iterative
    printf("\n\nExample 4: Recursive vs Iterative\n");
    printf("----------------------------------\n");
    int arr4[] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19};
    int n4 = sizeof(arr4) / sizeof(arr4[0]);
    
    printf("Array: ");
    print_array(arr4, n4);
    
    int target4 = 13;
    int pos_iter = binary_search(arr4, n4, target4, 0);
    int pos_rec = binary_search_recursive(arr4, 0, n4 - 1, target4);
    
    printf("Searching for %d:\n", target4);
    printf("  Iterative: found at index %d\n", pos_iter);
    printf("  Recursive: found at index %d\n", pos_rec);
    printf("  (Both give same result!)\n");
    
    printf("\n\nPress Enter to exit...");
    getchar();
    return 0;
}

/*
 * How Binary Search Works:
 * 
 * 1. Find middle element
 * 2. If target == middle: found!
 * 3. If target < middle: search left half
 * 4. If target > middle: search right half
 * 5. Repeat until found or range empty
 * 
 * Example: Search for 23 in [2, 5, 8, 12, 16, 23, 38, 45, 56, 67, 78]
 * Step 1: Check middle (23) -> FOUND!
 * 
 * Example: Search for 50
 * Step 1: Check middle (23) -> 50 > 23, go right
 * Step 2: Check middle (56) -> 50 < 56, go left
 * Step 3: Check middle (45) -> 50 > 45, go right
 * Step 4: Empty range -> NOT FOUND
 * 
 * Complexity:
 * - Time: O(log n) - halves search space each step
 * - Space: O(1) iterative, O(log n) recursive
 * 
 * Why O(log n)?
 * - Each step eliminates half the elements
 * - n -> n/2 -> n/4 -> n/8 -> ... -> 1
 * - Number of steps: log₂(n)
 * 
 * For n=1,000,000: only ~20 steps!
 * 
 * Requirements:
 * - Array MUST be sorted
 * - Need random access (arrays, not linked lists)
 * 
 * When to use:
 * - Searching sorted data
 * - Large datasets
 * - Fast lookups needed
 * - Most common search algorithm
 * 
 * Advantages:
 * - Very fast - O(log n)
 * - Simple to implement
 * - No extra space (iterative)
 * 
 * Disadvantages:
 * - Requires sorted data
 * - Needs random access
 * - Sort cost: O(n log n)
 * 
 * When NOT to use:
 * - Unsorted data (use linear search or hash table)
 * - Single search (cost of sorting > cost of linear search)
 * - Linked lists (no random access)
 * 
 * Try:
 * - Search for first and last elements
 * - Search for missing element
 * - Compare time vs linear search on different sizes
 * - Implement with different pivot strategies
 */
