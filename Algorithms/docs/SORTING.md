# Sorting Algorithms

How different sorting algorithms work and when to use them.

## The Big Picture

Sorting is arranging data in a specific order (ascending/descending). It's fundamental because many algorithms work better on sorted data (like binary search).

All sorting algorithms solve the same problem - just with different strategies and trade-offs.

## Bubble Sort

**Idea:** Repeatedly swap adjacent elements if they're in wrong order.

```c
void bubble_sort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(&arr[j], &arr[j + 1]);
            }
        }
    }
}
```

**How it works:**
1. Compare adjacent pairs
2. Swap if out of order
3. Largest element "bubbles" to the end
4. Repeat for remaining elements

**Visual:**
```
[5, 2, 8, 1] -> Compare 5,2 -> Swap
[2, 5, 8, 1] -> Compare 5,8 -> OK
[2, 5, 8, 1] -> Compare 8,1 -> Swap
[2, 5, 1, 8] -> 8 is in place!
[2, 1, 5, 8] -> Next pass...
[1, 2, 5, 8] -> Sorted!
```

**Complexity:**
- Time: O(n²) - nested loops
- Space: O(1) - in-place
- Stable: Yes - equal elements keep order

**When to use:**
- Small datasets (< 20 elements)
- Educational purposes
- Nearly sorted data (can optimize with early exit)

**Pros:**
- Simple to understand
- In-place (no extra memory)

**Cons:**
- Very slow for large data
- Many unnecessary comparisons

## Selection Sort

**Idea:** Find minimum element, swap it to front, repeat for rest.

```c
void selection_sort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < n; j++) {
            if (arr[j] < arr[min_idx]) {
                min_idx = j;
            }
        }
        swap(&arr[i], &arr[min_idx]);
    }
}
```

**How it works:**
1. Find smallest element in unsorted portion
2. Swap it to the beginning
3. Move boundary between sorted/unsorted
4. Repeat

**Visual:**
```
[5, 2, 8, 1] -> Find min (1), swap with first
[1, 2, 8, 5] -> Find min in rest (2), already in place
[1, 2, 8, 5] -> Find min in rest (5), swap
[1, 2, 5, 8] -> Sorted!
```

**Complexity:**
- Time: O(n²) - always
- Space: O(1) - in-place
- Stable: No

**When to use:**
- Minimize number of swaps (each element moved at most once)
- Small datasets
- Memory writes are expensive

**Pros:**
- Simple
- Few swaps
- Works well with expensive swap operations

**Cons:**
- Slow for large data
- Not stable

## Insertion Sort

**Idea:** Build sorted array one element at a time, inserting each into correct position.

```c
void insertion_sort(int arr[], int n) {
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;
        
        // Shift elements greater than key to right
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}
```

**How it works:**
1. Start with first element (considered sorted)
2. Take next element
3. Find its correct position in sorted portion
4. Shift elements and insert
5. Repeat

**Visual:**
```
[5, 2, 8, 1]
[5 | 2, 8, 1] -> Insert 2 into [5]
[2, 5 | 8, 1] -> Insert 8 into [2,5]
[2, 5, 8 | 1] -> Insert 1 into [2,5,8]
[1, 2, 5, 8] -> Sorted!
```

**Complexity:**
- Time: O(n²) worst, O(n) best (already sorted)
- Space: O(1) - in-place
- Stable: Yes

**When to use:**
- Small datasets
- Nearly sorted data (very fast!)
- Online sorting (data arrives over time)
- Often used in hybrid sorts (Timsort, Introsort)

**Pros:**
- Fast for small/nearly sorted data
- Stable
- Online algorithm
- Adaptive (faster if partially sorted)

**Cons:**
- Slow for large random data
- Many shifts

## Merge Sort

**Idea:** Divide array in half recursively, sort halves, merge them.

```c
void merge_sort(int arr[], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        
        merge_sort(arr, left, mid);      // Sort left half
        merge_sort(arr, mid + 1, right); // Sort right half
        merge(arr, left, mid, right);    // Merge sorted halves
    }
}
```

**How it works:**
1. Divide array into two halves
2. Recursively sort each half
3. Merge the sorted halves

**Visual:**
```
[5, 2, 8, 1, 9, 3]
Split:  [5, 2, 8] [1, 9, 3]
Split:  [5] [2, 8]  [1] [9, 3]
Split:  [5] [2] [8] [1] [9] [3]
Merge:  [5] [2, 8]  [1] [3, 9]
Merge:  [2, 5, 8]   [1, 3, 9]
Merge:  [1, 2, 3, 5, 8, 9]
```

