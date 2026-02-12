/*
 * Mutex Example
 * 
 * Shows how to fix race conditions using mutual exclusion (mutex).
 * Only one thread can hold the lock at a time.
 */

#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
    #include <windows.h>
    #define THREAD_FUNC DWORD WINAPI
    #define THREAD_RETURN return 0
    typedef HANDLE thread_t;
    typedef CRITICAL_SECTION mutex_t;
    typedef DWORD (WINAPI *thread_func_t)(void*);
    
    void mutex_init(mutex_t* m) {
        InitializeCriticalSection(m);
    }
    
    void mutex_lock(mutex_t* m) {
        EnterCriticalSection(m);
    }
    
    void mutex_unlock(mutex_t* m) {
        LeaveCriticalSection(m);
    }
    
    void mutex_destroy(mutex_t* m) {
        DeleteCriticalSection(m);
    }
#else
    #include <pthread.h>
    #define THREAD_FUNC void*
    #define THREAD_RETURN return NULL
    typedef pthread_t thread_t;
    typedef pthread_mutex_t mutex_t;
    typedef void* (*thread_func_t)(void*);
    
    void mutex_init(mutex_t* m) {
        pthread_mutex_init(m, NULL);
    }
    
    void mutex_lock(mutex_t* m) {
        pthread_mutex_lock(m);
    }
    
    void mutex_unlock(mutex_t* m) {
        pthread_mutex_unlock(m);
    }
    
    void mutex_destroy(mutex_t* m) {
        pthread_mutex_destroy(m);
    }
#endif

#define NUM_THREADS 4
#define INCREMENTS_PER_THREAD 250000

// Shared data
int counter = 0;
mutex_t counter_mutex;

// Thread function that safely increments counter
THREAD_FUNC safe_increment_thread(void* arg) {
    int id = *(int*)arg;
    
    printf("Thread %d starting increments...\n", id);
    
    for (int i = 0; i < INCREMENTS_PER_THREAD; i++) {
        mutex_lock(&counter_mutex);      // Lock
        counter++;                        // Critical section
        mutex_unlock(&counter_mutex);    // Unlock
    }
    
    printf("Thread %d finished\n", id);
    THREAD_RETURN;
}

// Optimized version: lock once per batch
THREAD_FUNC optimized_thread(void* arg) {
    int id = *(int*)arg;
    
    printf("Thread %d starting (optimized)...\n", id);
    
    // Accumulate locally
    int local_sum = INCREMENTS_PER_THREAD;
    
    // One lock at the end
    mutex_lock(&counter_mutex);
    counter += local_sum;
    mutex_unlock(&counter_mutex);
    
    printf("Thread %d finished\n", id);
    THREAD_RETURN;
}

void run_test(const char* description, thread_func_t thread_func) {
    printf("\n=== %s ===\n", description);
    
    counter = 0;
    
    #ifdef _WIN32
        HANDLE threads[NUM_THREADS];
        int thread_ids[NUM_THREADS];
        
        for (int i = 0; i < NUM_THREADS; i++) {
            thread_ids[i] = i;
            threads[i] = CreateThread(NULL, 0, thread_func, &thread_ids[i], 0, NULL);
        }
        
        WaitForMultipleObjects(NUM_THREADS, threads, TRUE, INFINITE);
        
        for (int i = 0; i < NUM_THREADS; i++) {
            CloseHandle(threads[i]);
        }
    #else
        pthread_t threads[NUM_THREADS];
        int thread_ids[NUM_THREADS];
        
        for (int i = 0; i < NUM_THREADS; i++) {
            thread_ids[i] = i;
            pthread_create(&threads[i], NULL, thread_func, &thread_ids[i]);
        }
        
        for (int i = 0; i < NUM_THREADS; i++) {
            pthread_join(threads[i], NULL);
        }
    #endif
    
    printf("\nExpected: %d\n", NUM_THREADS * INCREMENTS_PER_THREAD);
    printf("Actual:   %d\n", counter);
    
    if (counter == NUM_THREADS * INCREMENTS_PER_THREAD) {
        printf("✓ Correct! No race condition.\n");
    } else {
        printf("✗ Wrong! Race condition detected.\n");
    }
}

int main(void) {
    printf("=== Mutex Example ===\n\n");
    
    printf("This demonstrates how mutexes fix race conditions.\n");
    printf("We'll run %d threads, each incrementing %d times.\n",
           NUM_THREADS, INCREMENTS_PER_THREAD);
    printf("Expected result: %d\n", NUM_THREADS * INCREMENTS_PER_THREAD);
    
    mutex_init(&counter_mutex);
    
    // Test 1: Lock per increment (safe but slow)
    run_test("Test 1: Lock per increment", safe_increment_thread);
    
    // Test 2: Optimized - accumulate locally, lock once (safe and fast)
    run_test("Test 2: Optimized (local accumulation)", optimized_thread);
    
    mutex_destroy(&counter_mutex);
    
    printf("\n=== How it works ===\n");
    printf("Mutex ensures only one thread at a time enters the critical section:\n\n");
    printf("  Thread 1: Lock() -> counter++ -> Unlock()\n");
    printf("  Thread 2:              (waiting...)         -> Lock() -> counter++ -> Unlock()\n\n");
    
    printf("Timeline:\n");
    printf("  Thread 1: Lock ✓ -> Read(0) -> Add 1 -> Write(1) -> Unlock\n");
    printf("  Thread 2: Lock ⏰ (waiting for Thread 1)              -> Lock ✓ -> Read(1) -> Add 1 -> Write(2) -> Unlock\n");
    printf("  Result: 2 (correct!)\n\n");
    
    printf("=== Performance tip ===\n");
    printf("Locking has overhead. The optimized version is faster because:\n");
    printf("1. Each thread accumulates locally (no lock needed)\n");
    printf("2. Lock once at the end to update shared counter\n");
    printf("This reduces lock contention and improves performance.\n");
    
    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}
