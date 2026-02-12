/*
 * Producer-Consumer Pattern
 * 
 * Classic concurrent pattern using a bounded buffer.
 * Producers create items, consumers process them.
 * Uses mutex and condition variables for coordination.
 */

#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
    #include <windows.h>
    #define THREAD_FUNC DWORD WINAPI
    #define THREAD_RETURN return 0
    typedef HANDLE thread_t;
    typedef CRITICAL_SECTION mutex_t;
    typedef CONDITION_VARIABLE cond_t;
    
    void mutex_init(mutex_t* m) { InitializeCriticalSection(m); }
    void mutex_lock(mutex_t* m) { EnterCriticalSection(m); }
    void mutex_unlock(mutex_t* m) { LeaveCriticalSection(m); }
    void mutex_destroy(mutex_t* m) { DeleteCriticalSection(m); }
    
    void cond_init(cond_t* c) { InitializeConditionVariable(c); }
    void cond_wait(cond_t* c, mutex_t* m) { SleepConditionVariableCS(c, m, INFINITE); }
    void cond_signal(cond_t* c) { WakeConditionVariable(c); }
    void cond_broadcast(cond_t* c) { WakeAllConditionVariable(c); }
    void cond_destroy(cond_t* c) { /* No cleanup needed */ }
    
    void thread_sleep(int ms) { Sleep(ms); }
#else
    #include <pthread.h>
    #include <unistd.h>
    #define THREAD_FUNC void*
    #define THREAD_RETURN return NULL
    typedef pthread_t thread_t;
    typedef pthread_mutex_t mutex_t;
    typedef pthread_cond_t cond_t;
    
    void mutex_init(mutex_t* m) { pthread_mutex_init(m, NULL); }
    void mutex_lock(mutex_t* m) { pthread_mutex_lock(m); }
    void mutex_unlock(mutex_t* m) { pthread_mutex_unlock(m); }
    void mutex_destroy(mutex_t* m) { pthread_mutex_destroy(m); }
    
    void cond_init(cond_t* c) { pthread_cond_init(c, NULL); }
    void cond_wait(cond_t* c, mutex_t* m) { pthread_cond_wait(c, m); }
    void cond_signal(cond_t* c) { pthread_cond_signal(c); }
    void cond_broadcast(cond_t* c) { pthread_cond_broadcast(c); }
    void cond_destroy(cond_t* c) { pthread_cond_destroy(c); }
    
    void thread_sleep(int ms) { usleep(ms * 1000); }
#endif

#define BUFFER_SIZE 10
#define NUM_ITEMS 20
#define NUM_PRODUCERS 2
#define NUM_CONSUMERS 3

// Bounded buffer (circular queue)
typedef struct {
    int buffer[BUFFER_SIZE];
    int count;
    int in;
    int out;
    mutex_t mutex;
    cond_t not_empty;
    cond_t not_full;
    int done;  // Signal completion
} BoundedBuffer;

BoundedBuffer queue;

void buffer_init(BoundedBuffer* buf) {
    buf->count = 0;
    buf->in = 0;
    buf->out = 0;
    buf->done = 0;
    mutex_init(&buf->mutex);
    cond_init(&buf->not_empty);
    cond_init(&buf->not_full);
}

void buffer_destroy(BoundedBuffer* buf) {
    mutex_destroy(&buf->mutex);
    cond_destroy(&buf->not_empty);
    cond_destroy(&buf->not_full);
}

void buffer_put(BoundedBuffer* buf, int item) {
    mutex_lock(&buf->mutex);
    
    // Wait while buffer is full
    while (buf->count == BUFFER_SIZE) {
        printf("  [Producer blocked - buffer full]\n");
        cond_wait(&buf->not_full, &buf->mutex);
    }
    
    // Add item
    buf->buffer[buf->in] = item;
    buf->in = (buf->in + 1) % BUFFER_SIZE;
    buf->count++;
    
    printf("  Produced: %d (buffer: %d/%d)\n", item, buf->count, BUFFER_SIZE);
    
    // Signal consumers
    cond_signal(&buf->not_empty);
    
    mutex_unlock(&buf->mutex);
}

int buffer_get(BoundedBuffer* buf) {
    mutex_lock(&buf->mutex);
    
    // Wait while buffer is empty and not done
    while (buf->count == 0 && !buf->done) {
        printf("  [Consumer blocked - buffer empty]\n");
        cond_wait(&buf->not_empty, &buf->mutex);
    }
    
    // Check if we're done and buffer is empty
    if (buf->count == 0 && buf->done) {
        mutex_unlock(&buf->mutex);
        return -1;  // Signal end
    }
    
    // Get item
    int item = buf->buffer[buf->out];
    buf->out = (buf->out + 1) % BUFFER_SIZE;
    buf->count--;
    
    printf("  Consumed: %d (buffer: %d/%d)\n", item, buf->count, BUFFER_SIZE);
    
    // Signal producers
    cond_signal(&buf->not_full);
    
    mutex_unlock(&buf->mutex);
    return item;
}

