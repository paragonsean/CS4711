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

#ifdef __APPLE__
    // macOS: No need to pin threads to specific CPU cores (Use GCD if needed)
    #include <dispatch/dispatch.h>
#else
    // Linux: Include sched.h for CPU pinning
    #include <sched.h>
#endif

using namespace std;

// Cache-line aligned buffer item for efficient memory access
struct alignas(64) buffer_item {
    int value;  // The actual item value (could be anything, like an integer)
};

constexpr int BUFFER_SIZE = 128; // Larger buffer size for better cache utilization

// Shared memory variables
int shm_fd;          // Shared memory file descriptor
void* ptr;           // Pointer to the shared memory

// Synchronization primitives (semaphores and mutex)
sem_t sem_empty;     // Semaphore to track empty slots in the buffer
sem_t sem_full;      // Semaphore to track full slots in the buffer
pthread_mutex_t mutex_lock;  // Mutex lock for critical section

// Counters for tracking produced and consumed items
int total_produced = 0;
int total_consumed = 0;

// Function declarations
void* producer(void*);
void* consumer(void*);

// Pin thread to a specific core (used for performance optimizations)
void pin_thread_to_core(int core_id) {
#ifdef __APPLE__
    // macOS: No direct way to pin threads to CPU cores, we can skip or use GCD for thread management
    // Example using GCD (this doesn't give you CPU affinity, but helps manage thread execution)
    dispatch_queue_t queue = dispatch_queue_create("com.myapp.queue", NULL);
    dispatch_async(queue, ^{
        // Thread work goes here
    });
#else
    // Linux: Pin the thread to a specific core
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);

    pthread_t current_thread = pthread_self();
    pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset);  // Set the thread's CPU affinity
#endif
}

// Initialize shared memory buffer
void initialize_shared_memory() {
    shm_unlink("/OS");  // Unlink any existing shared memory
    shm_fd = shm_open("/OS", O_CREAT | O_RDWR, 0666);  // Open shared memory
    if (shm_fd == -1) { perror("shm_open"); exit(EXIT_FAILURE); }

    size_t size = BUFFER_SIZE * sizeof(buffer_item) + 2 * sizeof(int);  // Size for the buffer and in/out indices
    if (ftruncate(shm_fd, size) == -1) { perror("ftruncate"); exit(EXIT_FAILURE); }

    ptr = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);  // Map the shared memory to the process
    if (ptr == MAP_FAILED) { perror("mmap"); exit(EXIT_FAILURE); }

    // Touch all pages to prefault into RAM to avoid page faults later
    memset(ptr, 0, size);

    // Initialize the buffer and in/out pointers
    auto* buffer = static_cast<buffer_item*>(ptr);
    int* in = reinterpret_cast<int*>(buffer + BUFFER_SIZE);
    int* out = in + 1;
    *in = 0;
    *out = 0;
}

// Cleanup shared memory
void cleanup_shared_memory() {
    munmap(ptr, BUFFER_SIZE * sizeof(buffer_item) + 2 * sizeof(int));  // Unmap shared memory
    shm_unlink("/OS");  // Unlink shared memory to clean up
}

// Insert item into the buffer
void insert_item(int item_value) {
    sem_wait(&sem_empty);  // Wait for an empty slot in the buffer
    pthread_mutex_lock(&mutex_lock);  // Lock critical section

    auto* buffer = static_cast<buffer_item*>(ptr);
    int* in = reinterpret_cast<int*>(buffer + BUFFER_SIZE);

    // Insert item at the 'in' index and increment it
    buffer[*in].value = item_value;
    *in = (*in + 1) % BUFFER_SIZE;  // Circular buffer behavior
    ++total_produced;  // Increment the produced counter

    pthread_mutex_unlock(&mutex_lock);  // Unlock critical section
    sem_post(&sem_full);  // Signal that a new item is available in the buffer
}

