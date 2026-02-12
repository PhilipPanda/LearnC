# Recursion

Functions calling themselves to solve problems.

## The Big Picture

Recursion is when a function calls itself. It's a different way of thinking - instead of loops, you break a problem into smaller versions of itself.

Every recursive function needs:
1. **Base case** - when to stop
2. **Recursive case** - problem broken into smaller problem
3. **Progress toward base case** - eventually reaches base case

## Your First Recursive Function

### Factorial

```c
// n! = n × (n-1) × (n-2) × ... × 1
int factorial(int n) {
    // Base case: factorial of 0 or 1 is 1
    if (n <= 1) {
        return 1;
    }
    
    // Recursive case: n! = n × (n-1)!
    return n * factorial(n - 1);
}
```

**How it works:**
```
factorial(5):
  5 * factorial(4):
      4 * factorial(3):
          3 * factorial(2):
              2 * factorial(1):
                  return 1
              return 2 * 1 = 2
          return 3 * 2 = 6
      return 4 * 6 = 24
  return 5 * 24 = 120
```

**Stack visualization:**
```
factorial(5)
  factorial(4)
    factorial(3)
      factorial(2)
        factorial(1) → 1
      ← 2
    ← 6
  ← 24
← 120
```

## Common Recursive Patterns

### 1. Linear Recursion

One recursive call per function call.

```c
// Sum of array
int sum(int arr[], int n) {
    if (n == 0) return 0;
    return arr[n-1] + sum(arr, n-1);
}

// Count down
void countdown(int n) {
    if (n == 0) {
        printf("Blastoff!\n");
        return;
    }
    printf("%d\n", n);
    countdown(n - 1);
}
```

### 2. Binary Recursion

Two recursive calls per function call.

```c
// Fibonacci
int fib(int n) {
    if (n <= 1) return n;
    return fib(n-1) + fib(n-2);  // Two calls!
}

// Binary tree operations
void print_tree(TreeNode* node) {
    if (node == NULL) return;
    printf("%d ", node->value);
    print_tree(node->left);   // First recursive call
    print_tree(node->right);  // Second recursive call
}
```

### 3. Tail Recursion

Recursive call is the last operation.

```c
// Tail recursive factorial
int factorial_tail(int n, int accumulator) {
    if (n <= 1) return accumulator;
    return factorial_tail(n - 1, n * accumulator);
}

// Can be optimized to loop by compiler
int factorial_loop(int n) {
    int acc = 1;
    while (n > 1) {
        acc *= n;
        n--;
    }
    return acc;
}
```

## Recursion vs Iteration

Every recursive solution can be written iteratively (with a loop).

### Factorial

**Recursive:**
```c
int factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}
```

**Iterative:**
```c
int factorial(int n) {
    int result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}
```

### When to Use Recursion

