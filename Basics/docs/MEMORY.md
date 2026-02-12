# Memory Management

C gives you direct control over memory. That power comes with responsibility.

## Stack vs Heap

### Stack Memory

Automatic allocation, limited size, fast:

```c
int main(void) {
    int x = 5;           // Stack allocated
    int arr[100];        // Stack allocated
    // Automatically freed when function returns
}
```

**Pros:**
- Fast (just move stack pointer)
- Automatic cleanup
- Good for small, temporary data

**Cons:**
- Limited size (~1-8 MB typically)
- Can't return local arrays from functions
- Fixed size at compile time

### Heap Memory

Manual allocation, large size, slower:

```c
int *ptr = malloc(1000000 * sizeof(int));  // Heap allocated
// Must explicitly free
free(ptr);
```

**Pros:**
- Large size (limited by RAM)
- Dynamic sizing at runtime
- Can return from functions
- Lifetime controlled by you

**Cons:**
- Slower than stack
- Manual memory management
- Risk of leaks/errors

## Memory Functions

### malloc

Allocate memory:

```c
int *arr = malloc(10 * sizeof(int));
if (arr == NULL) {
    // Out of memory
}
// Use arr
free(arr);
```

Always use `sizeof` - don't hardcode sizes.

### calloc

Allocate and zero:

```c
int *arr = calloc(10, sizeof(int));  // All elements are 0
```

Equivalent to malloc + memset to zero, but potentially optimized.

### realloc

Resize allocation:

```c
int *arr = malloc(5 * sizeof(int));
// ... use it ...
arr = realloc(arr, 10 * sizeof(int));  // Now 10 elements
```

May return a different address. Can return NULL if it fails (old pointer still valid then).

### free

Release memory:

```c
free(ptr);
ptr = NULL;  // Prevent use-after-free
```

## Common Mistakes

### Memory Leak

Forgetting to free:

```c
void leak(void) {
    int *p = malloc(100);
    // Forgot to free - memory lost forever
}

for (int i = 0; i < 1000; i++) {
    leak();  // Leaking 100 bytes per iteration
}
```

### Use After Free

```c
int *p = malloc(sizeof(int));
*p = 42;
free(p);
printf("%d\n", *p);  // CRASH - memory no longer yours
```

### Double Free

```c
free(ptr);
free(ptr);  // CRASH - freeing twice
```

### Buffer Overflow

```c
char name[10];
strcpy(name, "This is way too long");  // CRASH - writes past end
```

### Dangling Pointer

```c
int* get_value(void) {
    int x = 42;
    return &x;  // WRONG - x dies when function returns
}

int *p = get_value();
*p = 100;  // CRASH - x doesn't exist anymore
```

## Good Practices

### Always Check malloc

```c
int *arr = malloc(size * sizeof(int));
if (arr == NULL) {
    fprintf(stderr, "Out of memory\n");
    return 1;
}
```

### Free in Pairs

Every malloc should have a matching free:

```c
ptr = malloc(...);  // Allocate
// Use it
free(ptr);          // Free
ptr = NULL;         // Nullify
```

### Use sizeof Correctly

```c
int *arr = malloc(n * sizeof(int));      // RIGHT
int *arr = malloc(n * sizeof(*arr));     // ALSO RIGHT
int *arr = malloc(n * 4);                // WRONG - assumes int is 4 bytes
```

### Null After Free

```c
free(ptr);
ptr = NULL;  // Prevents accidental use
```

## Memory Layout

A typical C program's memory:

```
[Code Segment]     - Your program instructions
[Data Segment]     - Global/static variables
[Heap]             - Grows up, malloc allocates here
    ↕
[Stack]            - Grows down, local variables here
```

Stack and heap grow toward each other. If they collide, you're out of memory.

## Debugging Memory Issues

### Linux: Valgrind

```bash
gcc -g program.c -o program
valgrind ./program
```

Shows leaks, invalid accesses, etc.

### General Tips

- Compile with `-Wall -Wextra` (shows warnings)
- Use `-g` flag for debugging symbols
- Print addresses with `%p` when debugging
- Check return values of all memory functions

## Size Matters

Check sizes on your system:

```c
printf("int: %zu bytes\n", sizeof(int));
printf("pointer: %zu bytes\n", sizeof(void*));
printf("char: %zu bytes\n", sizeof(char));  // Always 1
```

Pointers are usually 4 bytes (32-bit) or 8 bytes (64-bit), regardless of what they point to.

## When to Use What

**Stack:** Default choice for small, temporary data  
**Heap:** When you need:
- Large amounts of memory
- Dynamic sizing
- Data that outlives the function
- Memory shared across functions

Most programs use both - stack for local variables, heap for large structures and dynamic arrays.
