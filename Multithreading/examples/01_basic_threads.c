/*
 * Basic Threads Example
 * 
 * Shows:
 * - Creating threads
 * - Passing arguments
 * - Joining threads
 * - Thread IDs
 */

#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
    #include <windows.h>
    #define THREAD_FUNC DWORD WINAPI
    #define THREAD_RETURN return 0
    typedef HANDLE thread_t;
    typedef DWORD thread_id_t;
    
    thread_id_t get_thread_id() {
        return GetCurrentThreadId();
    }
    
    void thread_sleep(int ms) {
        Sleep(ms);
    }
#else
    #include <pthread.h>
    #include <unistd.h>
    #define THREAD_FUNC void*
    #define THREAD_RETURN return NULL
    typedef pthread_t thread_t;
    typedef unsigned long thread_id_t;
    
    thread_id_t get_thread_id() {
        return (unsigned long)pthread_self();
    }
    
    void thread_sleep(int ms) {
        usleep(ms * 1000);
    }
#endif

// Thread function that just prints a message
THREAD_FUNC simple_thread(void* arg) {
    int id = *(int*)arg;
    printf("[Thread %lu] Hello from thread %d\n", get_thread_id(), id);
    thread_sleep(100);
    printf("[Thread %lu] Thread %d finishing\n", get_thread_id(), id);
    THREAD_RETURN;
}

// Thread function that does some work
THREAD_FUNC worker_thread(void* arg) {
    int* data = (int*)arg;
    printf("[Thread %lu] Worker starting with value: %d\n", get_thread_id(), *data);
    
    // Simulate work
    int sum = 0;
    for (int i = 0; i < *data; i++) {
        sum += i;
        if (i % 1000000 == 0) {
            thread_sleep(10);
        }
    }
    
    printf("[Thread %lu] Worker computed sum: %d\n", get_thread_id(), sum);
    THREAD_RETURN;
}

// Thread function that returns a value
THREAD_FUNC compute_thread(void* arg) {
    int n = *(int*)arg;
    printf("[Thread %lu] Computing factorial of %d\n", get_thread_id(), n);
    
    int result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    
    int* return_value = (int*)malloc(sizeof(int));
    *return_value = result;
    
    printf("[Thread %lu] Factorial result: %d\n", get_thread_id(), result);
    
    #ifdef _WIN32
        return result;
    #else
        return return_value;
    #endif
}

int main(void) {
    printf("=== Basic Threads Example ===\n\n");
    
    // Example 1: Simple threads
    printf("1. Creating simple threads:\n");
    {
        #ifdef _WIN32
            HANDLE threads[3];
            int ids[3] = {1, 2, 3};
            
            for (int i = 0; i < 3; i++) {
                threads[i] = CreateThread(NULL, 0, simple_thread, &ids[i], 0, NULL);
                if (threads[i] == NULL) {
                    printf("Failed to create thread %d\n", i);
                    return 1;
                }
            }
            
            WaitForMultipleObjects(3, threads, TRUE, INFINITE);
            
            for (int i = 0; i < 3; i++) {
                CloseHandle(threads[i]);
            }
        #else
            pthread_t threads[3];
            int ids[3] = {1, 2, 3};
            
            for (int i = 0; i < 3; i++) {
                if (pthread_create(&threads[i], NULL, simple_thread, &ids[i]) != 0) {
                    printf("Failed to create thread %d\n", i);
                    return 1;
                }
            }
            
            for (int i = 0; i < 3; i++) {
                pthread_join(threads[i], NULL);
            }
        #endif
    }
    printf("\n");
    
    // Example 2: Worker threads with different workloads
    printf("2. Worker threads with different data:\n");
    {
        #ifdef _WIN32
            HANDLE threads[3];
            int workloads[3] = {1000000, 2000000, 500000};
            
            for (int i = 0; i < 3; i++) {
                threads[i] = CreateThread(NULL, 0, worker_thread, &workloads[i], 0, NULL);
            }
            
            WaitForMultipleObjects(3, threads, TRUE, INFINITE);
            
            for (int i = 0; i < 3; i++) {
                CloseHandle(threads[i]);
            }
        #else
            pthread_t threads[3];
            int workloads[3] = {1000000, 2000000, 500000};
            
            for (int i = 0; i < 3; i++) {
                pthread_create(&threads[i], NULL, worker_thread, &workloads[i]);
            }
            
            for (int i = 0; i < 3; i++) {
                pthread_join(threads[i], NULL);
            }
        #endif
    }
    printf("\n");
    
    // Example 3: Getting return values from threads
    printf("3. Thread return values:\n");
    {
        #ifdef _WIN32
            HANDLE thread;
            int n = 10;
            
            thread = CreateThread(NULL, 0, compute_thread, &n, 0, NULL);
            
            DWORD result;
            WaitForSingleObject(thread, INFINITE);
            GetExitCodeThread(thread, &result);
            
            printf("[Main] Received result from thread: %lu\n", result);
            CloseHandle(thread);
        #else
            pthread_t thread;
            int n = 10;
            
            pthread_create(&thread, NULL, compute_thread, &n);
            
            void* result;
            pthread_join(thread, &result);
            
            printf("[Main] Received result from thread: %d\n", *(int*)result);
            free(result);
        #endif
    }
    printf("\n");
    
    printf("=== All threads completed successfully ===\n");
    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}
