# C Syntax Guide

## The Big Picture

C programs are simple: you write instructions, the compiler turns them into machine code, the CPU runs them. No runtime, no garbage collector, no magic. Just your code and the hardware.

## Program Structure

Every C program starts with `main`:

```c
#include <stdio.h>

int main(void) {
    printf("Hello, World!\n");
    return 0;
}
```

- `#include <stdio.h>` - Brings in the standard I/O library
- `main()` - Where your program starts
- `return 0` - Exit code (0 means success)

The compiler looks for `main` and starts there. Everything else is just functions you call.

## Variables and Types

Variables store data. You must declare them before use:

```c
int age = 25;              // Integer
float price = 19.99f;      // 32-bit floating point
double pi = 3.14159;       // 64-bit floating point
char letter = 'A';         // Single character (1 byte)
```

Each type has a size:
- `char` - 1 byte
- `int` - Usually 4 bytes
- `float` - 4 bytes
- `double` - 8 bytes

Constants that can't change:

```c
const int MAX_USERS = 100;
#define PI 3.14159  // Preprocessor constant (replaced before compile)
```

## Operators

### Arithmetic

```c
int a = 10 + 5;   // Addition
int b = 10 - 5;   // Subtraction
int c = 10 * 5;   // Multiplication
int d = 10 / 5;   // Division (integer division!)
int e = 10 % 3;   // Modulo (remainder): 1
```

Watch out: `7 / 2` gives `3`, not `3.5`. That's integer division. Use `7.0 / 2` for floating point.

### Comparison

```c
if (x == 5)   // Equal
if (x != 5)   // Not equal
if (x < 5)    // Less than
if (x > 5)    // Greater than
if (x <= 5)   // Less or equal
if (x >= 5)   // Greater or equal
```

### Logical

```c
if (x > 0 && x < 10)  // AND - both must be true
if (x < 0 || x > 10)  // OR - at least one must be true
if (!(x == 5))        // NOT - inverts the condition
```

### Increment and Assignment

```c
x++;      // Same as x = x + 1
x--;      // Same as x = x - 1
x += 5;   // Same as x = x + 5
x *= 2;   // Same as x = x * 2
```

## Control Flow

### If/Else

```c
if (x > 10) {
    printf("Big\n");
} else if (x > 5) {
    printf("Medium\n");
} else {
    printf("Small\n");
}
```

The braces `{}` define a block. Single-statement blocks don't need them, but use them anyway - less bugs.

### Loops

**For loop** - when you know how many iterations:

```c
for (int i = 0; i < 10; i++) {
    printf("%d\n", i);
}
```

That's: initialize `i`, check condition, run block, increment `i`, repeat.

**While loop** - when you don't know how many iterations:

```c
while (x > 0) {
    printf("%d\n", x);
    x--;
}
```

**Do-while** - runs at least once:

```c
do {
    printf("At least once\n");
} while (x > 0);
```

**Break and continue:**

```c
for (int i = 0; i < 10; i++) {
    if (i == 5) break;      // Exit loop immediately
    if (i == 3) continue;   // Skip to next iteration
    printf("%d\n", i);
}
```

### Switch

Pattern matching for integers:

```c
switch (value) {
    case 1:
        printf("One\n");
        break;
    case 2:
        printf("Two\n");
        break;
    default:
        printf("Other\n");
        break;
}
```

The `break` is important. Without it, execution "falls through" to the next case.

## Functions

Functions let you reuse code:

```c
// Declaration (tells compiler it exists)
int add(int a, int b);

// Definition (the actual code)
int add(int a, int b) {
    return a + b;
}

// Call it
int result = add(5, 3);  // result is 8
```

Functions that don't return anything use `void`:

```c
void print_message(void) {
    printf("Hello!\n");
}
```

Parameters are passed by value - the function gets a copy. To modify the original, pass a pointer (see Pointers section).

## Arrays

Fixed-size collections:

```c
int numbers[5];              // Declare 5 integers
int numbers[5] = {1,2,3,4,5}; // Initialize
int x = numbers[0];          // Access first element
numbers[2] = 10;             // Modify third element
```

