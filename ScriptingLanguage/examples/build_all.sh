#!/bin/bash

echo ""
echo "Building Scripting Language Examples (Linux)"
echo "============================================"

mkdir -p bin

echo "Building 01_tokenizer..."
gcc -o bin/01_tokenizer 01_tokenizer.c -Wall || exit 1

echo "Building 02_calculator..."
gcc -o bin/02_calculator 02_calculator.c -Wall || exit 1

echo "Building 03_variables..."
gcc -o bin/03_variables 03_variables.c -Wall || exit 1

echo ""
echo "============================================"
echo "All examples built successfully!"
echo ""
echo "Executables in bin/ directory:"
ls -1 bin/
echo ""
echo "Try the calculator: ./bin/02_calculator"
echo "Try variables: ./bin/03_variables"
echo ""
