# Big O Notation

Understanding algorithm efficiency.

## The Big Picture

Big O notation describes how an algorithm's runtime grows as the input size increases. It ignores constants and focuses on the growth rate.

Think of it as asking: "If I double the input size, how much slower does it get?"

## Common Complexities

From fastest to slowest:

### O(1) - Constant Time
**Runtime doesn't depend on input size.**

```c
int get_first(int arr[], int n) {
    return arr[0];  // Always one operation
}
```

Examples: Array access, hash table lookup

### O(log n) - Logarithmic Time
**Runtime doubles when input size is squared.**

```c
// Binary search - each step eliminates half
int binary_search(int arr[], int n, int target) {
    int left = 0, right = n - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (arr[mid] == target) return mid;
        if (arr[mid] < target) left = mid + 1;
        else right = mid - 1;
    }
    return -1;
}
```

For n=1000: ~10 steps  
For n=1,000,000: ~20 steps  

Examples: Binary search, balanced tree operations

### O(n) - Linear Time
**Runtime grows directly with input size.**

```c
// Must check every element
int find_max(int arr[], int n) {
    int max = arr[0];
    for (int i = 1; i < n; i++) {
        if (arr[i] > max) max = arr[i];
    }
    return max;
}
```

For n=1000: 1000 steps  
For n=1,000,000: 1,000,000 steps  

Examples: Linear search, finding max/min, array traversal

### O(n log n) - Linearithmic Time
**Common for efficient sorting.**

```c
// Merge sort, quick sort
void merge_sort(int arr[], int n) {
    // Divide array in half (log n levels)
    // Merge at each level (n work per level)
    // Total: n * log n
}
```

For n=1000: ~10,000 steps  
For n=1,000,000: ~20,000,000 steps  

Examples: Merge sort, quick sort (average), heap sort

### O(n²) - Quadratic Time
**Nested loops over the data.**

```c
// Bubble sort - compare every pair
void bubble_sort(int arr[], int n) {
    for (int i = 0; i < n; i++) {        // n times
        for (int j = 0; j < n - 1; j++) { // n times
            if (arr[j] > arr[j + 1]) {
                swap(&arr[j], &arr[j + 1]);
            }
        }
    }
}
```

For n=1000: 1,000,000 steps  
For n=1,000,000: 1,000,000,000,000 steps (ouch!)  

Examples: Bubble sort, selection sort, insertion sort

### O(2ⁿ) - Exponential Time
**Runtime doubles with each additional input.**

```c
// Naive Fibonacci - recalculates same values
int fib(int n) {
    if (n <= 1) return n;
    return fib(n - 1) + fib(n - 2);  // Two recursive calls
}
```

For n=10: ~1,000 steps  
For n=20: ~1,000,000 steps  
For n=30: ~1,000,000,000 steps  

Examples: Generating all subsets, naive recursive solutions

### O(n!) - Factorial Time
**Every permutation of input.**

```c
// Generate all permutations
void permute(int arr[], int n) {
    // n! different orderings
}
```

For n=5: 120 permutations  
For n=10: 3,628,800 permutations  
For n=12: 479,001,600 permutations  

Examples: Brute force traveling salesman, generating all permutations

## Visual Comparison

```
Operations for different input sizes:

         n=10    n=100   n=1000
O(1)     1       1       1
O(log n) 3       7       10
O(n)     10      100     1,000
O(n²)    100     10,000  1,000,000
O(2ⁿ)    1,024   ∞       ∞
```

## Rules for Calculating Big O

### 1. Drop Constants

```c
// Both are O(n), not O(2n) or O(n/2)
for (int i = 0; i < n; i++) { }      // O(n)
for (int i = 0; i < n; i += 2) { }   // Still O(n)
```

### 2. Drop Non-Dominant Terms

```c
// O(n² + n) becomes O(n²)
for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) { }  // n²
}
for (int i = 0; i < n; i++) { }      // + n
// Result: O(n²)
```

### 3. Different Inputs = Different Variables