// Remove item from the buffer
void remove_item(int* item_value) {
    sem_wait(&sem_full);  // Wait for a full slot in the buffer
    pthread_mutex_lock(&mutex_lock);  // Lock critical section

    auto* buffer = static_cast<buffer_item*>(ptr);
    int* out = reinterpret_cast<int*>(buffer + BUFFER_SIZE) + 1;

    // Remove item from the 'out' index and increment it
    *item_value = buffer[*out].value;
    *out = (*out + 1) % BUFFER_SIZE;  // Circular buffer behavior
    ++total_consumed;  // Increment the consumed counter

    pthread_mutex_unlock(&mutex_lock);  // Unlock critical section
    sem_post(&sem_empty);  // Signal that there is an empty slot in the buffer
}

// Producer thread function
void* producer(void* arg) {
    int core_id = *(int*)arg;  // Get the core ID passed to this thread
    pin_thread_to_core(core_id);  // Pin the producer thread to the given core

    while (true) {
        int item_value = rand() % 100 + 1;  // Produce a random item (simulating production)
        insert_item(item_value);  // Insert item into the buffer
    }
    return nullptr;
}

// Consumer thread function
void* consumer(void* arg) {
    int core_id = *(int*)arg;  // Get the core ID passed to this thread
    pin_thread_to_core(core_id);  // Pin the consumer thread to the given core

    while (true) {
        int item_value;
        remove_item(&item_value);  // Remove item from the buffer
    }
    return nullptr;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Usage: " << argv[0] << " <sleep_time> <num_producers> <num_consumers>\n";
        return EXIT_FAILURE;
    }

    double sleep_time = atof(argv[1]);  // Sleep time for simulation
    int num_producers = atoi(argv[2]);  // Number of producer threads
    int num_consumers = atoi(argv[3]);  // Number of consumer threads

    cout << "Parameters -> sleep_time: " << sleep_time
         << ", producers: " << num_producers
         << ", consumers: " << num_consumers << endl;

    srand(static_cast<unsigned>(time(nullptr)));  // Seed for random number generation

    sem_init(&sem_empty, 0, BUFFER_SIZE);  // Initialize empty slots semaphore
    sem_init(&sem_full, 0, 0);  // Initialize full slots semaphore
    pthread_mutex_init(&mutex_lock, nullptr);  // Initialize mutex lock

    initialize_shared_memory();  // Set up shared memory

    auto t_start = chrono::high_resolution_clock::now();  // Start timer

    vector<pthread_t> threads;  // Vector to hold thread IDs
    vector<int> core_ids;  // Vector to hold core IDs for each thread
    threads.reserve(num_producers + num_consumers);  // Reserve space for all threads
    core_ids.reserve(num_producers + num_consumers);  // Reserve space for all core IDs

    int core_id = 0;  // Start assigning cores from 0
    // Create producer threads
    for (int i = 0; i < num_producers; ++i) {
        pthread_t tid;
        core_ids.push_back(core_id++);  // Assign a core ID to each producer
        pthread_create(&tid, nullptr, producer, &core_ids.back());
        threads.push_back(tid);
    }

    // Create consumer threads
    for (int i = 0; i < num_consumers; ++i) {
        pthread_t tid;
        core_ids.push_back(core_id++);  // Assign a core ID to each consumer
        pthread_create(&tid, nullptr, consumer, &core_ids.back());
        threads.push_back(tid);
    }

    this_thread::sleep_for(chrono::duration<double>(sleep_time));  // Sleep for the given time

    auto t_end = chrono::high_resolution_clock::now();  // End timer
    double elapsed = chrono::duration<double>(t_end - t_start).count();  // Calculate elapsed time

    cout << "Total items produced: " << total_produced << endl;
    cout << "Total items consumed: " << total_consumed << endl;
    cout << "Elapsed time: " << elapsed << " seconds\n";
    cout << "Throughput: " << (total_consumed / elapsed) << " items/sec\n";  // Output throughput

    cleanup_shared_memory();  // Clean up shared memory
    pthread_mutex_destroy(&mutex_lock);  // Destroy mutex lock
    sem_destroy(&sem_empty);  // Destroy semaphore for empty slots
    sem_destroy(&sem_full);  // Destroy semaphore for full slots

    return EXIT_SUCCESS;
}