**Use recursion when:**
- Problem naturally recursive (trees, divide-and-conquer)
- Code is clearer recursively
- Depth is limited (won't overflow stack)

**Use iteration when:**
- Simple loop is clearer
- Deep recursion (stack overflow risk)
- Performance critical (recursion has overhead)

## Classic Recursive Problems

### Binary Search

```c
int binary_search_recursive(int arr[], int left, int right, int target) {
    if (left > right) return -1;  // Base case
    
    int mid = left + (right - left) / 2;
    
    if (arr[mid] == target) return mid;
    
    if (arr[mid] < target)
        return binary_search_recursive(arr, mid + 1, right, target);
    else
        return binary_search_recursive(arr, left, mid - 1, target);
}
```

### Power Function

```c
// x^n efficiently
int power(int x, int n) {
    if (n == 0) return 1;
    if (n == 1) return x;
    
    // Divide and conquer: x^8 = (x^4)^2
    int half = power(x, n / 2);
    if (n % 2 == 0) {
        return half * half;
    } else {
        return x * half * half;
    }
}
```

### String Reversal

```c
void reverse_recursive(char* str, int start, int end) {
    if (start >= end) return;  // Base case
    
    // Swap ends
    char temp = str[start];
    str[start] = str[end];
    str[end] = temp;
    
    // Recurse on middle
    reverse_recursive(str, start + 1, end - 1);
}
```

### Palindrome Check

```c
bool is_palindrome(char* str, int start, int end) {
    if (start >= end) return true;  // Base case
    
    if (str[start] != str[end]) {
        return false;
    }
    
    return is_palindrome(str, start + 1, end - 1);
}
```

## Tree Recursion

Trees are naturally recursive - each node is a tree!

```c
typedef struct TreeNode {
    int value;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

// Count nodes
int count_nodes(TreeNode* root) {
    if (root == NULL) return 0;
    return 1 + count_nodes(root->left) + count_nodes(root->right);
}

// Find maximum
int tree_max(TreeNode* root) {
    if (root == NULL) return INT_MIN;
    
    int left_max = tree_max(root->left);
    int right_max = tree_max(root->right);
    
    int max = root->value;
    if (left_max > max) max = left_max;
    if (right_max > max) max = right_max;
    
    return max;
}

// Height of tree
int height(TreeNode* root) {
    if (root == NULL) return 0;
    
    int left_height = height(root->left);
    int right_height = height(root->right);
    
    return 1 + (left_height > right_height ? left_height : right_height);
}
```

## Common Mistakes

### Missing Base Case

```c
// INFINITE RECURSION!
int bad_factorial(int n) {
    return n * bad_factorial(n - 1);  // Never stops!
}
```

### Not Making Progress

```c
// INFINITE RECURSION!
int bad_countdown(int n) {
    if (n == 0) return;
    printf("%d\n", n);
    countdown(n);  // Should be n-1!
}
```

### Overlapping Subproblems

```c
// VERY SLOW - recalculates same values
int fib(int n) {
    if (n <= 1) return n;
    return fib(n-1) + fib(n-2);
}

// fib(5) calls:
//   fib(4) + fib(3)
//   (fib(3) + fib(2)) + (fib(2) + fib(1))
//   fib(2) calculated 3 times!
```

**Solution: Memoization (cache results)**

```c
int fib_memo(int n, int* memo) {
    if (n <= 1) return n;
    if (memo[n] != -1) return memo[n];  // Already calculated
    
    memo[n] = fib_memo(n-1, memo) + fib_memo(n-2, memo);
    return memo[n];
}
```

## Stack Overflow

Each recursive call uses stack space. Too many → stack overflow!

```c
// Dangerous for large n
int factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

factorial(1000000);  // CRASH - stack overflow
```

**Solution:** Use iteration or tail recursion (if compiler optimizes).

## Thinking Recursively

1. **Identify the base case** - simplest version
2. **Assume it works for smaller n** - leap of faith
3. **Build solution from smaller solution**

**Example: Reverse a list**

1. Base: Empty or one element → already reversed
2. Assume: Can reverse rest of list
3. Build: Reverse rest, put first element at end

```c
void reverse_list(Node* head) {
    if (head == NULL || head->next == NULL) return;
    
    reverse_list(head->next);  // Assume this works!
    head->next->next = head;
    head->next = NULL;
}
```

## Recursion Patterns

### Decrease and Conquer
Reduce problem size by constant each call.
```c
int factorial(int n) { return n * factorial(n - 1); }
```

### Divide and Conquer
Split problem in half each call.
```c
int binary_search(arr, left, right) {
    int mid = (left + right) / 2;
    search(arr, left, mid) or search(arr, mid+1, right);
}
```

### Multiple Recursion
Make multiple recursive calls.
```c
int fib(int n) { return fib(n-1) + fib(n-2); }
```

## Summary

**Recursion:**
- Function calls itself
- Needs base case and recursive case
- Natural for tree-like problems
- Can be clearer than loops
- Has overhead (stack space, function calls)

**When to use:**
- Trees and graphs
- Divide and conquer
- Backtracking
- When it makes code clearer

**When not to use:**
- Simple loops are clearer
- Deep recursion (stack overflow)
- Performance critical hot paths
- Overlapping subproblems without memoization

**Practice:** Take iterative code and convert to recursive. Take recursive code and convert to iterative. Both teach you a lot!
