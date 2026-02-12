/*
 * 02_arena_allocator.c
 * 
 * Arena (bump) allocator - fastest allocator for temporary data.
 * Allocate by bumping a pointer, free everything at once.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ===== Arena Allocator =====

typedef struct {
    char* buffer;      // Memory block
    size_t size;       // Total size
    size_t used;       // Currently used
} Arena;

// Create arena with given size
Arena* arena_create(size_t size) {
    Arena* arena = (Arena*)malloc(sizeof(Arena));
    if (arena == NULL) return NULL;
    
    arena->buffer = (char*)malloc(size);
    if (arena->buffer == NULL) {
        free(arena);
        return NULL;
    }
    
    arena->size = size;
    arena->used = 0;
    
    return arena;
}

// Allocate from arena (O(1) - just bump pointer!)
void* arena_alloc(Arena* arena, size_t size) {
    // Align to 8 bytes for better performance
    size_t aligned_size = (size + 7) & ~7;
    
    if (arena->used + aligned_size > arena->size) {
        return NULL;  // Out of space
    }
    
    void* ptr = arena->buffer + arena->used;
    arena->used += aligned_size;
    
    return ptr;
}

// Can't free individual allocations - that's the point!
// (Could implement, but defeats the purpose)

// Reset arena - free everything at once!
void arena_reset(Arena* arena) {
    arena->used = 0;
}

// Destroy arena
void arena_destroy(Arena* arena) {
    if (arena) {
        free(arena->buffer);
        free(arena);
    }
}

// Get arena statistics
void arena_print_stats(Arena* arena) {
    printf("Arena Stats:\n");
    printf("  Total size: %zu bytes\n", arena->size);
    printf("  Used: %zu bytes (%.1f%%)\n", 
           arena->used, (arena->used * 100.0) / arena->size);
    printf("  Free: %zu bytes\n", arena->size - arena->used);
}

// ===== Example Usage =====

typedef struct {
    int x, y;
    char name[32];
} Entity;

// Simulate game frame with temporary allocations
void simulate_game_frame(Arena* arena, int frame_num) {
    // All allocations for this frame use the arena
    
    // Allocate temporary entities
    int entity_count = 100;
    Entity* entities = (Entity*)arena_alloc(arena, entity_count * sizeof(Entity));
    
    if (entities == NULL) {
        printf("Arena out of space!\n");
        return;
    }
    
    // Initialize entities
    for (int i = 0; i < entity_count; i++) {
        entities[i].x = i * 10;
        entities[i].y = i * 20;
        snprintf(entities[i].name, sizeof(entities[i].name), "Entity_%d", i);
    }
    
    // Allocate temporary buffer for processing
    char* temp_buffer = (char*)arena_alloc(arena, 1024);
    if (temp_buffer) {
        snprintf(temp_buffer, 1024, "Frame %d: Processed %d entities", 
                 frame_num, entity_count);
    }
    
    // Allocate more temporary data
    int* temp_data = (int*)arena_alloc(arena, 500 * sizeof(int));
    if (temp_data) {
        for (int i = 0; i < 500; i++) {
            temp_data[i] = i * i;
        }
    }
    
    // No individual frees needed!
    // All memory freed at end of frame
}

// Compare arena vs malloc performance
void benchmark_arena_vs_malloc() {
    printf("\n=== Performance Comparison ===\n");
    
    const int iterations = 10000;
    const int allocs_per_iter = 100;
    clock_t start, end;
    double arena_time, malloc_time;
    
    // Benchmark arena
    Arena* arena = arena_create(1024 * 1024);  // 1MB
    start = clock();
    
    for (int i = 0; i < iterations; i++) {
        for (int j = 0; j < allocs_per_iter; j++) {
            void* p = arena_alloc(arena, 64);
            (void)p;  // Prevent optimization
        }
        arena_reset(arena);  // Free all at once
    }
    
    end = clock();
    arena_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    arena_destroy(arena);
    
    // Benchmark malloc/free
    start = clock();
    
    for (int i = 0; i < iterations; i++) {
        void* ptrs[100];
        for (int j = 0; j < allocs_per_iter; j++) {
            ptrs[j] = malloc(64);
        }
        for (int j = 0; j < allocs_per_iter; j++) {
            free(ptrs[j]);
        }
    }
    
    end = clock();
    malloc_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    // Results
    printf("Allocations: %d x %d = %d total\n", 
           iterations, allocs_per_iter, iterations * allocs_per_iter);
    printf("Arena time:  %.4f seconds\n", arena_time);
    printf("malloc time: %.4f seconds\n", malloc_time);
    printf("Speedup:     %.1fx faster\n", malloc_time / arena_time);
}

// Example: String processing with temporary allocations
void string_processing_example() {
    printf("\n=== String Processing Example ===\n");
    
    Arena* arena = arena_create(4096);  // 4KB for temporary strings
    
    const char* inputs[] = {
        "Hello World",
        "Arena Allocator",
        "Fast Memory Management",
        "Temporary Allocations"
    };
    
    printf("Processing strings:\n");
    
    for (int i = 0; i < 4; i++) {
        // Allocate temporary uppercase version
        size_t len = strlen(inputs[i]);
        char* upper = (char*)arena_alloc(arena, len + 1);
        
        if (upper) {
            // Convert to uppercase
            for (size_t j = 0; j < len; j++) {
                upper[j] = (inputs[i][j] >= 'a' && inputs[i][j] <= 'z') ?
                           inputs[i][j] - 32 : inputs[i][j];
            }
            upper[len] = '\0';
            
            printf("  '%s' -> '%s'\n", inputs[i], upper);
        }
        
        // No need to free - arena handles it!
    }
    
    arena_print_stats(arena);
    arena_destroy(arena);
}

// Example: Nested scopes
void nested_scope_example() {
    printf("\n=== Nested Scope Example ===\n");
    
    Arena* arena = arena_create(8192);
    
    printf("Outer scope:\n");
    int* outer_data = (int*)arena_alloc(arena, 100 * sizeof(int));
    for (int i = 0; i < 100; i++) {
        outer_data[i] = i;
    }
    printf("  Allocated outer_data (%zu bytes)\n", 100 * sizeof(int));
    arena_print_stats(arena);
    
    // Mark current position
    size_t checkpoint = arena->used;
    
    printf("\nInner scope:\n");
    {
        int* inner_data = (int*)arena_alloc(arena, 200 * sizeof(int));
        for (int i = 0; i < 200; i++) {
            inner_data[i] = i * 2;
        }
        printf("  Allocated inner_data (%zu bytes)\n", 200 * sizeof(int));
        arena_print_stats(arena);
    }
    
    // "Free" inner scope by resetting to checkpoint
    printf("\nAfter inner scope:\n");
    arena->used = checkpoint;
    arena_print_stats(arena);
    
    // outer_data still valid!
    printf("  outer_data[50] = %d (still valid)\n", outer_data[50]);
    
    arena_destroy(arena);
}

int main(void) {
    printf("=== Arena (Bump) Allocator ===\n");
    printf("Fastest allocator for temporary data\n");
    
    // Example 1: Basic usage
    printf("\n=== Example 1: Basic Usage ===\n");
    Arena* arena = arena_create(1024);
    
    printf("Created arena with 1024 bytes\n");
    arena_print_stats(arena);
    
    int* numbers = (int*)arena_alloc(arena, 10 * sizeof(int));
    printf("\nAllocated array of 10 ints\n");
    for (int i = 0; i < 10; i++) {
        numbers[i] = i * i;
    }
    arena_print_stats(arena);
    
    char* message = (char*)arena_alloc(arena, 50);
    strcpy(message, "Hello from arena!");
    printf("\nAllocated string: '%s'\n", message);
    arena_print_stats(arena);
    
    arena_destroy(arena);
    
    // Example 2: Game frame simulation
    printf("\n\n=== Example 2: Game Frame Simulation ===\n");
    Arena* frame_arena = arena_create(1024 * 100);  // 100KB per frame
    
    for (int frame = 0; frame < 3; frame++) {
        printf("\nFrame %d:\n", frame);
        arena_print_stats(frame_arena);
        
        simulate_game_frame(frame_arena, frame);
        
        printf("After allocations:\n");
        arena_print_stats(frame_arena);
        
        // Free everything for next frame!
        arena_reset(frame_arena);
    }
    
    arena_destroy(frame_arena);
    
    // Example 3: Performance benchmark
    benchmark_arena_vs_malloc();
    
    // Example 4: String processing
    string_processing_example();
    
    // Example 5: Nested scopes
    nested_scope_example();
    
    printf("\n\n=== Summary ===\n");
    printf("Arena allocator advantages:\n");
    printf("  - Extremely fast (just bump pointer)\n");
    printf("  - No fragmentation\n");
    printf("  - Free everything at once\n");
    printf("  - Simple implementation\n");
    printf("\nBest for:\n");
    printf("  - Per-frame game allocations\n");
    printf("  - Request handling (web servers)\n");
    printf("  - Parsing/compiling\n");
    printf("  - Any 'process and throw away' pattern\n");
    
    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}

/*
 * Arena Allocator Explained:
 * 
 * How It Works:
 * ┌────────────────────────────┐
 * │████████████░░░░░░░░░░░░░░░│ buffer
 * └────────────────────────────┘
 *             ↑ used pointer
 * 
 * Allocate: used += size (O(1))
 * Free all: used = 0 (O(1))
 * 
 * Advantages:
 * - Fastest possible allocator
 * - No fragmentation
 * - No per-allocation overhead
 * - Batch free (reset)
 * - Cache-friendly (linear allocation)
 * 
 * Disadvantages:
 * - Can't free individual allocations
 * - Must know maximum size upfront
 * - All allocations have same lifetime
 * 
 * When to Use:
 * - Temporary allocations with known scope
 * - Per-frame game data
 * - Request/response cycle
 * - Parsing phase
 * - String building
 * 
 * Performance:
 * - 10-100x faster than malloc/free
 * - No system calls after initial allocation
 * - Predictable performance
 * 
 * Variations:
 * - Growing arena: Allocate new chunks when full
 * - Scratch arena: Thread-local temporary storage
 * - Reset to checkpoint: Partial free
 */
