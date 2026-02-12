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

echo "Building 04_chat_server..."
gcc 04_chat_server.c -o bin/04_chat_server || exit 1

echo "Building 05_udp_sender..."
gcc 05_udp_sender.c -o bin/05_udp_sender || exit 1

echo "Building 06_udp_receiver..."
gcc 06_udp_receiver.c -o bin/06_udp_receiver || exit 1

echo "Building 07_http_client..."
gcc 07_http_client.c -o bin/07_http_client || exit 1

echo "Building 08_file_transfer..."
gcc 08_file_transfer.c -o bin/08_file_transfer || exit 1

echo
echo "All examples built successfully!"
echo "Run them from bin/"
echo "Example: ./bin/01_simple_server"
