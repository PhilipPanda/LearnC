#!/bin/bash
mkdir -p bin
echo "Building C Renderer Examples (Linux)..."

echo "Building 01_basic_shapes..."
gcc -o bin/01_basic_shapes 01_basic_shapes.c ../src/renderer.c -I ../src -lX11 -lm -O2 -Wall

echo "Building 02_3d_cube..."
gcc -o bin/02_3d_cube 02_3d_cube.c ../src/renderer.c -I ../src -lX11 -lm -O2 -Wall

echo "Building 03_3d_pyramid..."
gcc -o bin/03_3d_pyramid 03_3d_pyramid.c ../src/renderer.c -I ../src -lX11 -lm -O2 -Wall

echo "Building 04_particles..."
gcc -o bin/04_particles 04_particles.c ../src/renderer.c -I ../src -lX11 -lm -O2 -Wall

echo "Building 05_textured_cube..."
gcc -o bin/05_textured_cube 05_textured_cube.c ../src/renderer.c -I ../src -lX11 -lm -O2 -Wall

echo "Building 06_simple_game..."
gcc -o bin/06_simple_game 06_simple_game.c ../src/renderer.c -I ../src -lX11 -lm -O2 -Wall

echo ""
echo "All examples built! Run them from bin/"
echo "Example: ./bin/01_basic_shapes"
