#!/bin/bash

echo "Building C Algorithm Examples (Linux)"
echo "======================================"
echo

mkdir -p bin

echo "Building 01_bubble_sort..."
gcc 01_bubble_sort.c -o bin/01_bubble_sort -lm || exit 1

echo "Building 02_selection_sort..."
gcc 02_selection_sort.c -o bin/02_selection_sort || exit 1

echo "Building 03_insertion_sort..."
gcc 03_insertion_sort.c -o bin/03_insertion_sort || exit 1

echo "Building 04_merge_sort..."
gcc 04_merge_sort.c -o bin/04_merge_sort || exit 1

echo "Building 05_quick_sort..."
gcc 05_quick_sort.c -o bin/05_quick_sort || exit 1

echo "Building 06_binary_search..."
gcc 06_binary_search.c -o bin/06_binary_search || exit 1

echo "Building 07_recursion..."
gcc 07_recursion.c -o bin/07_recursion || exit 1

echo "Building 08_backtracking..."
gcc 08_backtracking.c -o bin/08_backtracking || exit 1

echo
echo "All examples built successfully!"
echo "Run them from bin/"
echo "Example: ./bin/01_bubble_sort"
