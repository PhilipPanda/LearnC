# Thread Basics

## The Big Picture

Your program normally runs on one core. One thread of execution, doing one thing at a time. Threads let you run multiple things simultaneously on multiple cores.

Parse a file while downloading another. Render while simulating physics. Process multiple requests at once. Threads unlock parallelism.

But with power comes complexity. Multiple threads accessing the same data creates race conditions, deadlocks, and subtle bugs. Master the basics first.

## What is a Thread?

A thread is an independent sequence of execution within a process. Multiple threads share:
- Memory (global variables, heap)
- File descriptors
- Process state

Each thread has its own:
- Stack
- Program counter
- Registers

```
Process:
┌─────────────────────────┐
│ Code                    │ ← Shared
├─────────────────────────┤
│ Global Data             │ ← Shared
├─────────────────────────┤
│ Heap                    │ ← Shared
├─────────────────────────┤
│ Thread 1 Stack          │ ← Private
├─────────────────────────┤
│ Thread 2 Stack          │ ← Private
├─────────────────────────┤
│ Thread 3 Stack          │ ← Private
└─────────────────────────┘
```

## Creating Threads

### Windows

```c
#include <windows.h>

DWORD WINAPI thread_function(LPVOID arg) {
    int* data = (int*)arg;
    printf("Thread running with value: %d\n", *data);
    return 0;
}

int main() {
    int value = 42;
    
    HANDLE thread = CreateThread(
        NULL,              // Security attributes
        0,                 // Stack size (0 = default)
        thread_function,   // Function to run
        &value,            // Argument to pass
        0,                 // Creation flags
        NULL               // Thread ID
    );
    
    if (thread == NULL) {
        printf("Thread creation failed\n");
        return 1;
    }
    
    // Wait for thread to finish
    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);
    
    return 0;
}
```

### Linux (pthreads)

```c
#include <pthread.h>
#include <stdio.h>

void* thread_function(void* arg) {
    int* data = (int*)arg;
    printf("Thread running with value: %d\n", *data);
    return NULL;
}

int main() {
    pthread_t thread;
    int value = 42;
    
    // Create thread
    if (pthread_create(&thread, NULL, thread_function, &value) != 0) {
        printf("Thread creation failed\n");
        return 1;
    }
    
    // Wait for thread to finish
    pthread_join(thread, NULL);
    
    return 0;
}
```

Compile with: `gcc program.c -pthread`

## Thread Lifecycle

```
Created → Running → Finished
   ↓
Joined (resources cleaned up)
```

