# C Syntax Basics

Quick reference for C syntax. Not a complete guide - just the essentials.

## Program Structure

Every C program has this basic structure:

```c
#include <stdio.h>  // Include libraries

int main(void) {
    // Your code here
    return 0;
}
```

- `#include` - Bring in external code
- `main()` - Entry point, where program starts
- `return 0` - Exit code (0 = success)

## Variables

Declare before use:

```c
int age = 25;           // Integer
float price = 19.99f;   // Floating point
char letter = 'A';      // Single character
double pi = 3.14159;    // Double precision float
```

Constants (can't change):

```c
const int MAX_USERS = 100;
#define PI 3.14159  // Preprocessor constant
```

## Functions

```c
// Declaration
int add(int a, int b);

// Definition
int add(int a, int b) {
    return a + b;
}

// Call it
int result = add(5, 3);
```

## Control Flow

**If/Else:**
```c
if (x > 10) {
    // do something
} else if (x > 5) {
    // do something else
} else {
    // default case
}
```

**Loops:**
```c
// For loop
for (int i = 0; i < 10; i++) {
    printf("%d\n", i);
}

// While loop
while (condition) {
    // do stuff
}

// Do-while (runs at least once)
do {
    // do stuff
} while (condition);
```

**Switch:**
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

## Arrays

Fixed-size collections:

```c
int numbers[5];              // Declare
int numbers[5] = {1,2,3,4,5}; // Initialize
int x = numbers[0];          // Access first element
numbers[2] = 10;             // Modify third element
```

## Pointers

Store memory addresses:

```c
int x = 42;
int *ptr = &x;   // ptr points to x
*ptr = 100;      // Change x through pointer

// Pointer to array
int arr[3] = {1, 2, 3};
int *p = arr;    // Points to first element
printf("%d\n", *p);     // Prints 1
printf("%d\n", *(p+1)); // Prints 2
```

## Strings

Arrays of characters ending in `\0`:

```c
char name[] = "Alice";  // Creates: ['A','l','i','c','e','\0']
char *str = "Hello";    // Pointer to string literal

// String functions (need #include <string.h>)
strlen(str);           // Length
strcpy(dest, src);     // Copy
strcmp(s1, s2);        // Compare
strcat(dest, src);     // Concatenate
```

## Structs

Custom data types:

```c
struct Person {
    char name[50];
    int age;
    float height;
};

struct Person alice;
alice.age = 25;
strcpy(alice.name, "Alice");
```

## Memory Management

Dynamic allocation:

```c
#include <stdlib.h>

// Allocate
int *arr = malloc(10 * sizeof(int));
if (arr == NULL) {
    // Handle error
}

// Use it
arr[0] = 42;

// Free when done
free(arr);
```

## Common Patterns

**Loop through array:**
```c
int arr[5] = {1, 2, 3, 4, 5};
for (int i = 0; i < 5; i++) {
    printf("%d\n", arr[i]);
}
```

**Check for errors:**
```c
FILE *f = fopen("file.txt", "r");
if (f == NULL) {
    printf("Error opening file\n");
    return 1;
}
// Use file
fclose(f);
```

**Swap two values:**
```c
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}
```

## Operators

**Arithmetic:** `+`, `-`, `*`, `/`, `%`  
**Comparison:** `==`, `!=`, `<`, `>`, `<=`, `>=`  
**Logical:** `&&` (AND), `||` (OR), `!` (NOT)  
**Bitwise:** `&`, `|`, `^`, `~`, `<<`, `>>`  
**Assignment:** `=`, `+=`, `-=`, `*=`, `/=`  
**Increment:** `++`, `--`

## Comments

```c
// Single line comment

/*
 * Multi-line
 * comment
 */
```

## Common Mistakes

**Assignment vs Comparison:**
```c
if (x = 5)  // WRONG - assigns 5 to x
if (x == 5) // RIGHT - checks if x equals 5
```

**Array bounds:**
```c
int arr[5];
arr[5] = 10;  // WRONG - index 5 is out of bounds (0-4 are valid)
arr[4] = 10;  // RIGHT - last element
```

**Uninitialized pointers:**
```c
int *p;
*p = 5;  // WRONG - p points nowhere

int *p = NULL;
if (p != NULL) *p = 5;  // RIGHT - check first
```

**String terminator:**
```c
char str[5] = "hello";  // WRONG - no room for '\0'
char str[6] = "hello";  // RIGHT - 5 chars + '\0'
```

## Compilation

```bash
# Basic
gcc program.c -o program

# With warnings
gcc -Wall -Wextra program.c -o program

# With math library
gcc program.c -o program -lm

# With debugging
gcc -g program.c -o program
```
