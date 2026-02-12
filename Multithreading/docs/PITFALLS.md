# Threading Pitfalls

## The Big Picture

Threading is hard. Bugs are subtle, intermittent, and hard to reproduce. A thread bug might work perfectly 99.9% of the time, then corrupt data on the 1000th run.

Learn the common pitfalls and how to avoid them.

## Deadlock

Two threads waiting for each other forever:

```c
pthread_mutex_t lock_a, lock_b;

// Thread 1                    // Thread 2
pthread_mutex_lock(&lock_a);   pthread_mutex_lock(&lock_b);
pthread_mutex_lock(&lock_b);   pthread_mutex_lock(&lock_a);
// DEADLOCK!                   // Both stuck waiting
```

**Timeline:**
```
T1: Lock A ✓
T2:          Lock B ✓
T1: Lock B ⏰ (waiting for T2)
T2:          Lock A ⏰ (waiting for T1)
```

### Prevention

**1. Lock ordering:**
Always acquire locks in same order:

```c
// Always lock A before B
void thread_func() {
    pthread_mutex_lock(&lock_a);
    pthread_mutex_lock(&lock_b);
    // Work...
    pthread_mutex_unlock(&lock_b);
    pthread_mutex_unlock(&lock_a);
}
```

**2. Try-lock with timeout:**
```c
if (pthread_mutex_trylock(&lock_b) != 0) {
    // Couldn't get lock, release lock_a and retry
    pthread_mutex_unlock(&lock_a);
    continue;
}
```

**3. Single global lock:**
Simple but less parallel.

## Race Conditions

Outcome depends on timing:

```c
int balance = 100;

// Thread 1: Deposit         // Thread 2: Withdraw
int temp = balance;          int temp = balance;
temp = temp + 50;            temp = temp - 30;
balance = temp;              balance = temp;
// Lost update!
```

**Result:** Varies! Might be 150, 70, or 120 depending on timing.

### Detection

Race conditions are hard to catch because:
- May work most of the time
- Timing-dependent
- Different on debug vs release
- Different CPU speeds change behavior

**Tools:**
- ThreadSanitizer: `gcc -fsanitize=thread`
- Valgrind with helgrind: `valgrind --tool=helgrind`
- Static analyzers

### Prevention

**1. Identify shared data:**
```c
// Shared (needs protection)
int global_counter;
Data* shared_data;

// Not shared (thread-local, safe)
int local_variable;
```

**2. Protect all accesses:**
```c
// Every access needs lock
pthread_mutex_lock(&mutex);
counter++;
pthread_mutex_unlock(&mutex);

// Not just writes!
pthread_mutex_lock(&mutex);
int value = counter;  // Reads too!
pthread_mutex_unlock(&mutex);
```

**3. Use const for read-only data:**
```c
const Config* config = load_config();
// Threads can read freely, no writes allowed
```

## Priority Inversion

High-priority thread blocked by low-priority thread:

```
Low priority thread: Lock mutex
High priority thread: Wait for mutex (blocked!)
Medium priority thread: Runs, delaying low priority
```

High priority waits for low priority to release lock, but medium priority keeps running.

**Solution:** Priority inheritance - temporarily boost priority of lock holder.

## Starvation

Thread never gets to run:

```c
// Writer starves if readers keep coming
while (1) {
    pthread_rwlock_rdlock(&rwlock);
    read_data();
    pthread_rwlock_unlock(&rwlock);
    
    // New reader arrives immediately
    // Writer never gets a chance!
}
```

**Prevention:** Fair locks, time limits, priority adjustments.

## False Sharing

Threads modify different variables on same cache line:

```c
struct {
    int counter_a;  // Thread 1 modifies this
    int counter_b;  // Thread 2 modifies this
} shared;  // Both on same cache line!
```

**Problem:** Each write invalidates the other thread's cache.

**Solution:** Pad to separate cache lines:

```c
struct {
    int counter_a;
    char padding1[60];  // Keep on separate cache lines
    int counter_b;
    char padding2[60];
} shared;
```

## Forgotten Lock

```c
pthread_mutex_lock(&mutex);

if (error_condition) {
    return;  // FORGOT TO UNLOCK!
}

pthread_mutex_unlock(&mutex);
```

