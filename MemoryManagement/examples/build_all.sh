#!/bin/bash

echo "Building Memory Management Examples (Linux)"
echo "============================================"
echo

mkdir -p bin

echo "Building 01_memory_bugs..."
gcc 01_memory_bugs.c -o bin/01_memory_bugs || exit 1

echo "Building 02_arena_allocator..."
gcc 02_arena_allocator.c -o bin/02_arena_allocator || exit 1

echo "Building 03_memory_pool..."
gcc 03_memory_pool.c -o bin/03_memory_pool || exit 1

echo "Building 04_stack_allocator..."
gcc 04_stack_allocator.c -o bin/04_stack_allocator || exit 1

echo "Building 05_tracking_allocator..."
gcc 05_tracking_allocator.c -o bin/05_tracking_allocator || exit 1

echo
echo "All examples built successfully!"
echo "Run them from bin/"
echo "Example: ./bin/02_arena_allocator"
