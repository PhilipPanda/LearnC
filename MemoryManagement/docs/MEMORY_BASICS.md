# Memory Basics

## The Big Picture

In C, you manage memory yourself. Want to store something? Ask for memory. Done with it? Give it back. Forget to give it back? Memory leak. Use it after giving it back? Crash.

No garbage collector will save you. This is power and responsibility combined.

## Memory Layout

When your program runs, memory is divided into sections:

```
High addresses
┌─────────────────┐
│  Stack          │ ← Local variables, function calls
│  (grows down)   │
├─────────────────┤
│                 │
│  Free space     │
│                 │
├─────────────────┤
│  Heap           │ ← malloc allocates here
│  (grows up)     │
├─────────────────┤
│  BSS            │ ← Uninitialized global/static
├─────────────────┤
│  Data           │ ← Initialized global/static
├─────────────────┤
│  Text (Code)    │ ← Your compiled code
└─────────────────┘
Low addresses
```

## Stack Memory

Automatic, fast, limited:

```c
void foo() {
    int x = 42;        // Stack allocation
    char buffer[100];  // Stack allocation
    // Automatically freed when function returns
}
```

**Characteristics:**
- Allocation: Instant (just move stack pointer)
- Deallocation: Instant (move pointer back)
- Size: Fixed at compile time
- Lifetime: Scope of the function
- Size limit: ~1-8MB typically (stack overflow if exceeded)

**Pros:** Fast, automatic cleanup, no fragmentation  
**Cons:** Fixed size, limited space, can't return to caller

## Heap Memory

Manual, flexible, unlimited (within RAM):

```c
void foo() {
    int* p = (int*)malloc(100 * sizeof(int));  // Heap allocation
    // Use p...
    free(p);  // Manual cleanup required
}
```

**Characteristics:**
- Allocation: Slow (must find free block)
- Deallocation: Manual (you must call free)
- Size: Can be determined at runtime
- Lifetime: Until you free it
- Size limit: Available RAM

**Pros:** Flexible size, return to caller, large allocations  
**Cons:** Slow, manual management, fragmentation, leaks

## malloc and free

### malloc

```c
void* malloc(size_t size);
```

Allocates `size` bytes on heap, returns pointer to it:

```c
int* numbers = (int*)malloc(10 * sizeof(int));
if (numbers == NULL) {
    // Out of memory!
    fprintf(stderr, "Allocation failed\n");
    exit(1);
}

// Use numbers[0] through numbers[9]
```

**Always check for NULL!** malloc can fail.

### calloc

Like malloc but zeros the memory:

```c
int* numbers = (int*)calloc(10, sizeof(int));
// All elements are 0
```

### realloc

Resize existing allocation:

```c
int* numbers = (int*)malloc(10 * sizeof(int));
// ... need more space ...
numbers = (int*)realloc(numbers, 20 * sizeof(int));
if (numbers == NULL) {
    // Realloc failed, original memory still valid
}
```

**Warning:** realloc may move the memory! Old pointer might be invalid.

### free

```c
void free(void* ptr);
```

Returns memory to the heap:

```c
int* p = (int*)malloc(100);
// Use p...
free(p);
// p now points to freed memory - don't use it!
```

## How malloc/free Work (Simplified)

The heap is managed by the allocator. Simplified view:

```
Heap:
┌──────────────────────┐
│ Used | Free | Used   │
└──────────────────────┘
```

**malloc:**
1. Search for free block of requested size
2. If found, mark as used and return pointer
3. If not found, request more memory from OS
4. Split blocks if too large

**free:**
1. Mark block as free
2. Merge with adjacent free blocks (coalescing)
3. Return memory to OS (sometimes)

**Why it's slow:**
- Must search for free block
- May need to split/merge blocks
- May need system calls

## Memory Overhead

Each allocation has metadata:

```
┌────────────┬──────────────────┐
│ Metadata   │ Your data        │
│ (8-16 bytes│                  │
└────────────┴──────────────────┘
```

Metadata tracks:
- Size of allocation
- Free/used status
- Pointers to next/prev blocks

**Implication:** Many small allocations waste memory!

