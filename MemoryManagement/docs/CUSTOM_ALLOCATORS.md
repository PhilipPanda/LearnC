# Custom Allocators

## The Big Picture

malloc is general-purpose. It handles any size, any pattern, any usage. That flexibility has a cost: it's slow.

If you know your allocation pattern, you can build a custom allocator that's 10-100x faster. Game engines, databases, compilers - they all use custom allocators for hot paths.

## Why Custom Allocators?

**Speed:**
- malloc: Search free list, split blocks, update metadata (~100-1000 cycles)
- Custom: Bump pointer (~3-10 cycles)

**Predictability:**
- malloc: Variable time, can fail unpredictably
- Custom: Constant time, controlled failure

**Fragmentation:**
- malloc: External fragmentation over time
- Custom: Can eliminate fragmentation entirely

**Batch operations:**
- malloc: Free one at a time
- Custom: Free thousands at once

**Cache locality:**
- malloc: Allocations scattered in memory
- Custom: Can keep related data together

## Arena Allocator (Bump Allocator)

Simplest and fastest allocator. Bump a pointer, that's it.

### How It Works

```
Arena:
┌────────────────────────────┐
│████████████░░░░░░░░░░░░░░░│
└────────────────────────────┘
            ^ current pointer

Allocate: Move pointer right
Free individual: Do nothing
Free all: Reset pointer to start
```

### Implementation

```c
typedef struct {
    char* buffer;
    size_t size;
    size_t used;
} Arena;

Arena* arena_create(size_t size) {
    Arena* arena = (Arena*)malloc(sizeof(Arena));
    arena->buffer = (char*)malloc(size);
    arena->size = size;
    arena->used = 0;
    return arena;
}

void* arena_alloc(Arena* arena, size_t size) {
    // Align to 8 bytes
    size = (size + 7) & ~7;
    
    if (arena->used + size > arena->size) {
        return NULL;  // Out of space
    }
    
    void* ptr = arena->buffer + arena->used;
    arena->used += size;
    return ptr;
}

void arena_free(Arena* arena, void* ptr) {
    // Do nothing - can't free individual allocations
}

void arena_reset(Arena* arena) {
    arena->used = 0;  // Free everything at once!
}

void arena_destroy(Arena* arena) {
    free(arena->buffer);
    free(arena);
}
```

### When to Use

**Perfect for:**
- Per-frame allocations (games)
- Request handling (web servers)
- Parsing (compilers)
- Any "process and throw away" pattern

**Example:**
```c
// Game loop
while (running) {
    Arena* frame_arena = arena_create(1024 * 1024);  // 1MB per frame
    
    // Allocate temporary game objects
    Entity* enemies = arena_alloc(frame_arena, 100 * sizeof(Entity));
    Particle* particles = arena_alloc(frame_arena, 1000 * sizeof(Particle));
    
    update_game();
    render();
    
    arena_destroy(frame_arena);  // Free everything at once!
}
```

**Performance:** 100x faster than malloc/free.

## Memory Pool (Object Pool)

Pre-allocate a bunch of same-sized objects. Allocation = grab from free list.

### How It Works

```
Pool of 4-byte objects:
┌────┬────┬────┬────┬────┬────┐
│Free│Free│Used│Free│Used│Free│
└────┴────┴────┴────┴────┴────┘
  ^                           
Free list: [0] -> [1] -> [3] -> [5]

Allocate: Pop from free list
Free: Push to free list
```

### Implementation

```c
typedef struct PoolNode {
    struct PoolNode* next;
} PoolNode;

typedef struct {
    char* buffer;
    PoolNode* free_list;
    size_t object_size;
    size_t count;
} Pool;

Pool* pool_create(size_t object_size, size_t count) {
    Pool* pool = (Pool*)malloc(sizeof(Pool));
    
    // Allocate buffer
    pool->object_size = object_size < sizeof(PoolNode) ? 
                       sizeof(PoolNode) : object_size;
    pool->count = count;
    pool->buffer = (char*)malloc(pool->object_size * count);
    
    // Build free list
    pool->free_list = NULL;
    for (size_t i = 0; i < count; i++) {
        PoolNode* node = (PoolNode*)(pool->buffer + i * pool->object_size);
        node->next = pool->free_list;
        pool->free_list = node;
    }
    
    return pool;
}

void* pool_alloc(Pool* pool) {
    if (pool->free_list == NULL) {
        return NULL;  // Pool exhausted
    }
    
    PoolNode* node = pool->free_list;
    pool->free_list = node->next;
    return node;
}

void pool_free(Pool* pool, void* ptr) {
    PoolNode* node = (PoolNode*)ptr;
    node->next = pool->free_list;
    pool->free_list = node;
}

void pool_destroy(Pool* pool) {
    free(pool->buffer);
    free(pool);
}
```

### When to Use

**Perfect for:**
- Game entities (enemies, bullets, particles)
- Network packets
- UI widgets
- Database connections
- Any fixed-size objects with frequent alloc/free

