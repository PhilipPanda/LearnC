# Algorithm Examples

Progressive examples demonstrating classic algorithms with visual output.

## Building

Windows:
```bash
build_all.bat
```

Linux:
```bash
./build_all.sh
```

## Examples

### 01_bubble_sort.c
**Bubble sort with visualization**

What it teaches:
- Simplest sorting algorithm
- Nested loops and O(n²) complexity
- When simple algorithms are OK
- Optimization with early exit

Visual output shows each swap and pass. Compare sorted vs nearly sorted performance.

### 02_selection_sort.c
**Selection sort - minimize swaps**

What it teaches:
- Finding minimum element
- In-place sorting
- Counting operations (comparisons vs swaps)
- When to minimize writes

Shows why selection sort makes fewer swaps than bubble sort.

### 03_insertion_sort.c
**Insertion sort - building sorted array**

What it teaches:
- Adaptive algorithms
- Best case O(n) on nearly sorted data
- Shifting elements
- When simple beats complex

Demonstrates massive speed difference between sorted and random input.

### 04_merge_sort.c
**Merge sort - divide and conquer**

What it teaches:
- Divide and conquer strategy
- Guaranteed O(n log n)
- Merging sorted arrays
- Space-time tradeoffs

Visualizes recursive splitting and merging. Shows consistent performance regardless of input.

### 05_quick_sort.c
**Quick sort - partitioning**

What it teaches:
- Partitioning around pivot
- Average O(n log n), worst O(n²)
- In-place sorting
- Most common general-purpose sort

Shows partition process and compares average vs worst case.

### 06_binary_search.c
**Binary search - efficient searching**

What it teaches:
- Logarithmic search O(log n)
- Divide and conquer on sorted data
- Iterative vs recursive
- Finding first occurrence

Compares binary vs linear search speed on large datasets. Dramatic difference!

### 07_recursion.c
**Recursion fundamentals**

What it teaches:
- Base case and recursive case
- Call stack visualization
- Memoization optimization
- When to use recursion

Examples: factorial, Fibonacci (naive + optimized), string reversal, palindrome, power, GCD.

### 08_backtracking.c
**N-Queens problem**

What it teaches:
- Backtracking strategy
- Exhaustive search with pruning
- State space exploration
- Constraint satisfaction

Solves N-Queens for different board sizes. Shows all solutions found.

## Learning Path

**Start with:**
1. 01 → 03: Understand O(n²) sorts
2. 04 → 05: Learn O(n log n) divide-and-conquer
3. 06: Master binary search
4. 07: Understand recursion deeply
5. 08: Apply to complex problem

**Compare algorithms:**
- Run sorting algorithms on same data
- Measure actual runtime
- See Big O in action
- Understand trade-offs

## Performance Testing

All examples include timing:
- Small arrays with visualization
- Large arrays with benchmarks
- Best/average/worst case demonstrations

**Try this:**
```bash
# Run each sort on 10,000 elements
bin\01_bubble_sort.exe    # Slowest
bin\02_selection_sort.exe # Slow
bin\03_insertion_sort.exe # Fast on nearly sorted
bin\04_merge_sort.exe     # Fast, consistent
bin\05_quick_sort.exe     # Fastest (usually)
```

## Visual Output

Examples show:
- Step-by-step process
- Array state after each operation
- Bar charts for sorting
- Comparison counts
- Swap counts
- Timing results

Makes algorithms concrete instead of abstract!

## Common Issues

**Stack overflow on large N:**
- Recursion uses stack space
- Large inputs can overflow
- Solution: use iteration or increase stack size

**Timing too fast:**
- Modern CPUs are fast
- Use larger datasets to see differences
- Run multiple iterations

## Next Steps

After understanding these basics:
- Implement heap sort
- Learn graph algorithms (BFS, DFS, Dijkstra)
- Study dynamic programming
- Explore advanced data structures
- Read algorithm analysis books

The patterns here apply everywhere!