```c
// BAD: 1000 allocations, 1000 * 16 bytes overhead
for (int i = 0; i < 1000; i++) {
    int* p = (int*)malloc(sizeof(int));  // 4 + 16 bytes each
}

// GOOD: 1 allocation, 16 bytes overhead
int* arr = (int*)malloc(1000 * sizeof(int));  // 4000 + 16 bytes total
```

## Alignment

Memory is aligned to word boundaries (4 or 8 bytes):

```c
struct A {
    char c;    // 1 byte
    int i;     // 4 bytes
};

// Actual size: 8 bytes (not 5!)
// Padding added for alignment:
// char c | padding (3 bytes) | int i (4 bytes)
```

malloc always returns aligned memory.

## Fragmentation

**External fragmentation:**
```
Before:
[Used][Free 100KB][Used][Free 50KB][Used]

After many allocs/frees:
[Used][Free 10][Used][Free 5][Used][Free 15][Used]

Can't allocate 100KB even though total free is > 100KB!
```

**Internal fragmentation:**
```
Request 10 bytes, allocator gives 16 bytes
6 bytes wasted inside the allocation
```

## Lifetime Management

Who owns the memory?

```c
// Caller allocates and frees
char* buffer = (char*)malloc(100);
process_data(buffer);
free(buffer);

// Function allocates, caller frees
char* create_string() {
    char* s = (char*)malloc(100);
    strcpy(s, "Hello");
    return s;  // Caller must free!
}

// Function allocates and frees
void print_message() {
    char* temp = (char*)malloc(100);
    sprintf(temp, "Message %d", 42);
    printf("%s\n", temp);
    free(temp);  // Cleaned up here
}
```

**Rule:** Whoever allocates should document who frees!

## Common Patterns

### RAII-style (Resource Acquisition Is Initialization)

```c
typedef struct {
    int* data;
    size_t size;
} Array;

Array* array_create(size_t size) {
    Array* arr = (Array*)malloc(sizeof(Array));
    arr->data = (int*)malloc(size * sizeof(int));
    arr->size = size;
    return arr;
}

void array_destroy(Array* arr) {
    if (arr) {
        free(arr->data);
        free(arr);
    }
}

// Usage
Array* arr = array_create(100);
// Use arr...
array_destroy(arr);
```

### Reference Counting

```c
typedef struct {
    char* data;
    int ref_count;
} RefCountedString;

RefCountedString* string_create(const char* str) {
    RefCountedString* s = malloc(sizeof(*s));
    s->data = strdup(str);
    s->ref_count = 1;
    return s;
}

void string_retain(RefCountedString* s) {
    s->ref_count++;
}

void string_release(RefCountedString* s) {
    s->ref_count--;
    if (s->ref_count == 0) {
        free(s->data);
        free(s);
    }
}
```

## Performance Tips

**1. Batch allocations:**
```c
// Slow: Many small allocations
for (int i = 0; i < 1000; i++) {
    Node* n = (Node*)malloc(sizeof(Node));
}

// Fast: One large allocation
Node* nodes = (Node*)malloc(1000 * sizeof(Node));
```

**2. Reuse memory:**
```c
// Slow: Allocate and free in loop
for (int i = 0; i < 1000; i++) {
    char* temp = (char*)malloc(100);
    process(temp);
    free(temp);
}

// Fast: Allocate once, reuse
char* temp = (char*)malloc(100);
for (int i = 0; i < 1000; i++) {
    process(temp);
}
free(temp);
```

**3. Use stack when possible:**
```c
// Slower
char* buffer = (char*)malloc(100);
sprintf(buffer, "Hello %d", 42);
printf("%s\n", buffer);
free(buffer);

// Faster
char buffer[100];  // Stack allocation
sprintf(buffer, "Hello %d", 42);
printf("%s\n", buffer);
// Auto cleanup
```

## Summary

Stack: Fast, automatic, limited.  
Heap: Flexible, manual, slower.

malloc allocates, free deallocates. Always free what you malloc. Check for NULL. Be aware of overhead and fragmentation.

Memory management is not hard, but it requires discipline. One mistake = crash or leak.
