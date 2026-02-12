#!/bin/bash

echo "Building C Basics Examples"
echo "=========================="
echo ""

mkdir -p bin

echo "Building examples..."
for file in *.c; do
    name="${file%.c}"
    echo "  $file"
    gcc -Wall -Wextra -std=c99 "$file" -o "bin/$name"
done

echo ""
echo "Done! Run examples from bin/"
echo "Example: ./bin/01_hello_world"
