# Memory Management in C

Deep dive into manual memory management. Learn malloc/free, debug memory leaks, and build custom allocators from scratch.

## What you get

- Understanding malloc/free internals
- Memory leak detection and prevention
- Custom allocators (arena, pool, stack)
- Common memory bugs and how to fix them
- Debugging with tools (Valgrind, sanitizers)
- Performance implications of allocation patterns

No garbage collector, no safety nets - raw control over memory.

## Building

Each example is standalone:

```bash
# Windows
cd examples
build_all.bat

# Linux
cd examples
./build_all.sh
```

## Documentation

- **[Memory Basics](docs/MEMORY_BASICS.md)** - Stack vs heap, malloc/free, memory layout
- **[Memory Bugs](docs/MEMORY_BUGS.md)** - Leaks, double free, use-after-free, corruption
- **[Custom Allocators](docs/CUSTOM_ALLOCATORS.md)** - Arena, pool, stack allocators
- **[Debugging Tools](docs/DEBUGGING_TOOLS.md)** - Valgrind, AddressSanitizer, tools

## Examples

| Example | What It Teaches |
|---------|----------------|
| 01_memory_bugs | Common bugs: leaks, double free, use-after-free |
| 02_arena_allocator | Fast bump allocator, free all at once |
| 03_memory_pool | Fixed-size object pool, O(1) alloc/free |
| 04_stack_allocator | LIFO allocator, very fast, scoped lifetime |
| 05_tracking_allocator | Wrapper to track allocations and find leaks |

Each example shows working code with explanations.

## Quick Start

```bash
cd examples
build_all.bat

# See common memory bugs
bin\01_memory_bugs.exe

# Build and test an arena allocator
bin\02_arena_allocator.exe
```

## What this teaches

- When to use malloc vs custom allocators
- How to prevent memory leaks
- Debugging memory issues
- Performance: allocation is expensive
- Memory fragmentation
- Ownership and lifetime management
- Building allocators from scratch

After this, you'll understand why languages have garbage collectors and how to manage memory manually when you need maximum performance.
