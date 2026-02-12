# C Basics

Learn C syntax and fundamentals through working examples.

## What's Here

Each example teaches one concept. Read the code, compile it, run it, modify it. That's how you learn.

## Examples

### Getting Started
- **01_hello_world** - Your first C program
- **02_variables** - Types, declaration, initialization
- **03_operators** - Math, comparison, logical operations
- **04_control_flow** - if/else, loops, switch

### Functions and Scope
- **05_functions** - Defining and calling functions
- **06_scope** - Variable scope and lifetime
- **07_recursion** - Functions calling themselves

### Memory and Pointers
- **08_pointers** - What pointers are and how to use them
- **09_arrays** - Fixed-size collections
- **10_strings** - Character arrays and string functions
- **11_dynamic_memory** - malloc, free, memory management

### Data Structures
- **12_structs** - Custom data types
- **13_typedef** - Type aliases
- **14_enums** - Named constants

### Files and I/O
- **15_file_io** - Reading and writing files
- **16_command_line** - argc/argv, command-line arguments

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
./build_all.bat    # Windows
./build_all.sh     # Linux
```

## Documentation

- **[Syntax Guide](docs/SYNTAX.md)** - C syntax basics
- **[Pointers Explained](docs/POINTERS.md)** - Understanding pointers
- **[Memory Management](docs/MEMORY.md)** - Stack vs heap, malloc/free

## Learning Path

Go in order. Each example assumes you understood the previous ones.

Don't just read - type the code yourself, change things, break it and fix it. That's how concepts stick.
