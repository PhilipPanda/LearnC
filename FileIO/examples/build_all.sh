#!/bin/bash

echo ""
echo "Building File I/O Examples (Linux)"
echo "============================================"

mkdir -p bin

echo "Building 01_text_basics..."
gcc -o bin/01_text_basics 01_text_basics.c -Wall || exit 1

echo "Building 02_word_count..."
gcc -o bin/02_word_count 02_word_count.c -Wall || exit 1

echo "Building 03_binary_data..."
gcc -o bin/03_binary_data 03_binary_data.c -Wall || exit 1

echo ""
echo "============================================"
echo "All examples built successfully!"
echo ""
echo "Executables in bin/ directory:"
ls -1 bin/
echo ""
echo "Try them:"
echo "  ./bin/01_text_basics"
echo "  ./bin/02_word_count"
echo "  ./bin/03_binary_data"
echo ""
