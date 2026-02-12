# Common Threading Patterns

## The Big Picture

Most concurrent programs follow established patterns. Don't reinvent the wheel - use proven approaches for common problems.

## Producer-Consumer Pattern

One or more threads produce data, others consume it. Use a queue to decouple them.

```c
typedef struct {
    int* buffer;
    int size;
    int count;
    int in;
    int out;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} BoundedQueue;

void* producer(void* arg) {
    BoundedQueue* queue = (BoundedQueue*)arg;
    
    for (int i = 0; i < 100; i++) {
        pthread_mutex_lock(&queue->mutex);
        
        // Wait if queue is full
        while (queue->count == queue->size) {
            pthread_cond_wait(&queue->not_full, &queue->mutex);
        }
        
        // Produce item
        queue->buffer[queue->in] = i;
        queue->in = (queue->in + 1) % queue->size;
        queue->count++;
        
        pthread_cond_signal(&queue->not_empty);  // Wake consumer
        pthread_mutex_unlock(&queue->mutex);
    }
    
    return NULL;
}

void* consumer(void* arg) {
    BoundedQueue* queue = (BoundedQueue*)arg;
    
    while (1) {
        pthread_mutex_lock(&queue->mutex);
        
        // Wait if queue is empty
        while (queue->count == 0) {
            pthread_cond_wait(&queue->not_empty, &queue->mutex);
        }
        
        // Consume item
        int item = queue->buffer[queue->out];
        queue->out = (queue->out + 1) % queue->size;
        queue->count--;
        
        pthread_cond_signal(&queue->not_full);  // Wake producer
        pthread_mutex_unlock(&queue->mutex);
        
        // Process item outside lock
        printf("Consumed: %d\n", item);
    }
    
    return NULL;
}
```

**Use for:**
- Network servers (accept thread + worker threads)
- Task queues
- Pipeline processing
- Buffering between fast/slow components

## Thread Pool Pattern

Reuse threads instead of creating/destroying them:

```c
typedef struct {
    void (*function)(void*);
    void* arg;
} Task;

typedef struct {
    pthread_t* threads;
    int thread_count;
    Task* queue;
    int queue_size;
    int queue_count;
    int queue_front;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int shutdown;
} ThreadPool;

void* worker_thread(void* arg) {
    ThreadPool* pool = (ThreadPool*)arg;
    
    while (1) {
        pthread_mutex_lock(&pool->mutex);
        
        // Wait for work or shutdown
        while (pool->queue_count == 0 && !pool->shutdown) {
            pthread_cond_wait(&pool->cond, &pool->mutex);
        }
        
        if (pool->shutdown) {
            pthread_mutex_unlock(&pool->mutex);
            break;
        }
        
        // Get task
        Task task = pool->queue[pool->queue_front];
        pool->queue_front = (pool->queue_front + 1) % pool->queue_size;
        pool->queue_count--;
        
        pthread_mutex_unlock(&pool->mutex);
        
        // Execute task (outside lock!)
        task.function(task.arg);
    }
    
    return NULL;
}

ThreadPool* pool_create(int num_threads) {
    ThreadPool* pool = (ThreadPool*)malloc(sizeof(ThreadPool));
    pool->threads = (pthread_t*)malloc(num_threads * sizeof(pthread_t));
    pool->thread_count = num_threads;
    pool->queue_size = 100;
    pool->queue = (Task*)malloc(pool->queue_size * sizeof(Task));
    pool->queue_count = 0;
    pool->queue_front = 0;
    pool->shutdown = 0;
    
    pthread_mutex_init(&pool->mutex, NULL);
    pthread_cond_init(&pool->cond, NULL);
    
    // Start worker threads
    for (int i = 0; i < num_threads; i++) {
        pthread_create(&pool->threads[i], NULL, worker_thread, pool);
    }
    
    return pool;
}

void pool_add_task(ThreadPool* pool, void (*function)(void*), void* arg) {
    pthread_mutex_lock(&pool->mutex);
    
    // Add to queue
    int rear = (pool->queue_front + pool->queue_count) % pool->queue_size;
    pool->queue[rear].function = function;
    pool->queue[rear].arg = arg;
    pool->queue_count++;
    
    pthread_cond_signal(&pool->cond);  // Wake a worker
    pthread_mutex_unlock(&pool->mutex);
}
```

**Use for:** Web servers, image processing, batch jobs.

## Reader-Writer Pattern

Many readers, one writer at a time:

```c
pthread_rwlock_t rwlock;
int shared_data = 0;

void* reader(void* arg) {
    pthread_rwlock_rdlock(&rwlock);  // Shared read lock
    printf("Read: %d\n", shared_data);
    pthread_rwlock_unlock(&rwlock);
    return NULL;
}

void* writer(void* arg) {
    pthread_rwlock_wrlock(&rwlock);  // Exclusive write lock
    shared_data++;
    printf("Wrote: %d\n", shared_data);
    pthread_rwlock_unlock(&rwlock);
    return NULL;
}
```

**Benefits:** Multiple readers can read simultaneously.

## Barrier Pattern

Wait for all threads to reach a point:

```c
pthread_barrier_t barrier;

void* worker(void* arg) {
    int id = *(int*)arg;
    
    // Phase 1
    printf("Thread %d: Phase 1\n", id);
    
    pthread_barrier_wait(&barrier);  // Wait for all threads
    
    // Phase 2 (all threads start together)
    printf("Thread %d: Phase 2\n", id);
    
    return NULL;
}

int main() {
    pthread_barrier_init(&barrier, NULL, 4);  // 4 threads
    
    pthread_t threads[4];
    int ids[4];
    
    for (int i = 0; i < 4; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, worker, &ids[i]);
    }
    
    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }
    
    pthread_barrier_destroy(&barrier);
}
```

**Use for:** Parallel algorithms with phases (simulations, matrix operations).

## Work Stealing

Idle threads steal work from busy threads:

```c
typedef struct {
    Task* queue;
    int front, rear, count;
    pthread_mutex_t mutex;
} WorkQueue;

WorkQueue queues[NUM_THREADS];

void* worker(void* arg) {
    int id = *(int*)arg;
    
    while (1) {
        Task task;
        int found = 0;
        
        // Try own queue first
        pthread_mutex_lock(&queues[id].mutex);
        if (queues[id].count > 0) {
            task = dequeue(&queues[id]);
            found = 1;
        }
        pthread_mutex_unlock(&queues[id].mutex);
        
        // Try stealing from others
        if (!found) {
            for (int i = 0; i < NUM_THREADS; i++) {
                if (i == id) continue;
                
                pthread_mutex_lock(&queues[i].mutex);
                if (queues[i].count > 0) {
                    task = dequeue(&queues[i]);
                    found = 1;
                    pthread_mutex_unlock(&queues[i].mutex);
                    break;
                }
                pthread_mutex_unlock(&queues[i].mutex);
            }
        }
        
        if (found) {
            execute_task(task);
        } else {
            break;  // No more work
        }
    }
    
    return NULL;
}
```

**Use for:** Load balancing, parallel algorithms.

## Pipeline Pattern

Data flows through stages, each in different thread:

```
Input → [Thread 1: Parse] → [Thread 2: Process] → [Thread 3: Output]
```

```c
Queue* stage1_to_stage2;
Queue* stage2_to_stage3;

void* stage1_parse(void* arg) {
    while (has_input()) {
        Data* data = parse_input();
        queue_push(stage1_to_stage2, data);
    }
    return NULL;
}

void* stage2_process(void* arg) {
    while (1) {
        Data* data = queue_pop(stage1_to_stage2);
        if (data == NULL) break;
        
        process(data);
        queue_push(stage2_to_stage3, data);
    }
    return NULL;
}

void* stage3_output(void* arg) {
    while (1) {
        Data* data = queue_pop(stage2_to_stage3);
        if (data == NULL) break;
        
        output(data);
        free(data);
    }
    return NULL;
}
```

**Use for:** Stream processing, video encoding, data transformation.

## Fork-Join Pattern

Parallelize a loop, then wait for all to finish:

```c
void process_item(int item) {
    // Expensive computation
}

int main() {
    int items[1000];
    
    // Fork: Create threads for parallel work
    pthread_t threads[4];
    for (int i = 0; i < 4; i++) {
        // Each thread processes 250 items
        pthread_create(&threads[i], NULL, worker, &items[i * 250]);
    }
    
    // Join: Wait for all to complete
    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("All items processed\n");
}
```

**Use for:** Parallel loops, map-reduce, batch processing.

## Lock-Free Data Structures

Use atomic operations instead of locks:

```c
typedef struct Node {
    int data;
    struct Node* next;
} Node;

typedef struct {
    atomic_ptr head;
} LockFreeStack;

void push(LockFreeStack* stack, Node* node) {
    Node* old_head;
    do {
        old_head = atomic_load(&stack->head);
        node->next = old_head;
    } while (!atomic_compare_exchange_weak(&stack->head, &old_head, node));
}
```

**Pros:** No blocking, faster  
**Cons:** Complex, tricky to get right

**When to use:** High-contention scenarios, real-time systems.

## Best Practices

**1. Minimize shared state:**
```c
// Bad: Everything shared
int global_counter = 0;

// Good: Thread-local, combine at end
void* worker(void* arg) {
    int local_counter = 0;
    // Do work, update local
    
    // One synchronized update at end
    pthread_mutex_lock(&mutex);
    global_counter += local_counter;
    pthread_mutex_unlock(&mutex);
}
```

**2. Lock ordering:**
```c
// Always acquire locks in same order
void transfer(Account* from, Account* to, int amount) {
    // Lock in consistent order (by address)
    Account* first = (from < to) ? from : to;
    Account* second = (from < to) ? to : from;
    
    pthread_mutex_lock(&first->mutex);
    pthread_mutex_lock(&second->mutex);
    
    from->balance -= amount;
    to->balance += amount;
    
    pthread_mutex_unlock(&second->mutex);
    pthread_mutex_unlock(&first->mutex);
}
```

**3. Document thread safety:**
```c
// Thread-safe: Can be called from multiple threads
int queue_push(Queue* q, int item);

// NOT thread-safe: Caller must synchronize
void internal_helper(Queue* q);
```

## Summary

Common patterns solve common problems:
- Producer-consumer: Decouple producers and consumers
- Thread pool: Reuse threads for many tasks
- Pipeline: Process data in stages
- Fork-join: Parallelize loops
- Reader-writer: Many reads, few writes

Choose the right pattern for your problem. Don't roll your own unless necessary.