Arrays are zero-indexed: `numbers[0]` is the first element, `numbers[4]` is the last in a 5-element array.

Multidimensional arrays:

```c
int grid[3][4];  // 3 rows, 4 columns
grid[0][0] = 1;  // Top-left element
```

## Pointers

Pointers store memory addresses:

```c
int x = 42;
int *ptr = &x;   // ptr holds the address of x
*ptr = 100;      // Changes x to 100
```

- `&x` - "address of x"
- `*ptr` - "value at address ptr points to"

Arrays and pointers are closely related:

```c
int arr[3] = {1, 2, 3};
int *p = arr;    // Points to first element

printf("%d\n", *p);     // 1
printf("%d\n", *(p+1)); // 2
printf("%d\n", *(p+2)); // 3
```

`arr` decays to a pointer to its first element.

See [POINTERS.md](POINTERS.md) for detailed explanation.

## Strings

Strings are arrays of characters ending in null terminator `\0`:

```c
char name[] = "Alice";  // Creates: ['A','l','i','c','e','\0']
char *str = "Hello";    // Pointer to string literal
```

String functions from `<string.h>`:

```c
strlen(str);           // Length (not counting '\0')
strcpy(dest, src);     // Copy string
strcmp(s1, s2);        // Compare (returns 0 if equal)
strcat(dest, src);     // Append src to dest
```

## Structs

Group related data together:

```c
struct Person {
    char name[50];
    int age;
    float height;
};

struct Person alice;
alice.age = 25;
strcpy(alice.name, "Alice");
alice.height = 1.65f;
```

You can use `typedef` to avoid writing `struct` every time:

```c
typedef struct {
    int x;
    int y;
} Point;

Point p;  // No need for "struct"
p.x = 10;
p.y = 20;
```

## Dynamic Memory

Allocate memory at runtime:

```c
#include <stdlib.h>

// Allocate array of 10 integers
int *arr = malloc(10 * sizeof(int));
if (arr == NULL) {
    printf("Out of memory!\n");
    return 1;
}

// Use it
arr[0] = 42;

// Free when done
free(arr);
```

Always check if `malloc` returned `NULL` (allocation failed). Always `free` what you allocate.

See [MEMORY.md](MEMORY.md) for stack vs heap and memory management details.

## File I/O

Reading and writing files:

```c
#include <stdio.h>

// Write to file
FILE *f = fopen("output.txt", "w");
if (f == NULL) {
    printf("Can't open file\n");
    return 1;
}
fprintf(f, "Hello, file!\n");
fclose(f);

// Read from file
FILE *f = fopen("input.txt", "r");
char line[100];
if (fgets(line, sizeof(line), f) != NULL) {
    printf("Read: %s", line);
}
fclose(f);
```

File modes:
- `"r"` - Read
- `"w"` - Write (creates or truncates)
- `"a"` - Append
- `"rb"`, `"wb"` - Binary mode

## Common Mistakes

### Assignment vs Comparison

```c
if (x = 5)  // WRONG - assigns 5 to x, then checks if 5 is true (it is)
if (x == 5) // RIGHT - checks if x equals 5
```

### Array Bounds

```c
int arr[5];
arr[5] = 10;  // WRONG - valid indices are 0-4
arr[4] = 10;  // RIGHT - last element
```

### Uninitialized Variables

```c
int x;
printf("%d\n", x);  // WRONG - x contains garbage

int x = 0;
printf("%d\n", x);  // RIGHT - initialized
```

### Forgetting Null Terminator

```c
char str[5] = "hello";  // WRONG - no room for '\0'
char str[6] = "hello";  // RIGHT - 5 chars + '\0'
```

## Compiling

```bash
# Basic
gcc program.c -o program

# With warnings (always use these)
gcc -Wall -Wextra program.c -o program

# With math library
gcc program.c -o program -lm

# Multiple files
gcc main.c helper.c -o program

# Debug symbols
gcc -g program.c -o program
```

Always compile with `-Wall -Wextra`. The compiler will catch many common mistakes.
