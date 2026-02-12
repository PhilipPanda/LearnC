# Memory Bugs

## The Big Picture

Memory bugs are subtle and deadly. They might work fine in testing, then crash in production. They might corrupt data silently. They might work on one OS but fail on another.

Learning to spot and fix these bugs is essential for C programming.

## Memory Leaks

Allocated memory that's never freed:

```c
void leak_example() {
    int* p = (int*)malloc(100 * sizeof(int));
    // Forgot to free!
}  // Memory lost forever

// Call this 1000 times = 400KB leaked
```

**Symptoms:**
- Program uses more and more memory
- Eventually runs out of memory
- Slow performance (more allocations needed)

**Common causes:**
```c
// 1. Early return
char* process() {
    char* buffer = (char*)malloc(1000);
    if (error_condition) {
        return NULL;  // LEAK! Forgot to free buffer
    }
    // ...
    free(buffer);
    return result;
}

// 2. Exception/goto without cleanup
void foo() {
    char* a = (char*)malloc(100);
    char* b = (char*)malloc(100);
    if (error) {
        free(a);
        return;  // LEAK! Forgot to free b
    }
    free(a);
    free(b);
}

// 3. Lost pointer
void bar() {
    char* p = (char*)malloc(100);
    p = (char*)malloc(200);  // LEAK! Lost first allocation
    free(p);
}
```

**Fix: Always free in all code paths:**
```c
char* process() {
    char* buffer = (char*)malloc(1000);
    char* result = NULL;
    
    if (error_condition) {
        goto cleanup;
    }
    
    result = do_work(buffer);
    
cleanup:
    free(buffer);
    return result;
}
```

## Double Free

Freeing the same memory twice:

```c
int* p = (int*)malloc(100);
free(p);
free(p);  // CRASH! (or worse - silent corruption)
```

**Why it's bad:**
- Corrupts allocator's internal structures
- Might crash immediately
- Might crash later in unrelated code
- Might allow security exploits

**Common cause - multiple owners:**
```c
void process(Data* data) {
    // ...
    free(data);  // Who owns data?
}

void main() {
    Data* d = create_data();
    process(d);
    free(d);  // DOUBLE FREE if process already freed it
}
```

**Fix: Clear pointer after free:**
```c
free(p);
p = NULL;  // Now safe to free again
```

## Use-After-Free

Using memory after it's been freed:

```c
int* p = (int*)malloc(100);
free(p);
p[0] = 42;  // UNDEFINED BEHAVIOR!
```

**What happens:**
- Might work (memory not reused yet)
- Might crash (memory unmapped)
- Might silently corrupt data (memory reused)

**Common causes:**
```c
// 1. Dangling pointer
Node* node = list_head;
list_remove(node);  // Frees node internally
printf("%d\n", node->data);  // USE AFTER FREE!

// 2. Double ownership
char* a = (char*)malloc(100);
char* b = a;  // Both point to same memory
free(a);
printf("%s\n", b);  // USE AFTER FREE!

// 3. Returning freed memory
char* get_string() {
    char buffer[100];  // Stack memory
    sprintf(buffer, "Hello");
    return buffer;  // RETURNS ADDRESS OF FREED STACK MEMORY!
}
```

**Fix: Don't keep pointers to freed memory:**
```c
free(p);
p = NULL;  // Can't accidentally use it
```

## Buffer Overflow

Writing past the end of allocated memory:

```c
char* buffer = (char*)malloc(10);
strcpy(buffer, "This is way too long");  // OVERFLOW!
```

**Consequences:**
- Corrupts adjacent memory
- Crashes
- Security vulnerabilities
- Silent data corruption

**Common causes:**
```c
// 1. Wrong size calculation
int* arr = (int*)malloc(10);  // Only 10 bytes!
for (int i = 0; i < 10; i++) {
    arr[i] = i;  // OVERFLOW! Should be malloc(10 * sizeof(int))
}

// 2. Off-by-one
char* str = (char*)malloc(5);
strcpy(str, "Hello");  // OVERFLOW! Needs 6 bytes (5 + null terminator)

// 3. Unbounded string operations
char buffer[100];
gets(buffer);  // NO SIZE LIMIT! Can overflow
scanf("%s", buffer);  // Same problem!
```

**Fix: Always check bounds:**
```c
// Good
char* buffer = (char*)malloc(100);
strncpy(buffer, source, 99);  // Leave room for null
buffer[99] = '\0';  // Ensure null termination

// Better
snprintf(buffer, 100, "%s", source);  // Safe formatting
```

