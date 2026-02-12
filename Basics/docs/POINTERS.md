# Pointers Explained

Pointers are the most important C concept. Once you get them, everything else makes sense.

## What is a Pointer?

A pointer is a variable that stores a memory address.

```c
int x = 42;      // Normal variable - stores value 42
int *ptr = &x;   // Pointer - stores the address of x
```

Think of memory like houses on a street:
- `x` is a house with the number 42 painted on it
- `&x` is the street address of that house
- `ptr` is a piece of paper with that address written on it
- `*ptr` means "go to that address and look at the number"

## The Two Operators

**`&` - Address-of operator**
```c
int x = 5;
int *ptr = &x;  // Get address of x
```

**`*` - Dereference operator**
```c
int value = *ptr;  // Get value at address stored in ptr
*ptr = 10;         // Set value at that address
```

## Example

```c
int x = 100;
printf("x = %d\n", x);              // Prints: x = 100
printf("Address of x = %p\n", &x);  // Prints: Address of x = 0x7ffd12345678

int *ptr = &x;                      // ptr now stores x's address
printf("ptr = %p\n", ptr);          // Prints same address as &x
printf("*ptr = %d\n", *ptr);        // Prints: *ptr = 100

*ptr = 200;                         // Change value through pointer
printf("x = %d\n", x);              // Prints: x = 200 (x changed!)
```

## Why Pointers?

### 1. Pass by Reference

Without pointers, functions get copies of variables:

```c
void try_to_change(int n) {
    n = 100;  // Only changes the copy
}

int x = 5;
try_to_change(x);
printf("%d\n", x);  // Still 5
```

With pointers, functions can modify the original:

```c
void actually_change(int *n) {
    *n = 100;  // Changes the original
}

int x = 5;
actually_change(&x);  // Pass address
printf("%d\n", x);  // Now 100
```

### 2. Dynamic Memory

You can't return local arrays from functions, but you can return pointers to dynamically allocated memory:

```c
int* create_array(int size) {
    int *arr = malloc(size * sizeof(int));
    return arr;  // Pointer to heap memory
}
```

### 3. Data Structures

Linked lists, trees, graphs - all use pointers to connect nodes:

```c
struct Node {
    int data;
    struct Node *next;  // Pointer to next node
};
```

## Pointer Arithmetic

Pointers can do math:

```c
int arr[5] = {10, 20, 30, 40, 50};
int *ptr = arr;  // Points to arr[0]

printf("%d\n", *ptr);      // 10
printf("%d\n", *(ptr+1));  // 20
printf("%d\n", *(ptr+2));  // 30

ptr++;  // Move to next element
printf("%d\n", *ptr);      // 20
```

When you do `ptr+1`, it doesn't add 1 byte - it adds `sizeof(type)` bytes. For `int*`, it moves 4 bytes (one int). This is why pointer arithmetic works with arrays.

## NULL Pointers

`NULL` means the pointer points nowhere:

```c
int *ptr = NULL;

// Always check before dereferencing
if (ptr != NULL) {
    *ptr = 5;  // Safe
}

// This crashes:
// *ptr = 5;  // ptr is NULL!
```

## Common Mistakes

### 1. Uninitialized Pointers

```c
int *ptr;     // Contains garbage address
*ptr = 5;     // CRASH - writing to random memory

// Fix:
int *ptr = NULL;
int x = 0;
ptr = &x;     // Now it points somewhere valid
*ptr = 5;     // OK
```

### 2. Dangling Pointers

```c
int *ptr = malloc(sizeof(int));
*ptr = 42;
free(ptr);    // Memory released
*ptr = 100;   // CRASH - memory no longer yours

// Fix:
free(ptr);
ptr = NULL;   // Mark as invalid
```

### 3. Lost Pointers

```c
int *ptr = malloc(100);
ptr = malloc(200);  // LEAK - lost reference to first 100 bytes

// Fix: free first allocation before reassigning
```

## Pointer Types

```c
int *pi;      // Pointer to int
char *pc;     // Pointer to char
float *pf;    // Pointer to float
void *pv;     // Generic pointer (can point to anything)
```

`void*` is special - you can assign any pointer type to it, but you must cast it back to use it:

```c
void *generic = malloc(sizeof(int));
int *actual = (int*)generic;
*actual = 42;
```

## Arrays and Pointers

Arrays are pointers in disguise:

```c
int arr[5] = {1, 2, 3, 4, 5};
int *ptr = arr;  // arr is the address of arr[0]

// These are equivalent:
arr[2]    == *(arr + 2)    == ptr[2]    == *(ptr + 2)
```

## Pointers to Pointers

You can have pointers to pointers:

```c
int x = 42;
int *ptr = &x;      // Pointer to x
int **pptr = &ptr;  // Pointer to pointer

printf("%d\n", **pptr);  // Dereference twice = 42
```

Used for dynamic 2D arrays and modifying pointers in functions.

## Function Pointers

Functions also have addresses:

```c
int add(int a, int b) {
    return a + b;
}

int (*func_ptr)(int, int) = add;  // Pointer to function
int result = func_ptr(3, 5);      // Call through pointer
```

Used for callbacks and dispatch tables.

## Rules of Thumb

1. **Initialize** pointers before using
2. **Check** for NULL before dereferencing
3. **Free** dynamically allocated memory
4. **Set to NULL** after freeing
5. **Don't** dereference NULL or freed pointers

## Practice

To really understand pointers, you need to:
1. Draw memory diagrams on paper
2. Write code that uses pointers
3. Debug crashes from bad pointer usage
4. Build data structures (linked lists, trees)

The examples in this folder all use pointers. Read them carefully.
