# Multithreading in C

Learn concurrent programming with threads. Understand race conditions, mutexes, deadlocks, and parallel patterns through working examples.

## What you get

- Thread creation and management
- Mutexes and synchronization
- Race conditions (and how to fix them)
- Producer-consumer pattern
- Deadlock scenarios and prevention
- Thread pools for parallel work

Everything shown with both Windows threads and pthreads where possible.

## Building

Windows uses native Windows threads, Linux uses pthreads:

```bash
# Windows
cd examples
build_all.bat

# Linux / MSYS2
cd examples
./build_all.sh
```

## Documentation

- **[Thread Basics](docs/THREAD_BASICS.md)** - Creating threads, joining, passing data
- **[Synchronization](docs/SYNCHRONIZATION.md)** - Mutexes, race conditions, critical sections
- **[Common Patterns](docs/PATTERNS.md)** - Producer-consumer, thread pools, work queues
- **[Pitfalls](docs/PITFALLS.md)** - Deadlocks, race conditions, debugging concurrent code

## Examples

| Example | What It Teaches |
|---------|----------------|
| 01_basic_threads | Creating threads, joining, passing arguments |
| 02_race_condition | Race conditions and why they're bad |
| 03_mutex | Using mutexes to fix race conditions |
| 04_producer_consumer | Producer-consumer with bounded buffer |
| 05_deadlock | Deadlock scenarios and how to prevent them |

Each example shows the problem, then the solution.

## What this teaches

- How threads work
- When code is thread-safe vs not
- Synchronization primitives (mutexes, condition variables)
- Common concurrent patterns
- How to debug race conditions
- Performance implications of locking
- When to use threads vs processes

After working through this, you'll understand concurrent programming and can write thread-safe code.

## Quick Start

```bash
cd examples
build_all.bat
bin\02_race_condition.exe    # See race condition in action
bin\03_mutex.exe              # See how mutex fixes it
```

Watch your output change between runs - that's concurrency!
