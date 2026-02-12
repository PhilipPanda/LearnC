# Memory Management

## The Big Picture

C gives you direct control over memory. You decide where data lives and when it dies. No garbage collector, no runtime - just you and the RAM.

This power lets you write fast, efficient code. It also lets you shoot yourself in the foot. Understanding memory is what separates C programmers from everyone else.

## Stack vs Heap

### Stack Memory

Automatic, fast, limited:

```c
int main(void) {
    int x = 5;           // Stack allocated
    int arr[100];        // Stack allocated
    // Automatically freed when function returns
}
```

**How it works:**

The stack is a region of memory that grows and shrinks as functions are called. When you declare a variable in a function, the stack pointer moves down to make room. When the function returns, it moves back up. Fast and simple.

**Pros:**
- Fast (just move a pointer)
- Automatic cleanup
- Good cache locality
- Perfect for small, temporary data

**Cons:**
- Limited size (typically 1-8 MB)
- Can't return local arrays from functions
- Size must be known at compile time

### Heap Memory

Manual, flexible, unlimited:

```c
int *ptr = malloc(1000000 * sizeof(int));  // Heap allocated
// Must explicitly free
free(ptr);
```

**How it works:**

The heap is a large pool of memory managed by the allocator. When you call `malloc`, it finds a free chunk of the requested size and returns its address. When you call `free`, that chunk becomes available again.

**Pros:**
- Large size (limited only by RAM)
- Dynamic sizing at runtime
- Can return from functions
- Lifetime controlled by you

**Cons:**
- Slower than stack (searching for free blocks)
- Manual memory management required
- Fragmentation over time
- Risk of leaks and errors

## Memory Functions

### malloc - Allocate

```c
int *arr = malloc(10 * sizeof(int));
if (arr == NULL) {
    printf("Out of memory\n");
    return 1;
}
// Use arr
free(arr);
```

`malloc` returns a pointer to uninitialized memory. The memory contains whatever garbage was there before. Always check if it returned `NULL` (allocation failed).

Always use `sizeof` - never hardcode byte counts. `sizeof(int)` is portable across systems.

### calloc - Allocate and Zero

```c
int *arr = calloc(10, sizeof(int));  // All elements are 0
```

Like `malloc`, but initializes everything to zero. Takes two arguments: count and size. Equivalent to `malloc` + `memset`, but potentially optimized by the system.

### realloc - Resize

```c
int *arr = malloc(5 * sizeof(int));
// ... use it ...
arr = realloc(arr, 10 * sizeof(int));  // Now 10 elements
if (arr == NULL) {
    // Realloc failed, old pointer still valid
}
```

Changes the size of an allocation. May return a new address (if it had to move the data) or the same address (if it could expand in place).

If it fails, returns `NULL` and the original pointer is still valid - don't lose it:

```c
int *new_arr = realloc(arr, bigger_size);
if (new_arr == NULL) {
    // Still have arr, can handle error gracefully
} else {
    arr = new_arr;  // Update pointer
}
```

### free - Release

```c
free(ptr);
ptr = NULL;  // Good practice
```

Returns memory to the heap. After `free`, the pointer is dangling - it points to memory you no longer own. Set it to `NULL` to prevent accidental use.

`free(NULL)` is safe and does nothing - useful for cleanup code.

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

The memory is allocated but unreachable. No way to free it. After the program runs for a while, it eats all RAM.

### Use After Free

```c
int *p = malloc(sizeof(int));
*p = 42;
free(p);
printf("%d\n", *p);  // CRASH - memory no longer yours
```

After `free`, the memory may be reused for something else. Reading or writing it is undefined behavior - might work, might crash, might corrupt data.

### Double Free

```c
free(ptr);
free(ptr);  // CRASH - freeing twice
```

The allocator's bookkeeping gets corrupted. Results in crashes or security vulnerabilities. This is why setting to `NULL` after `free` helps - `free(NULL)` is safe.

### Buffer Overflow

```c
char *name = malloc(10);
strcpy(name, "This is way too long");  // CRASH - writes past end
```

Writes beyond allocated memory. Corrupts adjacent data, crashes, or creates security holes. Always check sizes:

```c
strncpy(name, source, 9);
name[9] = '\0';  // Ensure null terminator
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

Returning address of local variable. Once the function returns, that stack memory is freed. The pointer points to garbage. Use heap memory for data that outlives the function:

```c
int* get_value(void) {
    int *x = malloc(sizeof(int));
    *x = 42;
    return x;  // OK - caller must free it
}
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

