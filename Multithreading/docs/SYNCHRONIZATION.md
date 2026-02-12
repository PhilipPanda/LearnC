# Synchronization

## The Big Picture

Multiple threads accessing shared data creates problems. Two threads increment a counter simultaneously - one update gets lost. Thread A reads while Thread B writes - reads garbage. Thread A assumes exclusive access - Thread B violates that assumption - corruption.

Synchronization primitives solve this. Mutexes provide exclusive access. Condition variables coordinate threads. Atomic operations guarantee safety.

## The Problem: Race Conditions

A race condition occurs when outcome depends on timing of thread execution.

```c
int counter = 0;  // Shared

// Thread 1                 // Thread 2
counter++;                  counter++;
```

Looks simple. Actually three operations:
1. Read counter
2. Add 1
3. Write counter back

**Timeline:**
```
Thread 1: Read (0) → Add 1 → Write (1)
Thread 2:     Read (0) → Add 1 → Write (1)
Result: 1 (should be 2!)
```

Both read 0 before either writes. One update lost.

Run 1000 times with 2 threads = expect 2000, get ~1500 (varies!).

## Solution: Mutex (Mutual Exclusion)

Only one thread can hold the lock at a time.

### Windows Mutex

```c
#include <windows.h>

CRITICAL_SECTION mutex;
int counter = 0;

DWORD WINAPI thread_func(LPVOID arg) {
    for (int i = 0; i < 1000; i++) {
        EnterCriticalSection(&mutex);  // Lock
        counter++;
        LeaveCriticalSection(&mutex);  // Unlock
    }
    return 0;
}

int main() {
    InitializeCriticalSection(&mutex);
    
    HANDLE threads[2];
    threads[0] = CreateThread(NULL, 0, thread_func, NULL, 0, NULL);
    threads[1] = CreateThread(NULL, 0, thread_func, NULL, 0, NULL);
    
    WaitForMultipleObjects(2, threads, TRUE, INFINITE);
    
    printf("Counter: %d\n", counter);  // Now correctly 2000!
    
    DeleteCriticalSection(&mutex);
    CloseHandle(threads[0]);
    CloseHandle(threads[1]);
}
```

### Linux (pthreads)

```c
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int counter = 0;

void* thread_func(void* arg) {
    for (int i = 0; i < 1000; i++) {
        pthread_mutex_lock(&mutex);    // Lock
        counter++;
        pthread_mutex_unlock(&mutex);  // Unlock
    }
    return NULL;
}

int main() {
    pthread_t threads[2];
    
    pthread_create(&threads[0], NULL, thread_func, NULL);
    pthread_create(&threads[1], NULL, thread_func, NULL);
    
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    
    printf("Counter: %d\n", counter);  // Correctly 2000!
    
    pthread_mutex_destroy(&mutex);
}
```

## How Mutexes Work

```
Thread 1: Lock() → counter++ → Unlock()
Thread 2:              (waiting...)      → Lock() → counter++ → Unlock()
```

When locked, other threads wait. Only one thread in critical section at a time.

## Critical Section

Code that accesses shared data:

```c
// Critical section - only one thread at a time
pthread_mutex_lock(&mutex);
shared_data->value++;
shared_data->sum += shared_data->value;
pthread_mutex_unlock(&mutex);
```

**Rules:**
- Keep critical sections small (fast)
- Don't call blocking operations while locked
- Always unlock (even on errors)

## Condition Variables

Threads waiting for a condition:

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int data_ready = 0;

// Producer
void* producer(void* arg) {
    pthread_mutex_lock(&mutex);
    
    // Produce data
    data_ready = 1;
    
    pthread_cond_signal(&cond);  // Wake up waiting thread
    pthread_mutex_unlock(&mutex);
    return NULL;
}

// Consumer
void* consumer(void* arg) {
    pthread_mutex_lock(&mutex);
    
    // Wait for data
    while (!data_ready) {
        pthread_cond_wait(&cond, &mutex);  // Unlocks mutex while waiting
    }
    
    // Process data
    printf("Data is ready!\n");
    
    pthread_mutex_unlock(&mutex);
    return NULL;
}
```

**Why while loop?** Spurious wakeups - thread can wake up even if not signaled.

## Atomic Operations

For simple operations, use atomics (faster than mutex):

```c
// C11
#include <stdatomic.h>