**Example:**
```c
Pool* bullet_pool = pool_create(sizeof(Bullet), 1000);

// Fast allocation
Bullet* b = (Bullet*)pool_alloc(bullet_pool);
if (b != NULL) {
    init_bullet(b, x, y, vx, vy);
}

// Fast deallocation
pool_free(bullet_pool, b);
```

**Performance:** 50x faster than malloc/free, O(1) guaranteed.

## Stack Allocator (LIFO Allocator)

Like arena, but can free in reverse order.

### How It Works

```
Stack:
┌────────────────────────────┐
│AAA│BBB│CCC│░░░░░░░░░░░░░░░│
└────────────────────────────┘
           ^ top

Allocate: Push (bump pointer)
Free: Pop (move pointer back)
Must free in reverse order!
```

### Implementation

```c
typedef struct {
    char* buffer;
    size_t size;
    size_t top;
} StackAllocator;

typedef struct {
    size_t previous_top;
} AllocationHeader;

StackAllocator* stack_create(size_t size) {
    StackAllocator* stack = (StackAllocator*)malloc(sizeof(StackAllocator));
    stack->buffer = (char*)malloc(size);
    stack->size = size;
    stack->top = 0;
    return stack;
}

void* stack_alloc(StackAllocator* stack, size_t size) {
    size_t total_size = sizeof(AllocationHeader) + size;
    total_size = (total_size + 7) & ~7;  // Align
    
    if (stack->top + total_size > stack->size) {
        return NULL;
    }
    
    AllocationHeader* header = (AllocationHeader*)(stack->buffer + stack->top);
    header->previous_top = stack->top;
    
    stack->top += total_size;
    
    return header + 1;  // Return memory after header
}

void stack_free(StackAllocator* stack, void* ptr) {
    if (ptr == NULL) return;
    
    AllocationHeader* header = ((AllocationHeader*)ptr) - 1;
    stack->top = header->previous_top;
}

void stack_destroy(StackAllocator* stack) {
    free(stack->buffer);
    free(stack);
}
```

### When to Use

**Perfect for:**
- Recursive algorithms
- Parser call stacks
- Scoped temporary allocations
- Expression evaluation

**Example:**
```c
void process_node(Node* node, StackAllocator* stack) {
    // Allocate temporary buffer
    char* temp = (char*)stack_alloc(stack, 1000);
    
    process_data(node, temp);
    
    // Recurse
    for (int i = 0; i < node->child_count; i++) {
        process_node(node->children[i], stack);
    }
    
    // Free in reverse order
    stack_free(stack, temp);
}
```

**Warning:** Must free in reverse order or corruption!

## Tracking Allocator (Wrapper)

Wraps malloc/free to track allocations:

```c
typedef struct Allocation {
    void* ptr;
    size_t size;
    const char* file;
    int line;
    struct Allocation* next;
} Allocation;

Allocation* all_allocations = NULL;

void* tracked_malloc(size_t size, const char* file, int line) {
    void* ptr = malloc(size);
    if (ptr) {
        Allocation* alloc = (Allocation*)malloc(sizeof(Allocation));
        alloc->ptr = ptr;
        alloc->size = size;
        alloc->file = file;
        alloc->line = line;
        alloc->next = all_allocations;
        all_allocations = alloc;
    }
    return ptr;
}

void tracked_free(void* ptr) {
    Allocation** current = &all_allocations;
    while (*current) {
        if ((*current)->ptr == ptr) {
            Allocation* to_free = *current;
            *current = (*current)->next;
            free(to_free);
            free(ptr);
            return;
        }
        current = &(*current)->next;
    }
    // Not found - double free or never allocated
}

void print_leaks() {
    Allocation* current = all_allocations;
    while (current) {
        printf("Leak: %zu bytes at %s:%d\n", 
               current->size, current->file, current->line);
        current = current->next;
    }
}

#define malloc(size) tracked_malloc(size, __FILE__, __LINE__)
#define free(ptr) tracked_free(ptr)
```

## Comparison

| Allocator | Speed | Fragmentation | Individual Free | Best For |
|-----------|-------|---------------|-----------------|----------|
| malloc | Slow | High | Yes | General purpose |
| Arena | Very fast | None | No | Temp allocations |
| Pool | Very fast | None | Yes | Fixed-size objects |
| Stack | Very fast | None | Yes (LIFO only) | Scoped allocations |

## Combining Allocators

Real systems use multiple allocators:

```c
typedef struct {
    Arena* frame_arena;      // Temp per-frame data
    Pool* entity_pool;       // Game entities
    Pool* particle_pool;     // Particles
    // Use malloc for everything else
} GameMemory;
```

## When NOT to Use Custom Allocators

- Simple programs
- Unpredictable allocation patterns
- When malloc is fast enough
- Small number of allocations
- Don't know the allocation pattern yet

**Rule:** Profile first, optimize later.

## Summary

Custom allocators trade generality for speed. If you know your allocation pattern, you can beat malloc by orders of magnitude.

Arena: Fastest, batch free.  
Pool: Fast, fixed-size, frequent alloc/free.  
Stack: Fast, LIFO pattern.

Real performance gains come from understanding your memory access patterns and choosing the right tool.