Allocation can fail. On embedded systems or large allocations, it's more common. Check every time.

### Free in Pairs

Every `malloc` should have a matching `free`:

```c
ptr = malloc(...);  // Allocate
// Use it
free(ptr);          // Free
ptr = NULL;         // Nullify
```

Use a consistent pattern. Makes it easier to audit for leaks.

### Use sizeof Correctly

```c
int *arr = malloc(n * sizeof(int));      // RIGHT
int *arr = malloc(n * sizeof(*arr));     // ALSO RIGHT (more portable)
int *arr = malloc(n * 4);                // WRONG - assumes int is 4 bytes
```

`sizeof(*arr)` is better because it's correct even if you change `arr`'s type later.

### Null After Free

```c
free(ptr);
ptr = NULL;  // Prevents accidental use
```

Double-free becomes a non-issue. Use-after-free crashes immediately instead of corrupting data.

## Memory Layout

A typical C program's memory layout:

```
High addresses
┌─────────────────┐
│   Stack         │  ← Grows down
│   (local vars)  │
├─────────────────┤
│                 │
│   (free space)  │
│                 │
├─────────────────┤
│   Heap          │  ← Grows up
│   (malloc)      │
├─────────────────┤
│   BSS           │  ← Uninitialized globals
├─────────────────┤
│   Data          │  ← Initialized globals
├─────────────────┤
│   Text/Code     │  ← Your program instructions
└─────────────────┘
Low addresses
```

Stack and heap grow toward each other. If they meet, you're out of memory (stack overflow or heap exhaustion).

## Debugging Memory Issues

### Linux: Valgrind

```bash
gcc -g program.c -o program
valgrind --leak-check=full ./program
```

Reports every leak, invalid access, uninitialized read. Essential tool for C programming.

### Compiler Warnings

```bash
gcc -Wall -Wextra -g program.c -o program
```

Catches many common issues at compile time. `-g` adds debug symbols for better error messages.

### Manual Debugging

```c
printf("Pointer: %p\n", (void*)ptr);
printf("Size: %zu\n", sizeof(*ptr));
```

Print addresses when things go wrong. Helps identify which allocation is causing problems.

### Address Sanitizer

```bash
gcc -fsanitize=address -g program.c -o program
./program
```

Runtime checking for memory errors. Catches use-after-free, buffer overflows, leaks. Slower but very thorough.

## Size Matters

Check sizes on your system:

```c
printf("char: %zu bytes\n", sizeof(char));      // Always 1
printf("int: %zu bytes\n", sizeof(int));        // Usually 4
printf("pointer: %zu bytes\n", sizeof(void*));  // 4 (32-bit) or 8 (64-bit)
printf("long: %zu bytes\n", sizeof(long));      // 4 or 8
```

Never assume sizes. Use `sizeof` everywhere. Different platforms have different sizes.

## When to Use What

**Use Stack When:**
- Data is small (< few KB)
- Size is known at compile time
- Lifetime is tied to function scope
- You need fast allocation/deallocation

**Use Heap When:**
- Data is large
- Size is determined at runtime
- Data must outlive the function
- Multiple functions need access
- Building dynamic data structures

Most programs use both - stack for local state, heap for large buffers and data structures.

## Memory Lifetime Example

```c
// Stack example
void process_data(void) {
    int buffer[100];  // Allocated when function starts
    // Use buffer
}  // Freed when function returns

// Heap example
int* create_buffer(int size) {
    int *buffer = malloc(size * sizeof(int));  // Allocated on heap
    return buffer;  // Pointer survives function return
}  // buffer still exists, caller must free

int main(void) {
    int *data = create_buffer(100);
    // Use data
    free(data);  // Explicitly free
    return 0;
}
```

Stack: automatic, scoped to function.  
Heap: manual, scoped to when you call `free`.

## The Golden Rules

1. **Initialize** pointers before use (to `NULL` or valid address)
2. **Check** every `malloc` return value
3. **Match** every `malloc` with a `free`
4. **Set** pointers to `NULL` after freeing
5. **Don't** access freed memory
6. **Use** `sizeof` for portability
7. **Enable** compiler warnings and memory checkers

Follow these, and you'll avoid 90% of memory bugs.
