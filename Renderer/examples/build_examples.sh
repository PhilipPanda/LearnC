#!/bin/bash
echo "Building C Renderer Examples (Linux)"
echo "===================================="
echo

mkdir -p bin

echo "Building 01_basic_shapes..."
gcc -o bin/01_basic_shapes 01_basic_shapes.c ../src/renderer.c -I ../src -lX11 -lm -O2 -Wall

echo "Building 02_image_rendering..."
gcc -o bin/02_image_rendering 02_image_rendering.c ../src/renderer.c -I ../src -lX11 -lm -O2 -Wall

echo "Building 03_3d_cube..."
gcc -o bin/03_3d_cube 03_3d_cube.c ../src/renderer.c -I ../src -lX11 -lm -O2 -Wall

echo "Building 04_3d_pyramid..."
gcc -o bin/04_3d_pyramid 04_3d_pyramid.c ../src/renderer.c -I ../src -lX11 -lm -O2 -Wall

echo "Building 05_particles..."
gcc -o bin/05_particles 05_particles.c ../src/renderer.c -I ../src -lX11 -lm -O2 -Wall

echo "Building 06_textured_cube..."
gcc -o bin/06_textured_cube 06_textured_cube.c ../src/renderer.c -I ../src -lX11 -lm -O2 -Wall

echo "Building 07_simple_game..."
gcc -o bin/07_simple_game 07_simple_game.c ../src/renderer.c -I ../src -lX11 -lm -O2 -Wall

echo
echo "All examples built! Run them from bin/"
echo "Example: ./bin/01_basic_shapes"
