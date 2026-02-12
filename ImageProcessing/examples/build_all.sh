#!/bin/bash

echo ""
echo "Building Image Processing Examples (Linux)"
echo "============================================"

mkdir -p bin

echo "Building 01_bmp_reader..."
gcc -o bin/01_bmp_reader 01_bmp_reader.c -Wall -lm || exit 1

echo ""
echo "============================================"
echo "All examples built successfully!"
echo ""
echo "Executables in bin/ directory:"
ls -1 bin/
echo ""
echo "Try them:"
echo "  ./bin/01_bmp_reader path/to/image.bmp"
echo ""
