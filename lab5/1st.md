To compile this program with and without mutex, you'll need to make a few modifications and use specific compilation commands. Here's how:

1. **With Mutex**:
- Uncomment the mutex lines in both functions:
```c
pthread_mutex_lock(&mut);
// ... code ...
pthread_mutex_unlock(&mut);
```
- Compile with:
```bash
gcc -o mutex_program mutex.c -pthread
```

2. **Without Mutex**:
- Keep the mutex lines commented out (as shown in your code)
- Compile with:
```bash
gcc -o no_mutex_program mutex.c -pthread
```

**Differences in execution:**

1. **With Mutex**:
- The program will execute in a thread-safe manner
- Only one thread can access the shared variable `common` at a time
- The final counter value will be predictable and correct (should be 100)
- Output will be more orderly
- Execution might be slightly slower due to mutex overhead

2. **Without Mutex**:
- Race conditions will occur
- The final counter value will be unpredictable and likely less than 100
- Multiple threads might try to access and modify `common` simultaneously
- Output might be mixed up
- Execution might be slightly faster but incorrect

Example of race condition without mutex:
1. Thread1 reads common = 5
2. Thread2 reads common = 5
3. Thread1 increments to 6
4. Thread2 increments to 6
5. Thread1 writes 6
6. Thread2 writes 6
Result: Two increments but value only increased by 1

To see the difference, run each version multiple times:
```bash
./mutex_program
./no_mutex_program
```

The mutex version should consistently show a final counter value of 100, while the no-mutex version will likely show different (lower) values each time it's run.




EXTRA:

Let me explain mutex (Mutual Exclusion) in detail:

1. **What Mutex Does**:
- Mutex is like a lock that protects a "critical section" of code
- Only one thread can hold the mutex at a time
- If Thread A has the mutex and Thread B tries to get it, Thread B will wait until Thread A releases it
- It's used to protect shared resources (not just variables) from concurrent access

2. **Thread Execution**:
- Threads run in parallel (not sequentially) by default
- The OS scheduler decides which thread runs when
- Without mutex, both threads run simultaneously and can interfere with each other

Here's a visualization:

**Without Mutex**:
```
Thread 1:  Read(5)  →  Add(1)  →  Write(6)
Thread 2:      Read(5)  →  Add(1)  →  Write(6)
```

**With Mutex**:
```
Thread 1:  [LOCK] → Read(5) → Add(1) → Write(6) → [UNLOCK]
Thread 2:  wait... [LOCK] → Read(6) → Add(1) → Write(7) → [UNLOCK]
```

Here's a modified version of your program that demonstrates mutex better:

```c
void do_one_thing(int *pnum_times) {
    int i;
    for (i = 0; i < 50; i++) {
        pthread_mutex_lock(&mut);   // Lock critical section
        
        // Critical section starts
        printf("Thread 1 entering critical section\n");
        int current = *pnum_times;
        printf("Thread 1 read value: %d\n", current);
        
        // Simulate some work
        sleep(1);
        
        *pnum_times = current + 1;
        printf("Thread 1 wrote value: %d\n", *pnum_times);
        printf("Thread 1 leaving critical section\n\n");
        // Critical section ends
        
        pthread_mutex_unlock(&mut); // Unlock critical section
        
        // Non-critical section
        printf("Thread 1 doing other work\n");
    }
}
```

3. **Common Uses of Mutex**:
```c
// Protecting shared variable
pthread_mutex_lock(&mut);
shared_counter++;
pthread_mutex_unlock(&mut);

// Protecting shared resource (like file)
pthread_mutex_lock(&mut);
fprintf(shared_file, "data");
pthread_mutex_unlock(&mut);

// Protecting complex operations
pthread_mutex_lock(&mut);
if(queue_not_empty) {
    remove_from_queue();
    process_item();
}
pthread_mutex_unlock(&mut);
```

4. **Important Notes**:
- Mutex is NOT for making threads run sequentially
- It's for protecting shared resources during concurrent execution
- Too many mutexes can cause deadlocks
- Too few mutexes can cause race conditions
- Mutex should protect the smallest possible critical section

5. **Comparison with other synchronization**:
```c
// Mutex (exclusive access)
pthread_mutex_lock(&mut);
// only one thread at a time
pthread_mutex_unlock(&mut);

// Semaphore (limited concurrent access)
sem_wait(&sem);  // can allow N threads
// N threads can be here
sem_post(&sem);

// Condition Variable (waiting for event)
pthread_cond_wait(&cond, &mut);
// wait for specific condition
pthread_cond_signal(&cond);
```

Remember:
- Mutexes are for mutual exclusion
- They don't make threads run in order
- They protect critical sections
- They should be used minimally
- They can cause performance overhead