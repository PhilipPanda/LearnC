#!/bin/bash
mkdir -p bin
echo "Building C Renderer (Linux)..."
gcc -o bin/CRenderer src/main.c src/renderer.c -I src -lX11 -lm -O2 -Wall
if [ $? -eq 0 ]; then
    echo "Build successful! Run: ./bin/CRenderer"
else
    echo "Build failed!"
fi