**Complexity:**
- Time: O(n log n) - always!
- Space: O(n) - needs temp array
- Stable: Yes

**When to use:**
- Large datasets
- Need guaranteed O(n log n)
- Linked lists (no random access needed)
- External sorting (data doesn't fit in memory)

**Pros:**
- Guaranteed O(n log n)
- Stable
- Good for linked lists
- Predictable performance

**Cons:**
- Needs O(n) extra space
- Slower than quicksort in practice

## Quick Sort

**Idea:** Pick a pivot, partition array around it, recursively sort partitions.

```c
void quick_sort(int arr[], int low, int high) {
    if (low < high) {
        int pivot = partition(arr, low, high);
        quick_sort(arr, low, pivot - 1);
        quick_sort(arr, pivot + 1, high);
    }
}

int partition(int arr[], int low, int high) {
    int pivot = arr[high];
    int i = low - 1;
    
    for (int j = low; j < high; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return i + 1;
}
```

**How it works:**
1. Choose a pivot element
2. Partition: elements < pivot left, > pivot right
3. Pivot is now in final position
4. Recursively sort left and right partitions

**Visual:**
```
[5, 2, 8, 1, 9] pivot=9
Partition: [5, 2, 8, 1] 9 []
[5, 2, 1, 8] pivot=8
Partition: [5, 2, 1] 8 []
[1, 2, 5] pivot=5
Partition: [1, 2] 5 []
[1, 2] -> Done!
Final: [1, 2, 5, 8, 9]
```

**Complexity:**
- Time: O(n log n) average, O(n²) worst
- Space: O(log n) recursion stack
- Stable: No

**When to use:**
- Large datasets
- In-place sorting needed
- Average case performance matters
- Most common general-purpose sort

**Pros:**
- Very fast in practice
- In-place (mostly)
- Cache-friendly

**Cons:**
- Worst case O(n²) (rare with good pivot)
- Not stable
- Recursive (stack overhead)

## Comparison Table

| Algorithm | Time (Best) | Time (Avg) | Time (Worst) | Space | Stable |
|-----------|-------------|------------|--------------|-------|--------|
| Bubble | O(n) | O(n²) | O(n²) | O(1) | Yes |
| Selection | O(n²) | O(n²) | O(n²) | O(1) | No |
| Insertion | O(n) | O(n²) | O(n²) | O(1) | Yes |
| Merge | O(n log n) | O(n log n) | O(n log n) | O(n) | Yes |
| Quick | O(n log n) | O(n log n) | O(n²) | O(log n) | No |

## When to Use Each

**Bubble Sort:**
- Never in production
- Educational purposes only
- Tiny datasets where simplicity matters

**Selection Sort:**
- Minimizing swaps
- Simple code needed
- Tiny datasets

**Insertion Sort:**
- Small datasets (< 50 elements)
- Nearly sorted data
- As part of hybrid algorithms

**Merge Sort:**
- Need guaranteed O(n log n)
- External sorting
- Linked lists
- Stable sort required

**Quick Sort:**
- General-purpose sorting
- Large datasets
- In-place needed
- Most common choice

## Stability

**Stable:** Equal elements keep their original order.

```c
// Sorting by score (stable sort):
[Alice:85, Bob:90, Charlie:85]
           ↓
[Alice:85, Charlie:85, Bob:90]
// Alice before Charlie (both 85)

// Unstable sort might give:
[Charlie:85, Alice:85, Bob:90]
// Order of equal elements changed
```

Matters when sorting by multiple keys (sort by name, then by score).

## In-Place vs Out-of-Place

**In-place:** Sorts within original array (O(1) extra space)
- Bubble, Selection, Insertion, Quick

**Out-of-place:** Needs additional memory
- Merge sort (O(n) temp array)

## Hybrid Approaches

Real-world sorts combine algorithms:

**Timsort** (Python, Java):
- Merge sort + Insertion sort
- Uses insertion sort for small runs
- Merge sort for combining

**Introsort** (C++ std::sort):
- Quick sort + Heap sort + Insertion sort
- Quick sort normally
- Heap sort if recursion too deep
- Insertion sort for small partitions

## Practical Tips

1. **Use library sort** for production (qsort, std::sort)
2. **Understand trade-offs** to choose wisely
3. **Profile before optimizing** - don't guess
4. **Consider data characteristics:**
   - Small? → Insertion sort
   - Nearly sorted? → Insertion sort
   - Need stable? → Merge sort
   - General purpose? → Quick sort

## Try This

Implement each algorithm and test with:
- Random data
- Sorted data
- Reverse sorted data
- Data with duplicates
- Small vs large datasets

Measure actual runtime and compare to Big O predictions!