atomic_int counter = 0;

void* thread_func(void* arg) {
    for (int i = 0; i < 1000; i++) {
        atomic_fetch_add(&counter, 1);  // Atomic increment
    }
    return NULL;
}
```

**Pros:** Much faster than mutex  
**Cons:** Only for simple operations

## Read-Write Locks

Many readers, one writer:

```c
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
int shared_data = 0;

// Reader (multiple allowed)
void* reader(void* arg) {
    pthread_rwlock_rdlock(&rwlock);
    printf("Read: %d\n", shared_data);
    pthread_rwlock_unlock(&rwlock);
    return NULL;
}

// Writer (exclusive)
void* writer(void* arg) {
    pthread_rwlock_wrlock(&rwlock);
    shared_data++;
    pthread_rwlock_unlock(&rwlock);
    return NULL;
}
```

**Use when:** Many reads, few writes. Readers don't block each other.

## Semaphores

Counter-based synchronization:

```c
#include <semaphore.h>

sem_t semaphore;
sem_init(&semaphore, 0, 3);  // Allow 3 threads at once

void* worker(void* arg) {
    sem_wait(&semaphore);     // Decrement, wait if 0
    
    // Critical section (max 3 threads here)
    printf("Working...\n");
    
    sem_post(&semaphore);     // Increment
    return NULL;
}
```

**Use for:** Limiting concurrent access (connection pool, resource limit).

## Lock Granularity

### Coarse-Grained

One lock for everything:

```c
pthread_mutex_t big_lock;

void modify_a() {
    pthread_mutex_lock(&big_lock);
    data_a++;
    pthread_mutex_unlock(&big_lock);
}

void modify_b() {
    pthread_mutex_lock(&big_lock);
    data_b++;
    pthread_mutex_unlock(&big_lock);
}
```

**Pros:** Simple, no deadlocks  
**Cons:** Less parallelism (serializes unrelated operations)

### Fine-Grained

Separate locks for each resource:

```c
pthread_mutex_t lock_a;
pthread_mutex_t lock_b;

void modify_a() {
    pthread_mutex_lock(&lock_a);
    data_a++;
    pthread_mutex_unlock(&lock_a);
}

void modify_b() {
    pthread_mutex_lock(&lock_b);
    data_b++;
    pthread_mutex_unlock(&lock_b);
}
```

**Pros:** More parallelism  
**Cons:** More complex, risk of deadlocks

## Performance Impact

Locking has cost:

```c
// No lock: ~5 cycles per increment
counter++;

// With mutex: ~100 cycles per increment
pthread_mutex_lock(&mutex);
counter++;
pthread_mutex_unlock(&mutex);

// With atomic: ~20 cycles per increment
atomic_fetch_add(&counter, 1);
```

**Lesson:** Don't lock unnecessarily!

## Common Patterns

### Lock Guard (RAII-style)

```c
#define LOCK_GUARD(mutex) \
    for (int _i = (pthread_mutex_lock(mutex), 0); \
         _i < 1; \
         _i++, pthread_mutex_unlock(mutex))

// Usage
LOCK_GUARD(&mutex) {
    // Critical section
    counter++;
    // Auto unlock at end of block
}
```

### Double-Checked Locking

```c
static int initialized = 0;
static Data* data = NULL;

Data* get_data() {
    if (!initialized) {  // First check without lock (fast path)
        pthread_mutex_lock(&mutex);
        if (!initialized) {  // Second check with lock
            data = create_data();
            initialized = 1;
        }
        pthread_mutex_unlock(&mutex);
    }
    return data;
}
```

Optimize for common case (already initialized).

## Summary

Race conditions happen when threads access shared data without synchronization. Mutexes provide exclusive access - only one thread at a time. Condition variables coordinate waiting threads. Atomics work for simple operations.

Key principles:
- Identify shared data
- Protect with mutex
- Keep critical sections small
- Always unlock (even on errors)
- Use atomics when possible

Threading is powerful but requires discipline. One unsynchronized access = hard-to-debug bugs.