**Solution:** Use lock guards or goto cleanup pattern:

```c
pthread_mutex_lock(&mutex);

if (error_condition) {
    goto cleanup;
}

// Work...

cleanup:
    pthread_mutex_unlock(&mutex);
    return;
```

## ABA Problem

Lock-free code issue:

```
1. Thread 1 reads A
2. Thread 2 changes A → B → A (back to A!)
3. Thread 1 thinks nothing changed
```

**Solution:** Use version numbers or generation counts.

## Sleeping While Locked

```c
pthread_mutex_lock(&mutex);
sleep(1000);  // BAD! Lock held for 1 second
pthread_mutex_unlock(&mutex);
```

**Rule:** Never sleep, block, or do I/O while holding a lock.

## Testing Concurrent Code

**Problem:** Race conditions are probabilistic. Might need thousands of runs to trigger.

**Strategies:**

**1. Stress testing:**
```c
// Run many iterations
for (int i = 0; i < 100000; i++) {
    spawn_threads();
    join_threads();
    check_invariants();
}
```

**2. Add delays to expose races:**
```c
// Insert random sleeps
counter++;
usleep(rand() % 10);  // Random delay
```

**3. Run with sanitizers:**
```bash
gcc -fsanitize=thread program.c
./program
```

**4. Increase thread count:**
More threads = more contention = more likely to trigger bugs.

**5. Use assertions:**
```c
pthread_mutex_lock(&mutex);
assert(invariant_holds());
// Modify data
assert(invariant_holds());
pthread_mutex_unlock(&mutex);
```

## Debugging Tips

**1. Reproduce consistently:**
- Add logging with timestamps and thread IDs
- Use same seed for random numbers
- Control thread scheduling with priorities

**2. Simplify:**
- Reduce to minimal test case
- Single out problematic section
- Remove unrelated code

**3. Visual tools:**
- ThreadSanitizer output
- Valgrind helgrind
- GDB thread commands (`info threads`, `thread N`)

**4. Print thread IDs:**
```c
#ifdef _WIN32
printf("[Thread %lu] Message\n", GetCurrentThreadId());
#else
printf("[Thread %lu] Message\n", (unsigned long)pthread_self());
#endif
```

## Common Anti-Patterns

**1. Too many threads:**
```c
// Bad: 10,000 threads on 4 cores
for (int i = 0; i < 10000; i++) {
    pthread_create(&threads[i], NULL, worker, NULL);
}

// Good: 4 threads, 10,000 tasks
ThreadPool* pool = pool_create(4);
for (int i = 0; i < 10000; i++) {
    pool_add_task(pool, work, &tasks[i]);
}
```

**2. Excessive locking:**
```c
// Bad: Lock for every access
for (int i = 0; i < 1000; i++) {
    pthread_mutex_lock(&mutex);
    sum += array[i];
    pthread_mutex_unlock(&mutex);
}

// Good: Accumulate locally, lock once
int local_sum = 0;
for (int i = 0; i < 1000; i++) {
    local_sum += array[i];
}
pthread_mutex_lock(&mutex);
sum += local_sum;
pthread_mutex_unlock(&mutex);
```

**3. Busy waiting:**
```c
// Bad: Wastes CPU
while (!ready) {
    // Spinning, using 100% CPU
}

// Good: Block and wait
pthread_mutex_lock(&mutex);
while (!ready) {
    pthread_cond_wait(&cond, &mutex);
}
pthread_mutex_unlock(&mutex);
```

## When NOT to Use Threads

**Don't use threads when:**
- Sequential is fast enough
- I/O-bound (async I/O might be better)
- Adds complexity without benefit
- Can't test properly
- Team doesn't understand concurrency

**Alternatives:**
- Async I/O (event loop)
- Processes (isolation)
- Single-threaded (simplicity)

## Summary

Threading pitfalls are many:
- Deadlocks: Lock ordering
- Race conditions: Synchronize everything
- Starvation: Fair scheduling
- False sharing: Cache line awareness

Testing is hard - use sanitizers, stress testing, and logging.

Key lesson: **Keep it simple.** Minimize shared state. Use proven patterns. Test thoroughly. Threading is powerful but dangerous.
