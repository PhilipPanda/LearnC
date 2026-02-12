#!/bin/bash

echo "Building C Networking Examples (Linux)"
echo "======================================"
echo

mkdir -p bin

echo "Building 01_simple_server..."
gcc 01_simple_server.c -o bin/01_simple_server || exit 1

echo "Building 02_simple_client..."
gcc 02_simple_client.c -o bin/02_simple_client || exit 1

echo "Building 03_echo_server..."
gcc 03_echo_server.c -o bin/03_echo_server || exit 1

echo
echo "All examples built successfully!"
echo "Run them from bin/"
echo "Example: ./bin/01_simple_server"
