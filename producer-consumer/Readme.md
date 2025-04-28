Producer-Consumer with Shared Memory

Overview
Video
https://youtu.be/lCTqOw4wVdc

This program demonstrates a producer-consumer model where multiple producer and consumer threads interact with a shared memory buffer. The shared memory buffer is implemented using shm_open() and mmap() functions. Semaphores are used for synchronization to manage the empty and full slots in the buffer, and mutexes are used to ensure mutual exclusion when accessing the buffer.

Key Concepts:
Producer-Consumer Problem: Multiple producer threads generate items and place them into a shared buffer. Multiple consumer threads consume items from the same buffer. The buffer has a limited capacity.
Shared Memory: The buffer is stored in a shared memory region, allowing the producer and consumer threads to interact with the same data.
Semaphores: Used to control access to the buffer and ensure that producers and consumers do not exceed the buffer's limits.
Mutex Lock: Ensures that only one thread can access the buffer at a time, preventing race conditions.
Requirements

Operating System: macOS (this implementation uses shm_open, mmap, and semaphores specific to UNIX-like systems).
Compiler: clang or gcc to compile the program.
Libraries:
POSIX Threads (pthread)
POSIX Semaphores (semaphore.h)
Shared Memory (sys/mman.h, fcntl.h)
Compilation

To compile the program, you can use the following command:

clang++ -o producer_consumer_shared_memory producer_consumer.cpp -lpthread
Usage

To run the program, use the following command:

./producer_consumer_shared_memory <sleep_time> <num_producers> <num_consumers>
sleep_time: The number of seconds the main thread will sleep before the program exits.
num_producers: The number of producer threads.
num_consumers: The number of consumer threads.
Example:

./producer_consumer_shared_memory 10 2 2
This will create 2 producer threads and 2 consumer threads. The main thread will sleep for 10 seconds before exiting.

Program Flow

Initialization:
The program first initializes shared memory for the buffer using shm_open() and mmap().
It also sets up semaphores (empty, full) for buffer slots and a mutex (mutex_lock) to synchronize access to the buffer.
Producer Threads:
Each producer thread generates a random number (representing an item) and attempts to insert it into the shared buffer.
The thread first waits for an empty slot using the empty semaphore.
It acquires the mutex lock before accessing the buffer to prevent race conditions.
After inserting the item, it releases the mutex lock and signals that an item has been produced by posting to the full semaphore.
Consumer Threads:
Each consumer thread waits for a full slot in the buffer using the full semaphore.
It acquires the mutex lock before accessing the buffer to prevent race conditions.
After consuming an item, it releases the mutex lock and signals that a slot is now empty by posting to the empty semaphore.
Main Thread:
The main thread creates the producer and consumer threads.
It then sleeps for the specified sleep_time before cleaning up the resources (shared memory, semaphores, and mutex).
