/*
 * Deadlock Example
 * 
 * Shows what causes deadlocks and how to prevent them.
 * Demonstrates both the problem and the solution.
 */

#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
    #include <windows.h>
    #define THREAD_FUNC DWORD WINAPI
    #define THREAD_RETURN return 0
    typedef HANDLE thread_t;
    typedef CRITICAL_SECTION mutex_t;
    
    void mutex_init(mutex_t* m) { InitializeCriticalSection(m); }
    void mutex_lock(mutex_t* m) { EnterCriticalSection(m); }
    void mutex_unlock(mutex_t* m) { LeaveCriticalSection(m); }
    void mutex_destroy(mutex_t* m) { DeleteCriticalSection(m); }
    void thread_sleep(int ms) { Sleep(ms); }
#else
    #include <pthread.h>
    #include <unistd.h>
    #define THREAD_FUNC void*
    #define THREAD_RETURN return NULL
    typedef pthread_t thread_t;
    typedef pthread_mutex_t mutex_t;
    
    void mutex_init(mutex_t* m) { pthread_mutex_init(m, NULL); }
    void mutex_lock(mutex_t* m) { pthread_mutex_lock(m); }
    void mutex_unlock(mutex_t* m) { pthread_mutex_unlock(m); }
    void mutex_destroy(mutex_t* m) { pthread_mutex_destroy(m); }
    void thread_sleep(int ms) { usleep(ms * 1000); }
#endif

// Two resources with locks
mutex_t lock_a;
mutex_t lock_b;

// Shared account balances
int account_a = 1000;
int account_b = 1000;

// === DEADLOCK VERSION (BROKEN) ===

THREAD_FUNC transfer_a_to_b_bad(void* arg) {
    printf("[Thread 1] Attempting to transfer $100 from A to B\n");
    
    printf("[Thread 1] Locking A...\n");
    mutex_lock(&lock_a);
    printf("[Thread 1] Locked A ✓\n");
    
    thread_sleep(100);  // Give Thread 2 time to lock B
    
    printf("[Thread 1] Waiting for B...\n");
    mutex_lock(&lock_b);  // DEADLOCK! Thread 2 has B and wants A
    
    // This code never runs!
    account_a -= 100;
    account_b += 100;
    printf("[Thread 1] Transfer complete: A=$%d, B=$%d\n", account_a, account_b);
    
    mutex_unlock(&lock_b);
    mutex_unlock(&lock_a);
    
    THREAD_RETURN;
}

THREAD_FUNC transfer_b_to_a_bad(void* arg) {
    printf("[Thread 2] Attempting to transfer $200 from B to A\n");
    
    printf("[Thread 2] Locking B...\n");
    mutex_lock(&lock_b);
    printf("[Thread 2] Locked B ✓\n");
    
    thread_sleep(100);  // Give Thread 1 time to lock A
    
    printf("[Thread 2] Waiting for A...\n");
    mutex_lock(&lock_a);  // DEADLOCK! Thread 1 has A and wants B
    
    // This code never runs!
    account_b -= 200;
    account_a += 200;
    printf("[Thread 2] Transfer complete: A=$%d, B=$%d\n", account_a, account_b);
    
    mutex_unlock(&lock_a);
    mutex_unlock(&lock_b);
    
    THREAD_RETURN;
}

// === FIXED VERSION (LOCK ORDERING) ===

// Always lock in same order: A then B
void lock_both_ordered() {
    mutex_lock(&lock_a);
    mutex_lock(&lock_b);
}

void unlock_both() {
    mutex_unlock(&lock_b);
    mutex_unlock(&lock_a);
}

THREAD_FUNC transfer_a_to_b_good(void* arg) {
    printf("[Thread 1] Attempting to transfer $100 from A to B\n");
    
    printf("[Thread 1] Locking both (ordered)...\n");
    lock_both_ordered();  // Always same order!
    printf("[Thread 1] Locked both ✓\n");
    
    account_a -= 100;
    account_b += 100;
    printf("[Thread 1] Transfer complete: A=$%d, B=$%d\n", account_a, account_b);
    
    unlock_both();
    
    THREAD_RETURN;
}

THREAD_FUNC transfer_b_to_a_good(void* arg) {
    printf("[Thread 2] Attempting to transfer $200 from B to A\n");
    
    thread_sleep(50);  // Start slightly after Thread 1
    
    printf("[Thread 2] Locking both (ordered)...\n");
    lock_both_ordered();  // Always same order!
    printf("[Thread 2] Locked both ✓\n");
    
    account_b -= 200;
    account_a += 200;
    printf("[Thread 2] Transfer complete: A=$%d, B=$%d\n", account_a, account_b);
    
    unlock_both();
    
    THREAD_RETURN;
}

