#!/bin/bash

echo ""
echo "Building Text Editor Examples (Linux)"
echo "============================================"

mkdir -p bin

echo "Building 01_simple_editor..."
gcc -o bin/01_simple_editor 01_simple_editor.c -Wall || exit 1

echo ""
echo "============================================"
echo "All examples built successfully!"
echo ""
echo "Executables in bin/ directory:"
ls -1 bin/
echo ""
echo "Try it:"
echo "  ./bin/01_simple_editor test.txt"
echo ""
echo "Controls:"
echo "  Arrow keys - Move cursor"
echo "  Backspace  - Delete"
echo "  Enter      - New line"
echo "  Ctrl+S     - Save"
echo "  Ctrl+Q     - Quit"
echo ""
