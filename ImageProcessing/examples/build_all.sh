#!/bin/bash

echo ""
echo "Building Image Processing Examples (Linux)"
echo "============================================"

mkdir -p bin

echo "Building 01_bmp_reader..."
gcc -o bin/01_bmp_reader 01_bmp_reader.c -Wall -lm || exit 1

echo "Building 02_bmp_writer..."
gcc -o bin/02_bmp_writer 02_bmp_writer.c -Wall -lm || exit 1

echo "Building 03_image_filters..."
gcc -o bin/03_image_filters 03_image_filters.c -Wall -lm || exit 1

echo "Building 04_color_operations..."
gcc -o bin/04_color_operations 04_color_operations.c -Wall -lm || exit 1

echo "Building 05_transformations..."
gcc -o bin/05_transformations 05_transformations.c -Wall -lm || exit 1

echo "Building 06_image_effects..."
gcc -o bin/06_image_effects 06_image_effects.c -Wall -lm || exit 1

echo ""
echo "============================================"
echo "All examples built successfully!"
echo ""
echo "Executables in bin/ directory:"
ls -1 bin/
echo ""
echo "Try them:"
echo "  ./bin/01_bmp_reader"
echo "  ./bin/02_bmp_writer"
echo "  ./bin/03_image_filters"
echo "  ./bin/04_color_operations"
echo "  ./bin/05_transformations"
echo "  ./bin/06_image_effects"
echo ""
