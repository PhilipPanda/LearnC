/*
 * 03_memory_pool.c
 * 
 * Memory pool for fixed-size objects.
 * O(1) allocation and deallocation, no fragmentation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ===== Memory Pool =====

typedef struct PoolNode {
    struct PoolNode* next;
} PoolNode;

typedef struct {
    char* buffer;
    PoolNode* free_list;
    size_t object_size;
    size_t total_count;
    size_t free_count;
} Pool;

// Create pool for fixed-size objects
Pool* pool_create(size_t object_size, size_t count) {
    Pool* pool = (Pool*)malloc(sizeof(Pool));
    if (pool == NULL) return NULL;
    
    // Object must be at least size of PoolNode (for free list)
    pool->object_size = object_size < sizeof(PoolNode) ? 
                        sizeof(PoolNode) : object_size;
    pool->total_count = count;
    pool->free_count = count;
    
    // Allocate buffer for all objects
    pool->buffer = (char*)malloc(pool->object_size * count);
    if (pool->buffer == NULL) {
        free(pool);
        return NULL;
    }
    
    // Build free list
    pool->free_list = NULL;
    for (size_t i = 0; i < count; i++) {
        PoolNode* node = (PoolNode*)(pool->buffer + i * pool->object_size);
        node->next = pool->free_list;
        pool->free_list = node;
    }
    
    return pool;
}

// Allocate object from pool (O(1))
void* pool_alloc(Pool* pool) {
    if (pool->free_list == NULL) {
        return NULL;  // Pool exhausted
    }
    
    PoolNode* node = pool->free_list;
    pool->free_list = node->next;
    pool->free_count--;
    
    return node;
}

// Free object back to pool (O(1))
void pool_free(Pool* pool, void* ptr) {
    if (ptr == NULL) return;
    
    PoolNode* node = (PoolNode*)ptr;
    node->next = pool->free_list;
    pool->free_list = node;
    pool->free_count++;
}

// Get pool statistics
void pool_print_stats(Pool* pool) {
    printf("Pool Stats:\n");
    printf("  Object size: %zu bytes\n", pool->object_size);
    printf("  Total objects: %zu\n", pool->total_count);
    printf("  Free objects: %zu\n", pool->free_count);
    printf("  Used objects: %zu (%.1f%%)\n", 
           pool->total_count - pool->free_count,
           ((pool->total_count - pool->free_count) * 100.0) / pool->total_count);
}

// Destroy pool
void pool_destroy(Pool* pool) {
    if (pool) {
        free(pool->buffer);
        free(pool);
    }
}

// ===== Example: Bullet System =====

typedef struct {
    float x, y;
    float vx, vy;
    int active;
    int damage;
} Bullet;

void bullet_system_example() {
    printf("\n=== Bullet System Example ===\n");
    
    // Create pool for 1000 bullets
    Pool* bullet_pool = pool_create(sizeof(Bullet), 1000);
    printf("Created bullet pool\n");
    pool_print_stats(bullet_pool);
    
    // Spawn some bullets
    printf("\nSpawning 10 bullets:\n");
    Bullet* bullets[10];
    for (int i = 0; i < 10; i++) {
        bullets[i] = (Bullet*)pool_alloc(bullet_pool);
        if (bullets[i]) {
            bullets[i]->x = i * 10.0f;
            bullets[i]->y = 100.0f;
            bullets[i]->vx = 5.0f;
            bullets[i]->vy = 0.0f;
            bullets[i]->active = 1;
            bullets[i]->damage = 10;
            printf("  Bullet %d at (%.0f, %.0f)\n", i, bullets[i]->x, bullets[i]->y);
        }
    }
    
    pool_print_stats(bullet_pool);
    
    // Free some bullets
    printf("\nFreeing bullets 0, 2, 4:\n");
    pool_free(bullet_pool, bullets[0]);
    pool_free(bullet_pool, bullets[2]);
    pool_free(bullet_pool, bullets[4]);
    
    pool_print_stats(bullet_pool);
    
    // Reuse freed slots
    printf("\nSpawning 3 more bullets (reusing freed slots):\n");
    for (int i = 0; i < 3; i++) {
        Bullet* b = (Bullet*)pool_alloc(bullet_pool);
        if (b) {
            b->x = (i + 10) * 10.0f;
            b->y = 200.0f;
            printf("  New bullet at (%.0f, %.0f)\n", b->x, b->y);
        }
    }
    
    pool_print_stats(bullet_pool);
    pool_destroy(bullet_pool);
}

// ===== Performance Benchmark =====

void benchmark_pool_vs_malloc() {
    printf("\n=== Performance Benchmark ===\n");
    
    const int iterations = 100000;
    clock_t start, end;
    double pool_time, malloc_time;
    
    // Benchmark pool
    Pool* pool = pool_create(64, iterations);
    void* ptrs[1000];
    
    start = clock();
    for (int i = 0; i < iterations; i++) {
        // Allocate
        ptrs[i % 1000] = pool_alloc(pool);
        
        // Free some to create churn
        if (i >= 1000) {
            pool_free(pool, ptrs[i % 1000]);
        }
    }
    end = clock();
    pool_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    pool_destroy(pool);
    
    // Benchmark malloc/free
    start = clock();
    for (int i = 0; i < iterations; i++) {
        ptrs[i % 1000] = malloc(64);
        
        if (i >= 1000) {
            free(ptrs[i % 1000]);
        }
    }
    end = clock();
    malloc_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    // Results
    printf("Operations: %d\n", iterations);
    printf("Pool time:   %.4f seconds\n", pool_time);
    printf("malloc time: %.4f seconds\n", malloc_time);
    printf("Speedup:     %.1fx faster\n", malloc_time / pool_time);
}

// ===== Example: Particle System =====

typedef struct {
    float x, y;
    float vx, vy;
    float life;
    unsigned char r, g, b;
} Particle;

void particle_system_example() {
    printf("\n=== Particle System Example ===\n");
    
    Pool* particle_pool = pool_create(sizeof(Particle), 5000);
    printf("Created particle pool for 5000 particles\n");
    
    // Simulate particle spawning/dying
    Particle* active[100];
    int active_count = 0;
    
    printf("\nSpawning particles:\n");
    for (int frame = 0; frame < 5; frame++) {
        printf("Frame %d:\n", frame);
        
        // Spawn new particles
        for (int i = 0; i < 20; i++) {
            Particle* p = (Particle*)pool_alloc(particle_pool);
            if (p && active_count < 100) {
                p->x = (float)(rand() % 100);
                p->y = (float)(rand() % 100);
                p->vx = (float)(rand() % 10 - 5);
                p->vy = (float)(rand() % 10 - 5);
                p->life = 1.0f;
                p->r = rand() % 255;
                p->g = rand() % 255;
                p->b = rand() % 255;
                active[active_count++] = p;
            }
        }
        
        // Update particles, kill old ones
        int alive = 0;
        for (int i = 0; i < active_count; i++) {
            active[i]->life -= 0.2f;
            if (active[i]->life <= 0) {
                pool_free(particle_pool, active[i]);
            } else {
                active[alive++] = active[i];
            }
        }
        active_count = alive;
        
        pool_print_stats(particle_pool);
    }
    
    pool_destroy(particle_pool);
}

int main(void) {
    printf("=== Memory Pool Allocator ===\n");
    printf("Fixed-size object pool with O(1) operations\n");
    
    srand(time(NULL));
    
    // Example 1: Basic usage
    printf("\n=== Example 1: Basic Usage ===\n");
    Pool* pool = pool_create(sizeof(int), 100);
    
    printf("Created pool for 100 integers\n");
    pool_print_stats(pool);
    
    // Allocate some objects
    int* numbers[10];
    printf("\nAllocating 10 integers:\n");
    for (int i = 0; i < 10; i++) {
        numbers[i] = (int*)pool_alloc(pool);
        if (numbers[i]) {
            *numbers[i] = i * i;
            printf("  numbers[%d] = %d\n", i, *numbers[i]);
        }
    }
    
    pool_print_stats(pool);
    
    // Free some
    printf("\nFreeing numbers 0, 5, 9:\n");
    pool_free(pool, numbers[0]);
    pool_free(pool, numbers[5]);
    pool_free(pool, numbers[9]);
    
    pool_print_stats(pool);
    
    pool_destroy(pool);
    
    // Example 2: Bullet system
    bullet_system_example();
    
    // Example 3: Performance
    benchmark_pool_vs_malloc();
    
    // Example 4: Particle system
    particle_system_example();
    
    printf("\n\n=== Summary ===\n");
    printf("Memory pool advantages:\n");
    printf("  - O(1) allocation and deallocation\n");
    printf("  - No fragmentation\n");
    printf("  - Excellent cache locality\n");
    printf("  - Predictable performance\n");
    printf("\nBest for:\n");
    printf("  - Game entities (bullets, enemies, particles)\n");
    printf("  - Network packets\n");
    printf("  - UI widgets\n");
    printf("  - Any fixed-size objects with frequent alloc/free\n");
    
    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}

/*
 * Memory Pool Explained:
 * 
 * Structure:
 * ┌────┬────┬────┬────┬────┬────┐
 * │Free│Free│Used│Free│Used│Free│
 * └────┴────┴────┴────┴────┴────┘
 *   ↓    ↓         ↓         ↓
 * Free list: [0] -> [1] -> [3] -> [5]
 * 
 * Allocate: Pop from free list (O(1))
 * Free: Push to free list (O(1))
 * 
 * Advantages:
 * - Guaranteed O(1) operations
 * - No fragmentation
 * - Cache-friendly
 * - No per-object overhead
 * 
 * Disadvantages:
 * - Only one object size
 * - Fixed maximum count
 * - Memory usage = total * object_size
 * 
 * When to Use:
 * - Many allocations of same-sized objects
 * - Frequent alloc/free cycles
 * - Need predictable performance
 * - Game entities, particles, network packets
 * 
 * Performance:
 * - 50-100x faster than malloc/free
 * - No system calls
 * - No searching for free blocks
 */
