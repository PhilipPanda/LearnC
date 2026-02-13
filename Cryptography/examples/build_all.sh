#!/bin/bash
# Build all cryptography examples (Linux/MSYS2)

echo "========================================"
echo "Building Cryptography Examples"
echo "========================================"
echo ""

# Create bin directory
mkdir -p bin

# Build each example
echo "Building 01_hash_functions..."
gcc -o bin/01_hash_functions 01_hash_functions.c -lm
if [ $? -ne 0 ]; then
    echo "ERROR: Failed to build 01_hash_functions"
    exit 1
fi

echo "Building 02_symmetric_crypto..."
gcc -o bin/02_symmetric_crypto 02_symmetric_crypto.c -lm
if [ $? -ne 0 ]; then
    echo "ERROR: Failed to build 02_symmetric_crypto"
    exit 1
fi

echo "Building 03_password_hashing..."
gcc -o bin/03_password_hashing 03_password_hashing.c -lm
if [ $? -ne 0 ]; then
    echo "ERROR: Failed to build 03_password_hashing"
    exit 1
fi

echo "Building 04_base64..."
gcc -o bin/04_base64 04_base64.c -lm
if [ $? -ne 0 ]; then
    echo "ERROR: Failed to build 04_base64"
    exit 1
fi

echo "Building 05_rsa_basics..."
gcc -o bin/05_rsa_basics 05_rsa_basics.c -lm
if [ $? -ne 0 ]; then
    echo "ERROR: Failed to build 05_rsa_basics"
    exit 1
fi

echo "Building 06_file_encryption..."
gcc -o bin/06_file_encryption 06_file_encryption.c -lm
if [ $? -ne 0 ]; then
    echo "ERROR: Failed to build 06_file_encryption"
    exit 1
fi

echo ""
echo "========================================"
echo "Build Complete!"
echo "========================================"
echo ""
echo "All examples built successfully in bin/ directory:"
echo "  - 01_hash_functions"
echo "  - 02_symmetric_crypto"
echo "  - 03_password_hashing"
echo "  - 04_base64"
echo "  - 05_rsa_basics"
echo "  - 06_file_encryption"
echo ""
echo "To run examples:"
echo "  ./bin/01_hash_functions"
echo "  ./bin/02_symmetric_crypto"
echo "  ... etc"
echo ""
