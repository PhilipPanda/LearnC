/*
 * Race Condition Example
 * 
 * Shows what happens when multiple threads access shared data
 * without synchronization. Run this multiple times and watch
 * the counter give different (wrong) results each time.
 */

#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
    #include <windows.h>
    #define THREAD_FUNC DWORD WINAPI
    #define THREAD_RETURN return 0
    typedef HANDLE thread_t;
#else
    #include <pthread.h>
    #define THREAD_FUNC void*
    #define THREAD_RETURN return NULL
    typedef pthread_t thread_t;
#endif

#define NUM_THREADS 4
#define INCREMENTS_PER_THREAD 250000

// Shared counter (DANGER ZONE!)
volatile int counter = 0;

// Thread function that increments counter
THREAD_FUNC increment_thread(void* arg) {
    int id = *(int*)arg;
    
    printf("Thread %d starting increments...\n", id);
    
    for (int i = 0; i < INCREMENTS_PER_THREAD; i++) {
        // THIS IS NOT ATOMIC!
        // Actually three operations:
        // 1. Read counter
        // 2. Add 1
        // 3. Write back
        counter++;
    }
    
    printf("Thread %d finished\n", id);
    THREAD_RETURN;
}

int main(void) {
    printf("=== Race Condition Demonstration ===\n\n");
    
    printf("We'll start %d threads, each incrementing a counter %d times.\n",
           NUM_THREADS, INCREMENTS_PER_THREAD);
    printf("Expected final value: %d\n", NUM_THREADS * INCREMENTS_PER_THREAD);
    printf("Let's see what we actually get...\n\n");
    
    #ifdef _WIN32
        HANDLE threads[NUM_THREADS];
        int thread_ids[NUM_THREADS];
        
        // Create threads
        for (int i = 0; i < NUM_THREADS; i++) {
            thread_ids[i] = i;
            threads[i] = CreateThread(NULL, 0, increment_thread, &thread_ids[i], 0, NULL);
            if (threads[i] == NULL) {
                printf("Failed to create thread %d\n", i);
                return 1;
            }
        }
        
        // Wait for all threads
        WaitForMultipleObjects(NUM_THREADS, threads, TRUE, INFINITE);
        
        // Cleanup
        for (int i = 0; i < NUM_THREADS; i++) {
            CloseHandle(threads[i]);
        }
    #else
        pthread_t threads[NUM_THREADS];
        int thread_ids[NUM_THREADS];
        
        // Create threads
        for (int i = 0; i < NUM_THREADS; i++) {
            thread_ids[i] = i;
            if (pthread_create(&threads[i], NULL, increment_thread, &thread_ids[i]) != 0) {
                printf("Failed to create thread %d\n", i);
                return 1;
            }
        }
        
        // Wait for all threads
        for (int i = 0; i < NUM_THREADS; i++) {
            pthread_join(threads[i], NULL);
        }
    #endif
    
    printf("\n=== Results ===\n");
    printf("Expected: %d\n", NUM_THREADS * INCREMENTS_PER_THREAD);
    printf("Actual:   %d\n", counter);
    
    int lost_updates = (NUM_THREADS * INCREMENTS_PER_THREAD) - counter;
    printf("Lost updates: %d (%.2f%%)\n", 
           lost_updates,
           (lost_updates * 100.0) / (NUM_THREADS * INCREMENTS_PER_THREAD));
    
    printf("\n=== What happened? ===\n");
    printf("Multiple threads read the same value, incremented it,\n");
    printf("and wrote it back. Some updates got overwritten!\n\n");
    
    printf("Timeline example:\n");
    printf("  Thread 1: Read(0) -> Add 1 -> Write(1)\n");
    printf("  Thread 2:     Read(0) -> Add 1 -> Write(1)\n");
    printf("  Result: 1 (should be 2!)\n\n");
    
    printf("Run this program multiple times. You'll get different results\n");
    printf("each time because the race condition is timing-dependent.\n\n");
    
    printf("Next: See 03_mutex.c for how to fix this with synchronization.\n");
    
    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}
