#!/bin/bash
# Build all compression examples (Linux/MSYS2)

echo "========================================"
echo "Building Compression Examples"
echo "========================================"
echo ""

# Create bin directory
mkdir -p bin

# Build each example
echo "Building 01_rle..."
gcc -o bin/01_rle 01_rle.c -lm
if [ $? -ne 0 ]; then
    echo "ERROR: Failed to build 01_rle"
    exit 1
fi

echo "Building 02_huffman..."
gcc -o bin/02_huffman 02_huffman.c -lm
if [ $? -ne 0 ]; then
    echo "ERROR: Failed to build 02_huffman"
    exit 1
fi

echo "Building 03_lz77..."
gcc -o bin/03_lz77 03_lz77.c -lm
if [ $? -ne 0 ]; then
    echo "ERROR: Failed to build 03_lz77"
    exit 1
fi

echo "Building 04_file_compression..."
gcc -o bin/04_file_compression 04_file_compression.c -lm
if [ $? -ne 0 ]; then
    echo "ERROR: Failed to build 04_file_compression"
    exit 1
fi

echo ""
echo "========================================"
echo "Build Complete!"
echo "========================================"
echo ""
echo "All examples built successfully in bin/ directory:"
echo "  - 01_rle"
echo "  - 02_huffman"
echo "  - 03_lz77"
echo "  - 04_file_compression"
echo ""
echo "To run examples:"
echo "  ./bin/01_rle"
echo "  ./bin/02_huffman"
echo "  ... etc"
echo ""
