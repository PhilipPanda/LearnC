# C Basics

Learn C syntax and fundamentals through working examples.

## What you get

- Variables and types
- Operators and expressions
- Control flow (if, loops, switch)
- Functions and scope
- Pointers and memory addresses
- Arrays and strings
- Dynamic memory allocation
- Structs and custom types
- File I/O

Each example is standalone and teaches one concept.

## Building

Each example is standalone:

```bash
# Windows
gcc 01_hello_world.c -o 01_hello_world.exe
.\01_hello_world.exe

# Linux
gcc 01_hello_world.c -o 01_hello_world
./01_hello_world
```

Or build all at once:
```bash
cd examples
./build_all.bat    # Windows
./build_all.sh     # Linux
```

## Using it

Here's a simple example:

```c
#include <stdio.h>

int main(void) {
    int x = 42;
    int *ptr = &x;  // Pointer to x
    
    printf("x = %d\n", x);
    printf("Address: %p\n", (void*)&x);
    printf("Via pointer: %d\n", *ptr);
    
    *ptr = 100;  // Modify through pointer
    printf("x is now: %d\n", x);
    
    return 0;
}
```

Compile:
```bash
gcc program.c -o program
```

## Documentation

- **[Syntax Guide](docs/SYNTAX.md)** - C syntax quick reference
- **[Pointers Explained](docs/POINTERS.md)** - Understanding pointers
- **[Memory Management](docs/MEMORY.md)** - Stack vs heap, malloc/free

## Examples

Each example teaches something specific:

| Example | What It Teaches |
|---------|----------------|
| 01_hello_world | Basic program structure |
| 02_variables | Types, declaration, initialization |
| 03_operators | Arithmetic, comparison, logical operations |
| 04_control_flow | if/else, loops, switch statements |
| 05_functions | Function declaration, definition, calling |
| 06_arrays | Fixed-size collections, indexing |
| 07_strings | Character arrays, string functions |
| 08_pointers | Memory addresses, dereferencing |
| 09_memory | malloc, free, dynamic allocation |
| 10_structs | Custom data types, nested structs |
| 11_file_io | Reading and writing files |

Go in order. Each one builds on previous concepts.

## What this teaches

- C syntax and semantics
- How memory works in C
- Pointer manipulation
- Manual memory management
- Function calling and stack frames
- Data layout and alignment
- File operations and I/O

This is foundation knowledge for systems programming, embedded development, and understanding how higher-level languages work under the hood.
