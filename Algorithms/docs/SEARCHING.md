# Searching Algorithms

Finding elements efficiently in collections.

## The Big Picture

Searching is finding a specific element in a collection. The efficiency depends on whether the data is sorted and what data structure you're using.

## Linear Search

**Idea:** Check every element until you find it.

```c
int linear_search(int arr[], int n, int target) {
    for (int i = 0; i < n; i++) {
        if (arr[i] == target) {
            return i;  // Found at index i
        }
    }
    return -1;  // Not found
}
```

**How it works:**
1. Start at beginning
2. Check each element
3. Return index if found
4. Return -1 if not found

**Complexity:**
- Time: O(n) - might check every element
- Space: O(1)

**When to use:**
- Unsorted data
- Small datasets
- Data structures without random access (linked lists)

**Pros:**
- Works on unsorted data
- Simple
- Works on any data structure

**Cons:**
- Slow for large datasets
- Doesn't leverage sorted data

## Binary Search

**Idea:** Divide and conquer on sorted data.

```c
int binary_search(int arr[], int n, int target) {
    int left = 0, right = n - 1;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        
        if (arr[mid] == target) {
            return mid;  // Found
        }
        
        if (arr[mid] < target) {
            left = mid + 1;  // Search right half
        } else {
            right = mid - 1; // Search left half
        }
    }
    
    return -1;  // Not found
}
```

**How it works:**
1. Find middle element
2. If target == middle, done!
3. If target < middle, search left half
4. If target > middle, search right half
5. Repeat until found or range is empty

**Visual:**
```
Search for 7 in [1, 3, 5, 7, 9, 11, 13]:

Step 1: [1, 3, 5, |7|, 9, 11, 13]  mid=7 → Found!

Search for 6:
Step 1: [1, 3, 5, |7|, 9, 11, 13]  mid=7 > 6 → go left
Step 2: [1, |3|, 5]  mid=3 < 6 → go right
Step 3: [5]  mid=5 < 6 → go right
Step 4: Empty range → Not found
```

**Complexity:**
- Time: O(log n) - halves search space each step
- Space: O(1) iterative, O(log n) recursive

**When to use:**
- Sorted data
- Large datasets
- Fast lookups needed

**Pros:**
- Very fast - O(log n)
- Efficient for large data

**Cons:**
- **Requires sorted data**
- Needs random access (arrays)

## Jump Search

**Idea:** Jump ahead by fixed steps, then linear search.

```c
int jump_search(int arr[], int n, int target) {
    int step = sqrt(n);
    int prev = 0;
    
    // Jump until we pass target
    while (arr[min(step, n) - 1] < target) {
        prev = step;
        step += sqrt(n);
        if (prev >= n) return -1;
    }
    
    // Linear search in block
    while (arr[prev] < target) {
        prev++;
        if (prev == min(step, n)) return -1;
    }
    
    if (arr[prev] == target) return prev;
    return -1;
}
```

**How it works:**
1. Jump by √n steps
2. When you overshoot, go back
3. Linear search in that block

**Visual:**
```
Search for 7 in [1, 2, 3, 4, 5, 6, 7, 8, 9], step=3:

Jump: [1, 2, 3] → 3 < 7, continue
Jump: [4, 5, 6] → 6 < 7, continue
Jump: [7, 8, 9] → 7 >= 7, found block!
Linear search: 7 found!
```

**Complexity:**
- Time: O(√n)
- Space: O(1)

**When to use:**
- Sorted data
- Between linear and binary
- When jumping ahead is cheaper than comparisons

**Pros:**
- Faster than linear
- Better cache locality than binary search
- Good for systems where jumping back is expensive

**Cons:**
- Slower than binary search
- Still requires sorted data

## Comparison Table

| Algorithm | Time | Space | Requires Sorted | Random Access |
|-----------|------|-------|-----------------|---------------|
| Linear | O(n) | O(1) | No | No |
| Binary | O(log n) | O(1) | Yes | Yes |
| Jump | O(√n) | O(1) | Yes | Yes |

## Binary Search Variants

### Find First Occurrence

```c
// Find leftmost position of target
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
```

### Find Last Occurrence

```c
// Find rightmost position of target
int binary_search_last(int arr[], int n, int target) {
    int left = 0, right = n - 1;
    int result = -1;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        
        if (arr[mid] == target) {
            result = mid;
            left = mid + 1;  // Keep searching right
        } else if (arr[mid] < target) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    
    return result;
}
```

### Find Insertion Point

```c
// Find where to insert target to keep array sorted
int binary_search_insert(int arr[], int n, int target) {
    int left = 0, right = n;
    
    while (left < right) {
        int mid = left + (right - left) / 2;
        
        if (arr[mid] < target) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    
    return left;
}
```

## Binary Search on Answer Space

Binary search isn't just for arrays - it works on any monotonic function!

**Example: Find square root**

```c
double sqrt_binary(double x) {
    double left = 0, right = x;
    double epsilon = 0.00001;
    
    while (right - left > epsilon) {
        double mid = (left + right) / 2;
        double square = mid * mid;
        
        if (square < x) {
            left = mid;
        } else {
            right = mid;
        }
    }
    
    return (left + right) / 2;
}
```

## Common Mistakes

### Off-by-One Errors

```c
// WRONG - can overflow
int mid = (left + right) / 2;

// RIGHT - prevents overflow
int mid = left + (right - left) / 2;
```

### Infinite Loops

```c
// WRONG - can infinite loop
while (left < right) {
    int mid = (left + right) / 2;
    if (arr[mid] < target) {
        left = mid;  // Doesn't move if mid == left
    }
}

// RIGHT
while (left < right) {
    int mid = (left + right) / 2;
    if (arr[mid] < target) {
        left = mid + 1;  // Always progresses
    }
}
```

### Wrong Bounds

```c
// For finding first occurrence:
if (arr[mid] == target) {
    result = mid;
    right = mid - 1;  // Keep searching left!
}
```

## When to Use Each

**Linear Search:**
- Unsorted data
- Small datasets (< 100 elements)
- Linked lists
- First occurrence in unsorted data

**Binary Search:**
- Sorted arrays
- Large datasets
- Need O(log n) lookups
- Most common for sorted data

**Jump Search:**
- Sorted data with expensive random access
- Systems where jumping forward is cheap
- Between linear and binary performance

## Practical Tips

1. **Sort once, search many** - if you'll search repeatedly, sort first
2. **Binary search is tricky** - test edge cases
3. **Use library functions** - bsearch() in C
4. **Consider hash tables** - O(1) average lookup
5. **Profile** - don't assume, measure

## Try This

Implement each search and test with:
- Element at start
- Element at end
- Element in middle
- Element not present
- Array with duplicates
- Empty array

Compare actual runtimes for different data sizes!