void run_deadlock_demo() {
    printf("\n=== DEADLOCK DEMONSTRATION (will hang!) ===\n");
    printf("This will deadlock. Press Ctrl+C to stop after ~5 seconds.\n\n");
    
    account_a = 1000;
    account_b = 1000;
    
    #ifdef _WIN32
        HANDLE threads[2];
        threads[0] = CreateThread(NULL, 0, transfer_a_to_b_bad, NULL, 0, NULL);
        threads[1] = CreateThread(NULL, 0, transfer_b_to_a_bad, NULL, 0, NULL);
        
        // Wait for threads (will never finish!)
        WaitForMultipleObjects(2, threads, TRUE, 5000);  // 5 second timeout
        
        printf("\n[Main] Threads are deadlocked!\n");
        printf("[Main] Thread 1 has lock A, waiting for lock B\n");
        printf("[Main] Thread 2 has lock B, waiting for lock A\n");
        printf("[Main] Neither can proceed - DEADLOCK!\n");
        
        // Force terminate (normally you'd need to restart program)
        TerminateThread(threads[0], 0);
        TerminateThread(threads[1], 0);
        CloseHandle(threads[0]);
        CloseHandle(threads[1]);
    #else
        pthread_t threads[2];
        pthread_create(&threads[0], NULL, transfer_a_to_b_bad, NULL);
        pthread_create(&threads[1], NULL, transfer_b_to_a_bad, NULL);
        
        // Wait briefly
        sleep(5);
        
        printf("\n[Main] Threads are deadlocked!\n");
        printf("[Main] Thread 1 has lock A, waiting for lock B\n");
        printf("[Main] Thread 2 has lock B, waiting for lock A\n");
        printf("[Main] Neither can proceed - DEADLOCK!\n");
        
        // Force cancel (normally you'd need to restart program)
        pthread_cancel(threads[0]);
        pthread_cancel(threads[1]);
        pthread_join(threads[0], NULL);
        pthread_join(threads[1], NULL);
    #endif
}

void run_fixed_demo() {
    printf("\n\n=== FIXED VERSION (lock ordering) ===\n\n");
    
    account_a = 1000;
    account_b = 1000;
    
    printf("Initial balances: A=$%d, B=$%d\n\n", account_a, account_b);
    
    #ifdef _WIN32
        HANDLE threads[2];
        threads[0] = CreateThread(NULL, 0, transfer_a_to_b_good, NULL, 0, NULL);
        threads[1] = CreateThread(NULL, 0, transfer_b_to_a_good, NULL, 0, NULL);
        
        WaitForMultipleObjects(2, threads, TRUE, INFINITE);
        
        CloseHandle(threads[0]);
        CloseHandle(threads[1]);
    #else
        pthread_t threads[2];
        pthread_create(&threads[0], NULL, transfer_a_to_b_good, NULL);
        pthread_create(&threads[1], NULL, transfer_b_to_a_good, NULL);
        
        pthread_join(threads[0], NULL);
        pthread_join(threads[1], NULL);
    #endif
    
    printf("\n✓ Both transfers completed successfully!\n");
    printf("Final balances: A=$%d, B=$%d\n", account_a, account_b);
}

int main(void) {
    printf("=== Deadlock Example ===\n");
    
    mutex_init(&lock_a);
    mutex_init(&lock_b);
    
    printf("\nThis program demonstrates:\n");
    printf("1. How deadlocks happen\n");
    printf("2. How to fix them with lock ordering\n\n");
    
    printf("Choose demo:\n");
    printf("  1 - Deadlock demonstration (will hang for 5s)\n");
    printf("  2 - Fixed version (lock ordering)\n");
    printf("  3 - Run both\n");
    printf("\nChoice (1-3): ");
    
    int choice;
    if (scanf("%d", &choice) != 1) {
        choice = 3;
    }
    
    // Clear input buffer
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    
    if (choice == 1 || choice == 3) {
        run_deadlock_demo();
    }
    
    if (choice == 2 || choice == 3) {
        run_fixed_demo();
    }
    
    mutex_destroy(&lock_a);
    mutex_destroy(&lock_b);
    
    printf("\n=== Key Lessons ===\n");
    printf("1. Deadlock occurs when:\n");
    printf("   - Thread A holds lock 1, waits for lock 2\n");
    printf("   - Thread B holds lock 2, waits for lock 1\n");
    printf("   - Neither can proceed\n\n");
    
    printf("2. Prevention strategies:\n");
    printf("   - Lock ordering: Always acquire locks in same order\n");
    printf("   - Lock timeout: Use trylock with timeout\n");
    printf("   - Single lock: Use one lock for related operations\n");
    printf("   - Lock-free: Use atomic operations when possible\n\n");
    
    printf("3. Always lock in consistent order to avoid deadlocks!\n");
    
    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}
