#!/bin/bash

echo "Building Game Programming Examples (Linux)"
echo "==========================================="
echo

mkdir -p bin

RENDERER="../../Renderer/src/renderer.c"
FLAGS="-I../../Renderer/src"

echo "Building snake..."
gcc snake.c $RENDERER $FLAGS -o bin/snake -lX11 -lm || exit 1

echo "Building pong..."
gcc pong.c $RENDERER $FLAGS -o bin/pong -lX11 -lm || exit 1

echo "Building breakout..."
gcc breakout.c $RENDERER $FLAGS -o bin/breakout -lX11 -lm || exit 1

echo
echo "All games built successfully!"
echo "Run them from bin/"
echo "Example: ./bin/snake"
