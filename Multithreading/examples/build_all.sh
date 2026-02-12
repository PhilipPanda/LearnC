#!/bin/bash

echo ""
echo "Building Multithreading Examples (Linux)"
echo "============================================"

mkdir -p bin

echo "Building 01_basic_threads..."
gcc -o bin/01_basic_threads 01_basic_threads.c -pthread -Wall || exit 1

echo "Building 02_race_condition..."
gcc -o bin/02_race_condition 02_race_condition.c -pthread -Wall || exit 1

echo "Building 03_mutex..."
gcc -o bin/03_mutex 03_mutex.c -pthread -Wall || exit 1

echo "Building 04_producer_consumer..."
gcc -o bin/04_producer_consumer 04_producer_consumer.c -pthread -Wall || exit 1

echo "Building 05_deadlock..."
gcc -o bin/05_deadlock 05_deadlock.c -pthread -Wall || exit 1

echo ""
echo "============================================"
echo "All examples built successfully!"
echo ""
echo "Executables in bin/ directory:"
ls -1 bin/
echo ""
echo "Run any example to see threading in action."
echo ""