**Detached thread:** Cleans up automatically when finished (can't join).

```c
// Windows
CreateThread(...);  // Don't save handle, don't wait

// Linux
pthread_t thread;
pthread_create(&thread, NULL, func, NULL);
pthread_detach(thread);  // Auto cleanup
```

## Passing Data to Threads

### Simple Data

```c
void* thread_func(void* arg) {
    int value = *(int*)arg;
    printf("Received: %d\n", value);
    return NULL;
}

int main() {
    int data = 42;
    pthread_t thread;
    pthread_create(&thread, NULL, thread_func, &data);
    pthread_join(thread, NULL);
}
```

**Warning:** Make sure data lives until thread finishes!

```c
// BAD: data goes out of scope
void spawn_thread() {
    int data = 42;
    pthread_create(&thread, NULL, func, &data);
    // Returns immediately, data destroyed!
}

// GOOD: data on heap or in main
int* data = (int*)malloc(sizeof(int));
*data = 42;
pthread_create(&thread, NULL, func, data);
```

### Multiple Arguments

Use a struct:

```c
typedef struct {
    int id;
    char name[50];
    float value;
} ThreadData;

void* thread_func(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    printf("Thread %d: %s = %.2f\n", data->id, data->name, data->value);
    free(data);  // Thread frees its own data
    return NULL;
}

int main() {
    for (int i = 0; i < 5; i++) {
        ThreadData* data = (ThreadData*)malloc(sizeof(ThreadData));
        data->id = i;
        sprintf(data->name, "Thread_%d", i);
        data->value = i * 1.5f;
        
        pthread_t thread;
        pthread_create(&thread, NULL, thread_func, data);
        pthread_detach(thread);  // Auto cleanup
    }
    
    sleep(1);  // Wait for threads to finish
}
```

## Return Values

```c
void* thread_func(void* arg) {
    int* result = (int*)malloc(sizeof(int));
    *result = 42;
    return result;
}

int main() {
    pthread_t thread;
    pthread_create(&thread, NULL, thread_func, NULL);
    
    void* ret;
    pthread_join(thread, &ret);
    
    int result = *(int*)ret;
    printf("Thread returned: %d\n", result);
    free(ret);
}
```

## Multiple Threads

```c
#define NUM_THREADS 4

void* worker(void* arg) {
    int id = *(int*)arg;
    printf("Worker %d starting\n", id);
    // Do work...
    printf("Worker %d done\n", id);
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];
    
    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, worker, &thread_ids[i]);
    }
    
    // Wait for all
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("All workers finished\n");
}
```

## Thread IDs

```c
// Windows
DWORD tid = GetCurrentThreadId();
printf("My thread ID: %lu\n", tid);

// Linux
pthread_t tid = pthread_self();
printf("My thread ID: %lu\n", (unsigned long)tid);
```

## Thread Local Storage

Each thread gets its own copy:

```c
// Windows
__declspec(thread) int thread_local_var = 0;

// Linux
__thread int thread_local_var = 0;

// C11 standard
_Thread_local int thread_local_var = 0;

void* thread_func(void* arg) {
    thread_local_var = *(int*)arg;
    printf("My local var: %d\n", thread_local_var);
    // Other threads don't see this value
    return NULL;
}
```

## When to Use Threads

**Good reasons:**
- CPU-bound parallel work (process images, run simulations)
- I/O concurrency (handle multiple clients)
- Responsive UI (background loading)
- Multi-core utilization

**Bad reasons:**
- Making code "faster" without measuring
- Simple sequential tasks
- When single-threaded is fast enough

**Rule:** Profile first, thread later.

## How Many Threads?

```c
#include <windows.h>

// Get CPU core count
int get_core_count() {
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return info.dwNumberOfProcessors;
}

// Linux
#include <unistd.h>
int cores = sysconf(_SC_NPROCESSORS_ONLN);
```

**Rule of thumb:**
- CPU-bound work: cores or cores - 1
- I/O-bound work: 2-4x cores

## Thread vs Process

| Feature | Thread | Process |
|---------|--------|---------|
| Memory | Shared | Isolated |
| Creation | Fast | Slow |
| Communication | Easy (shared mem) | Hard (IPC) |
| Crash impact | Crashes whole process | Isolated |
| Overhead | Low | High |

Use threads for parallel work within one program. Use processes for isolation.

## Common Mistakes

**1. Not joining threads:**
```c
// BAD: Thread might still be running
pthread_create(&thread, NULL, func, NULL);
// Program exits immediately!

// GOOD: Wait for completion
pthread_create(&thread, NULL, func, NULL);
pthread_join(thread, NULL);
```

**2. Passing stack variables:**
```c
// BAD
for (int i = 0; i < 10; i++) {
    pthread_create(&threads[i], NULL, func, &i);
    // All threads see same i!
}

// GOOD
int* ids = (int*)malloc(10 * sizeof(int));
for (int i = 0; i < 10; i++) {
    ids[i] = i;
    pthread_create(&threads[i], NULL, func, &ids[i]);
}
```

**3. Accessing shared data without synchronization:**
```c
// BAD: Race condition!
int counter = 0;

void* thread_func(void* arg) {
    for (int i = 0; i < 1000; i++) {
        counter++;  // NOT ATOMIC!
    }
    return NULL;
}
```

We'll fix this with mutexes in the next section.

## Performance Considerations

**Thread creation is expensive:**
```c
// Slow: Create thread per task
for (int i = 0; i < 1000; i++) {
    pthread_create(&thread, NULL, process_item, &items[i]);
    pthread_join(thread, NULL);
}

// Fast: Thread pool with work queue
ThreadPool* pool = pool_create(4);
for (int i = 0; i < 1000; i++) {
    pool_add_work(pool, process_item, &items[i]);
}
pool_wait(pool);
```

**Context switching has overhead:**
- Creating 100 threads on 4 cores = lots of switching
- Better: 4 worker threads, queue of tasks

## Summary

Threads enable parallelism. Create with `CreateThread` (Windows) or `pthread_create` (Linux). Always join or detach. Pass data carefully - watch lifetime and scope.

Shared memory makes communication easy but introduces race conditions. Next: synchronization to make it safe.
