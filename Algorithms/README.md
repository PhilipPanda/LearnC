# C Algorithms

Learn classic algorithms through working implementations. No theory-heavy explanations - just clear code and visual output.

## What you get

- Sorting algorithms (bubble, insertion, selection, merge, quick)
- Searching algorithms (linear, binary, jump)
- Graph algorithms (BFS, DFS, Dijkstra)
- Dynamic programming basics
- String matching algorithms
- Recursion and backtracking
- Visual terminal output to see algorithms in action

Everything implemented from scratch. See exactly how each algorithm works step-by-step.

## Building

Each example is standalone:

```bash
# Windows
gcc 01_bubble_sort.c -o 01_bubble_sort.exe
.\01_bubble_sort.exe

# Linux
gcc 01_bubble_sort.c -o 01_bubble_sort
./01_bubble_sort
```

Or build all at once:
```bash
cd examples
./build_all.bat    # Windows
./build_all.sh     # Linux
```

## Using it

Here's bubble sort visualized:

```c
#include <stdio.h>

void bubble_sort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                // Swap
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

int main(void) {
    int arr[] = {64, 34, 25, 12, 22, 11, 90};
    int n = sizeof(arr) / sizeof(arr[0]);
    
    bubble_sort(arr, n);
    
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    return 0;
}
```

## Documentation

- **[Big O Notation](docs/BIG_O.md)** - Understanding algorithm complexity
- **[Sorting Algorithms](docs/SORTING.md)** - How each sort works
- **[Searching](docs/SEARCHING.md)** - Linear vs binary search
- **[Recursion](docs/RECURSION.md)** - Thinking recursively

## Examples

Each example includes visualization and analysis:

| Example | What It Teaches |
|---------|----------------|
| 01_bubble_sort | Simple comparison sort, O(n²) |
| 02_selection_sort | Finding minimum, in-place sorting |
| 03_insertion_sort | Building sorted array, efficient for small data |
| 04_merge_sort | Divide and conquer, O(n log n) |
| 05_quick_sort | Pivot-based sorting, average O(n log n) |
| 06_binary_search | Efficient search in sorted arrays, O(log n) |
| 07_recursion | Factorial, Fibonacci, understanding recursion |
| 08_backtracking | N-Queens problem, exploring all solutions |

Go in order. Each one builds on previous concepts.

## What this teaches

- Algorithm design and analysis
- Time and space complexity
- When to use each algorithm
- Optimization techniques
- Problem-solving strategies
- How to visualize algorithms
- Practical implementation skills

After working through this, you'll understand how to choose the right algorithm for your problem and implement it efficiently.