```c
// O(a + b), NOT O(n)
void process(int a[], int a_len, int b[], int b_len) {
    for (int i = 0; i < a_len; i++) { }  // O(a)
    for (int i = 0; i < b_len; i++) { }  // O(b)
}
```

### 4. Nested Loops = Multiply

```c
// O(n * m)
for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) { }
}
```

## Space Complexity

Big O also describes memory usage:

```c
// O(1) space - fixed amount
void reverse(int arr[], int n) {
    int temp;  // Only one extra variable
    // ...
}

// O(n) space - array copy
int* copy_array(int arr[], int n) {
    int *copy = malloc(n * sizeof(int));  // n extra memory
    return copy;
}

// O(n) space - recursion depth
int factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);  // n stack frames
}
```

## Best, Average, Worst Case

Algorithms can have different performance:

**Quick Sort:**
- Best case: O(n log n) - perfect pivots
- Average case: O(n log n) - random data
- Worst case: O(n²) - already sorted

**Binary Search:**
- Best case: O(1) - found immediately
- Average case: O(log n)
- Worst case: O(log n) - search entire tree

We usually care about **worst case** (guaranteed performance) and **average case** (typical performance).

## Practical Examples

### Example 1: Find Duplicates

```c
// O(n²) - nested loops
bool has_duplicates_slow(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (arr[i] == arr[j]) return true;
        }
    }
    return false;
}

// O(n log n) - sort first
bool has_duplicates_faster(int arr[], int n) {
    qsort(arr, n, sizeof(int), compare);  // O(n log n)
    for (int i = 0; i < n - 1; i++) {     // O(n)
        if (arr[i] == arr[i + 1]) return true;
    }
    return false;
}
```

### Example 2: Is Palindrome

```c
// O(n) - single pass
bool is_palindrome(char *str) {
    int len = strlen(str);           // O(n)
    for (int i = 0; i < len / 2; i++) {  // O(n/2) = O(n)
        if (str[i] != str[len - 1 - i]) {
            return false;
        }
    }
    return true;
}
```

### Example 3: Sum of Pairs

```c
// O(n²) - check all pairs
bool has_pair_sum_slow(int arr[], int n, int target) {
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (arr[i] + arr[j] == target) return true;
        }
    }
    return false;
}

// O(n) - with hash set (not shown, requires extra space)
// Trade space for time!
```

## When Does Big O Matter?

**Small data (n < 100):**
- Big O doesn't matter much
- Simple algorithms (bubble sort) work fine
- Focus on code clarity

**Medium data (n = 1,000 - 10,000):**
- O(n²) starts to hurt
- Use O(n log n) algorithms
- Notice performance difference

**Large data (n > 100,000):**
- Big O is critical
- O(n²) is unusable
- O(n) vs O(n log n) matters

**Very large data (n > 1,000,000):**
- Every operation counts
- Cache locality matters
- Consider parallel algorithms

## Summary Table

| Notation | Name | Example | When n doubles... |
|----------|------|---------|-------------------|
| O(1) | Constant | Array access | No change |
| O(log n) | Logarithmic | Binary search | Add one step |
| O(n) | Linear | Linear search | Double time |
| O(n log n) | Linearithmic | Merge sort | ~2x time |
| O(n²) | Quadratic | Bubble sort | 4x time |
| O(2ⁿ) | Exponential | Subsets | Square time |
| O(n!) | Factorial | Permutations | Impossible |

## Key Takeaways

1. **Big O describes growth rate**, not exact time
2. **Drop constants** - O(2n) is just O(n)
3. **Drop smaller terms** - O(n² + n) is O(n²)
4. **Nested loops multiply** - two nested n loops = O(n²)
5. **Logarithmic is very fast** - even for huge inputs
6. **Exponential is very slow** - avoid if possible
7. **Space complexity matters too** - memory isn't free
8. **Trade-offs exist** - often space for time

Understanding Big O helps you:
- Choose the right algorithm
- Predict performance at scale
- Optimize bottlenecks
- Make informed design decisions

Start recognizing patterns - loops, recursion depth, data structure operations. Big O will become intuitive!