void buffer_set_done(BoundedBuffer* buf) {
    mutex_lock(&buf->mutex);
    buf->done = 1;
    cond_broadcast(&buf->not_empty);  // Wake all consumers
    mutex_unlock(&buf->mutex);
}

// Producer thread
THREAD_FUNC producer_thread(void* arg) {
    int id = *(int*)arg;
    
    printf("[Producer %d] Starting\n", id);
    
    for (int i = 0; i < NUM_ITEMS; i++) {
        int item = id * 1000 + i;
        
        printf("[Producer %d] Producing item %d...\n", id, item);
        buffer_put(&queue, item);
        
        // Simulate work
        thread_sleep(50 + (rand() % 100));
    }
    
    printf("[Producer %d] Finished\n", id);
    THREAD_RETURN;
}

// Consumer thread
THREAD_FUNC consumer_thread(void* arg) {
    int id = *(int*)arg;
    int consumed_count = 0;
    
    printf("[Consumer %d] Starting\n", id);
    
    while (1) {
        int item = buffer_get(&queue);
        
        if (item == -1) {
            break;  // Done
        }
        
        printf("[Consumer %d] Processing item %d...\n", id, item);
        consumed_count++;
        
        // Simulate processing
        thread_sleep(100 + (rand() % 150));
        
        printf("[Consumer %d] Finished processing item %d\n", id, item);
    }
    
    printf("[Consumer %d] Finished (processed %d items)\n", id, consumed_count);
    THREAD_RETURN;
}

int main(void) {
    printf("=== Producer-Consumer Pattern ===\n\n");
    
    printf("Configuration:\n");
    printf("  Buffer size: %d\n", BUFFER_SIZE);
    printf("  Producers: %d (each producing %d items)\n", NUM_PRODUCERS, NUM_ITEMS);
    printf("  Consumers: %d\n", NUM_CONSUMERS);
    printf("  Total items: %d\n\n", NUM_PRODUCERS * NUM_ITEMS);
    
    buffer_init(&queue);
    
    #ifdef _WIN32
        HANDLE producers[NUM_PRODUCERS];
        HANDLE consumers[NUM_CONSUMERS];
        int producer_ids[NUM_PRODUCERS];
        int consumer_ids[NUM_CONSUMERS];
        
        // Create producers
        for (int i = 0; i < NUM_PRODUCERS; i++) {
            producer_ids[i] = i;
            producers[i] = CreateThread(NULL, 0, producer_thread, &producer_ids[i], 0, NULL);
        }
        
        // Create consumers
        for (int i = 0; i < NUM_CONSUMERS; i++) {
            consumer_ids[i] = i;
            consumers[i] = CreateThread(NULL, 0, consumer_thread, &consumer_ids[i], 0, NULL);
        }
        
        // Wait for producers to finish
        WaitForMultipleObjects(NUM_PRODUCERS, producers, TRUE, INFINITE);
        printf("\n=== All producers finished ===\n\n");
        
        // Signal consumers that production is done
        buffer_set_done(&queue);
        
        // Wait for consumers to finish
        WaitForMultipleObjects(NUM_CONSUMERS, consumers, TRUE, INFINITE);
        
        // Cleanup
        for (int i = 0; i < NUM_PRODUCERS; i++) CloseHandle(producers[i]);
        for (int i = 0; i < NUM_CONSUMERS; i++) CloseHandle(consumers[i]);
    #else
        pthread_t producers[NUM_PRODUCERS];
        pthread_t consumers[NUM_CONSUMERS];
        int producer_ids[NUM_PRODUCERS];
        int consumer_ids[NUM_CONSUMERS];
        
        // Create producers
        for (int i = 0; i < NUM_PRODUCERS; i++) {
            producer_ids[i] = i;
            pthread_create(&producers[i], NULL, producer_thread, &producer_ids[i]);
        }
        
        // Create consumers
        for (int i = 0; i < NUM_CONSUMERS; i++) {
            consumer_ids[i] = i;
            pthread_create(&consumers[i], NULL, consumer_thread, &consumer_ids[i]);
        }
        
        // Wait for producers to finish
        for (int i = 0; i < NUM_PRODUCERS; i++) {
            pthread_join(producers[i], NULL);
        }
        printf("\n=== All producers finished ===\n\n");
        
        // Signal consumers that production is done
        buffer_set_done(&queue);
        
        // Wait for consumers to finish
        for (int i = 0; i < NUM_CONSUMERS; i++) {
            pthread_join(consumers[i], NULL);
        }
    #endif
    
    buffer_destroy(&queue);
    
    printf("\n=== Complete ===\n");
    printf("All items produced and consumed successfully!\n");
    
    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}
