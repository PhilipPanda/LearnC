#!/bin/bash

echo "Building C Data Structure Examples (Linux)"
echo "==========================================="
echo

mkdir -p bin

echo "Building 01_linked_list..."
gcc 01_linked_list.c -o bin/01_linked_list || exit 1

echo "Building 02_doubly_linked_list..."
gcc 02_doubly_linked_list.c -o bin/02_doubly_linked_list || exit 1

echo "Building 03_stack..."
gcc 03_stack.c -o bin/03_stack || exit 1

echo "Building 04_queue..."
gcc 04_queue.c -o bin/04_queue || exit 1

echo "Building 05_binary_tree..."
gcc 05_binary_tree.c -o bin/05_binary_tree || exit 1

echo "Building 06_hash_table..."
gcc 06_hash_table.c -o bin/06_hash_table || exit 1

echo "Building 07_graph..."
gcc 07_graph.c -o bin/07_graph || exit 1

echo "Building 08_heap..."
gcc 08_heap.c -o bin/08_heap || exit 1

echo
echo "All examples built successfully!"
echo "Run them from bin/"
echo "Example: ./bin/01_linked_list"
