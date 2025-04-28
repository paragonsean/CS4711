/**************************************************************
 * producer_consumer.cpp
 * Sean Baker 04/26/2025
 * sbake021@odu.edu
 * Bounded-buffer (size 5) producer–consumer solution using:
 *   - Pthreads (pthread_create) and optimizations including volatile and 
     using memory pages to improve cache locality
 *   - POSIX semaphores (sem_init, sem_wait, sem_post)
 *   - A mutex lock for mutual exclusion
 *
 * Usage:
 *    g++ -o producer_consumer producer_consumer.cpp -pthread -lrt
 *    ./producer_consumer <sleep_time> <num_producers> <num_consumers>
 *
 * Example:
 *    ./producer_consumer 10 1 1
 *
 * Explanation:
 *   - Main creates <num_producers> producer threads
 *   - Main creates <num_consumers> consumer threads
 *   - Main sleeps for <sleep_time> seconds, then the program exits
 *
 **************************************************************/
#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <cstring>
#include <thread>    
#include <chrono>    

using namespace std;

typedef int buffer_item;
#define BUFFER_SIZE 5

// Shared memory
int shm_fd;
void* ptr;

// Synchronization
sem_t sem_empty;
sem_t full;
pthread_mutex_t mutex_lock;

// Counters
int total_produced = 0;
int total_consumed = 0;

// Thread functionsoid* producer(void*);
void* consumer(void*);


// Initialize shared memory buffer and touch memory to improve cache locality
void initialize_shared_memory() {
    shm_unlink("/OS");
    shm_fd = shm_open("/OS", O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) exit(1);

    size_t size = BUFFER_SIZE * sizeof(buffer_item) + 2 * sizeof(int);
    if (ftruncate(shm_fd, size) == -1) exit(1);

    ptr = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) exit(1);

    buffer_item* buffer = static_cast<buffer_item*>(ptr);
    for (int i = 0; i < BUFFER_SIZE; ++i) buffer[i] = -1;

    int* in  = reinterpret_cast<int*>(buffer + BUFFER_SIZE);
    int* out = in + 1;
    *in  = 0;
    *out = 0;

    // Touch all memory pages to improve cache locality
    for (int i = 0; i < BUFFER_SIZE; ++i) {
        volatile buffer_item tmp = buffer[i];  // Read from the buffer to ensure it's loaded into cache
    }

    // Touch the in and out pointers
    volatile int tmp_in = *in;
    volatile int tmp_out = *out;
}


// Cleanup shared memory
void cleanup_shared_memory() {
    shm_unlink("/OS");
}

// Insert item into buffer and count
int insert_item(buffer_item item) {
    sem_wait(&sem_empty);
    pthread_mutex_lock(&mutex_lock);

    auto* buffer = static_cast<buffer_item*>(ptr);
    int* in = reinterpret_cast<int*>(buffer + BUFFER_SIZE);
    *in = (*in + 1) % BUFFER_SIZE;
    buffer[*in] = item;
    ++total_produced;

    pthread_mutex_unlock(&mutex_lock);
    sem_post(&full);
    return 0;
}

// Remove item from buffer and count
int remove_item(buffer_item* item) {
    // Wait until there is at least one item in the buffer
    sem_wait(&full);  

    pthread_mutex_lock(&mutex_lock);

    auto* buffer = static_cast<buffer_item*>(ptr);
    int* out = reinterpret_cast<int*>(buffer + BUFFER_SIZE) + 1;

    // If the item at 'out' is valid (not -1), consume it
    if (buffer[*out] != -1) {
        *item = buffer[*out];  // Remove the item
        ++total_consumed;       // Increment the total consumed count
        *out = (*out + 1) % BUFFER_SIZE;  // Update the circular buffer index
    }

    pthread_mutex_unlock(&mutex_lock);
    sem_post(&sem_empty);  // Signal that there's an empty slot available
    return 0;
}



// Producer thread: produce forever
void* producer(void*) {
    while (true) {
        buffer_item item = (rand() % 5) + 1;
        insert_item(item);
    }
    return nullptr;
}

// Consumer thread: consume forever
void* consumer(void*) {
    while (true) {
        buffer_item item;
        remove_item(&item);
    }
    return nullptr;
}

int main(int argc, char* argv[]) {
    if (argc != 4) return 1;

    // parse args (now supports fractional seconds)
    double sleep_time    = atof(argv[1]);
    int    num_producers = atoi(argv[2]);
    int    num_consumers = atoi(argv[3]);

    cout << "Parameters -> sleep_time: " << sleep_time
         << ", producers: "  << num_producers
         << ", consumers: "  << num_consumers << endl;

    srand(static_cast<unsigned>(time(nullptr)));

    sem_init(&sem_empty, 0, BUFFER_SIZE);
    sem_init(&full,      0, 0);
    pthread_mutex_init(&mutex_lock, nullptr);

    initialize_shared_memory();

    // start timing
    auto t_start = chrono::high_resolution_clock::now();

    // launch producers
    vector<pthread_t> prod_threads(num_producers);
    for (int i = 0; i < num_producers; ++i)
        pthread_create(&prod_threads[i], nullptr, producer, nullptr);

    // launch consumers
    vector<pthread_t> cons_threads(num_consumers);
    for (int i = 0; i < num_consumers; ++i)
        pthread_create(&cons_threads[i], nullptr, consumer, nullptr);

    // fractional sleep: e.g. 2.5 seconds
    this_thread::sleep_for(chrono::duration<double>(sleep_time));

    // end timing
    auto t_end   = chrono::high_resolution_clock::now();
    double elapsed = chrono::duration<double>(t_end - t_start).count();

    // report
    cout << "Total items produced: " << total_produced << endl;
    cout << "Total items consumed: " << total_consumed << endl;
    cout << "Elapsed time: " << elapsed << " seconds" << endl;
    cout << "Throughput: "
         << (total_consumed / elapsed)
         << " items/sec" << endl;

    // cleanup
    cleanup_shared_memory();
    pthread_mutex_destroy(&mutex_lock);
    sem_destroy(&sem_empty);
    sem_destroy(&full);

    return 0;
}
