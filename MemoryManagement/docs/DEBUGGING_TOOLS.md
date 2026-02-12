# Debugging Memory Tools

## The Big Picture

Memory bugs are hard to spot. They might work fine until that one edge case. They might corrupt data silently. They might work on your machine but crash on production.

Tools catch what code review misses.

## Valgrind (Linux/Mac)

The gold standard for memory debugging.

### Installation

```bash
# Linux
sudo apt install valgrind

# Mac
brew install valgrind
```

### Basic Usage

```bash
valgrind --leak-check=full ./program
```

### What It Detects

- Memory leaks
- Use-after-free
- Double free
- Buffer overflows
- Uninitialized memory reads
- Invalid pointer usage

### Example Output

```
==12345== Invalid write of size 4
==12345==    at 0x40051B: main (test.c:10)
==12345==  Address 0x5204064 is 0 bytes after a block of size 100 alloc'd
==12345==    at 0x4C2AB80: malloc (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
==12345==    by 0x400505: main (test.c:8)
```

Translation: You wrote past the end of a malloc'd buffer at line 10.

### Options

```bash
# Full leak check
valgrind --leak-check=full --show-leak-kinds=all ./program

# Track origins of uninitialized values
valgrind --track-origins=yes ./program

# Generate suppression file
valgrind --gen-suppressions=all ./program
```

### Performance

Valgrind slows programs 10-100x. Use on small test cases.

## AddressSanitizer (ASan)

Faster than Valgrind, built into compilers.

### Usage

```bash
# Compile with ASan
gcc -fsanitize=address -g program.c -o program

# Run normally
./program
```

### What It Detects

- Heap buffer overflow
- Stack buffer overflow
- Use-after-free
- Double free
- Memory leaks (with additional flag)

### Example Output

```
=================================================================
==12345==ERROR: AddressSanitizer: heap-buffer-overflow on address 0x602000000064
WRITE of size 4 at 0x602000000064 thread T0
    #0 0x4008b4 in main test.c:10
    #1 0x7ffff7a2d82f in __libc_start_main
```

### Options

```bash
# Enable leak detection
export ASAN_OPTIONS=detect_leaks=1

# More verbose output
export ASAN_OPTIONS=verbosity=1

# Continue after first error
export ASAN_OPTIONS=halt_on_error=0
```

### Performance

Only 2-3x slowdown. Can use in testing regularly.

## MemorySanitizer (MSan)

Detects uninitialized memory reads.

```bash
clang -fsanitize=memory -g program.c -o program
./program
```

## UndefinedBehaviorSanitizer (UBSan)

Detects undefined behavior:

```bash
gcc -fsanitize=undefined -g program.c -o program
./program
```

Catches:
- Integer overflow
- Null pointer dereference
- Array bounds
- Type mismatches

## Static Analysis Tools

Find bugs without running code.

### Clang Static Analyzer

```bash
clang --analyze program.c
```

### Cppcheck

```bash
cppcheck --enable=all program.c
```

### Splint

```bash
splint program.c
```

## GDB (Debugger)

Inspect memory at runtime.

### Basic Commands

```bash
gdb ./program

(gdb) run                    # Run program
(gdb) break main             # Set breakpoint
(gdb) print variable         # Print value
(gdb) x/10x 0x602000         # Examine memory (hex)
(gdb) watch variable         # Break when variable changes
(gdb) backtrace              # Show call stack
```

### Detecting Corruption

```gdb
# Set watchpoint on memory
(gdb) watch *(int*)0x602000

# Program stops when that memory changes
# Shows you exactly where corruption happens
```

## Compiler Warnings

Enable all warnings:

```bash
gcc -Wall -Wextra -Wpedantic program.c
```

Common warnings to fix:
- Unused variables
- Uninitialized variables
- Type mismatches
- Implicit conversions

## Custom Guards

Add your own checks:

```c
#ifdef DEBUG
  #define ASSERT(cond) \
    if (!(cond)) { \
      fprintf(stderr, "Assertion failed: %s at %s:%d\n", \
              #cond, __FILE__, __LINE__); \
      abort(); \
    }
#else
  #define ASSERT(cond)
#endif

// Usage
char* buffer = (char*)malloc(100);
ASSERT(buffer != NULL);
```

### Guard Bytes

```c
#define GUARD_BYTE 0xAA

typedef struct {
    unsigned char guard_start[16];
    char data[100];
    unsigned char guard_end[16];
} GuardedBuffer;

GuardedBuffer* create_buffer() {
    GuardedBuffer* buf = (GuardedBuffer*)malloc(sizeof(GuardedBuffer));
    memset(buf->guard_start, GUARD_BYTE, 16);
    memset(buf->guard_end, GUARD_BYTE, 16);
    return buf;
}

void check_buffer(GuardedBuffer* buf) {
    for (int i = 0; i < 16; i++) {
        ASSERT(buf->guard_start[i] == GUARD_BYTE);
        ASSERT(buf->guard_end[i] == GUARD_BYTE);
    }
}
```

## Heap Debugging (Windows)

Windows has built-in heap debugging:

```c
#ifdef _WIN32
  #include <crtdbg.h>
  
  void enable_leak_detection() {
      _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
  }
  
  // At program exit, prints leaks
#endif
```

## Electric Fence

Catches buffer overflows immediately:

```bash
# Linux
sudo apt install electric-fence

# Link with efence
gcc program.c -o program -lefence
./program
```

Uses virtual memory tricks to crash on overflow.

## Workflow

**1. Develop:**
- Compile with `-Wall -Wextra`
- Use ASSERT liberally
- Enable runtime checks in debug build

**2. Test:**
- Run with AddressSanitizer
- Profile with Valgrind on critical paths
- Use static analysis tools

**3. Deploy:**
- Ship optimized build without checks
- Keep symbols for crash reports

## Common Valgrind Messages

**"Invalid read/write":**
Buffer overflow or use-after-free.

**"Conditional jump depends on uninit":**
Using uninitialized variable.

**"definitely lost":**
Memory leak, no pointers to it.

**"indirectly lost":**
Leaked structure containing leaked data.

**"still reachable":**
Not freed at exit, but pointer still exists. Usually OK (global).

## Tips

**1. Test early and often:**
Don't wait until the end to run tools.

**2. Fix warnings:**
Every warning is a potential bug.

**3. Use debug builds:**
```bash
gcc -g -O0  # Debug symbols, no optimization
```

**4. Reproduce bugs:**
Can't fix what you can't reproduce. Add logging.

**5. Bisect:**
Use git bisect to find when bug was introduced.

**6. Simplify:**
Create minimal test case that shows the bug.

## Summary

Tools make memory debugging tractable:

- Valgrind: Comprehensive, slow
- ASan: Fast, good for testing
- Static analyzers: No runtime cost
- GDB: Interactive debugging
- Compiler warnings: Catch obvious mistakes

Use all of them. They catch different bugs. Memory safety requires vigilance and tools.