## Uninitialized Memory

Reading memory before writing to it:

```c
int* p = (int*)malloc(sizeof(int));
printf("%d\n", *p);  // UNDEFINED! Could be anything
```

**Why it's bad:**
- Values are random garbage
- Behavior is non-deterministic
- Might work in debug, fail in release
- Security: might leak sensitive data

**Fix: Initialize memory:**
```c
int* p = (int*)malloc(sizeof(int));
*p = 0;  // Initialize

// Or use calloc
int* p = (int*)calloc(1, sizeof(int));  // Already zeroed
```

## Memory Corruption

Writing to wrong memory location:

```c
int* arr = (int*)malloc(10 * sizeof(int));
*(arr + 100) = 42;  // Way past the end!
```

**Symptoms:**
- Random crashes
- Data mysteriously changes
- Works sometimes, crashes other times
- Very hard to debug

**Common causes:**
```c
// 1. Wrong pointer arithmetic
char* p = (char*)malloc(100);
int* ip = (int*)p;
ip[50] = 42;  // Past the end! (50 * 4 = 200 bytes)

// 2. Freed memory reused
char* a = (char*)malloc(100);
free(a);
char* b = (char*)malloc(100);  // Might reuse same memory
a[0] = 'X';  // Corrupts b!

// 3. Stack overflow
void recursive(int n) {
    char buffer[1000];
    recursive(n + 1);  // Eventually overflows stack
}
```

## Wild Pointer

Pointer that was never initialized:

```c
int* p;  // Garbage value
*p = 42;  // CRASH! Writing to random memory
```

**Fix: Always initialize pointers:**
```c
int* p = NULL;  // Safe initial value
```

## Null Pointer Dereference

Dereferencing NULL:

```c
int* p = (int*)malloc(100);
if (some_condition) {
    free(p);
    p = NULL;
}
*p = 42;  // CRASH if p is NULL
```

**Fix: Check before dereferencing:**
```c
if (p != NULL) {
    *p = 42;
}
```

## Detecting Memory Bugs

### Manual checking

```c
// Add guards
typedef struct {
    uint32_t guard_start;
    char data[100];
    uint32_t guard_end;
} GuardedBuffer;

#define GUARD_VALUE 0xDEADBEEF

GuardedBuffer* buf = (GuardedBuffer*)malloc(sizeof(GuardedBuffer));
buf->guard_start = GUARD_VALUE;
buf->guard_end = GUARD_VALUE;

// Later, check guards
assert(buf->guard_start == GUARD_VALUE);  // Overflow before?
assert(buf->guard_end == GUARD_VALUE);    // Overflow after?
```

### Valgrind (Linux/Mac)

```bash
valgrind --leak-check=full ./program
```

Detects:
- Memory leaks
- Use-after-free
- Buffer overflows
- Uninitialized memory

### AddressSanitizer (ASan)

```bash
gcc -fsanitize=address program.c -o program
./program
```

Detects same issues as Valgrind, faster.

### Static Analysis

```bash
cppcheck program.c
clang --analyze program.c
```

Finds bugs without running code.

## Best Practices

**1. Initialize everything:**
```c
int* p = NULL;
char buffer[100] = {0};
```

**2. Set to NULL after free:**
```c
free(p);
p = NULL;
```

**3. Use safe string functions:**
```c
// Bad
strcpy(dest, src);
sprintf(buffer, "%s", str);

// Good
strncpy(dest, src, sizeof(dest) - 1);
snprintf(buffer, sizeof(buffer), "%s", str);
```

**4. Check all allocations:**
```c
char* p = (char*)malloc(100);
if (p == NULL) {
    // Handle error
    return -1;
}
```

**5. Match alloc/free:**
```c
// Every malloc needs a free
// Every new needs a delete (C++)
// Every fopen needs an fclose
```

**6. Use tools:**
- Compile with `-Wall -Wextra`
- Use Valgrind or ASan in testing
- Enable core dumps for debugging

**7. Document ownership:**
```c
// Caller must free the returned string
char* create_string();

// Takes ownership of data, will free it
void consume_data(Data* data);

// Borrows data, does not free
void process_data(const Data* data);
```

## Summary

Memory bugs are the price of manual memory management. They're preventable with discipline:

- Always free what you allocate
- Never use freed memory
- Always check bounds
- Initialize everything
- Use tools to catch what you miss

One mistake can bring down the entire program. But master this, and you have complete control.
