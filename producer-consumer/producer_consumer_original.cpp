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
#include <unistd.h>    // for sleep()
#include <cstdlib>     // for rand(), srand()
#include <ctime>       // for time()
#include <vector>
#include <fcntl.h>     // for shm_open()
#include <sys/mman.h>  // for mmap()
#include <sys/stat.h>  // for ftruncate()
#include <cstring>     // for memset()

using namespace std;

// =========================== Bounded Buffer Setup =========================== //

typedef int buffer_item;
#define BUFFER_SIZE 5           // Size of the circular buffer

// Shared memory file descriptor
int shm_fd;
// Pointer to the shared memory object
void *ptr;

// =========================== Synchronization Objects ======================== //

sem_t sem_empty;  // Semaphore for empty slots in the buffer
sem_t full;   // Semaphore for full slots in the buffer
pthread_mutex_t mutex_lock; // Mutex lock for buffer access

// =========================== Producer and Consumer Functions ================ //

void *producer(void *param);
void *consumer(void *param);

// =========================== Shared Memory Setup ============================ //
void initialize_shared_memory() {
    shm_unlink("/OS");  // Ensure it's gone before creating a new one


    shm_fd = shm_open("/OS", O_CREAT | O_RDWR, 0666);  // Use / prefix for macOS
    if (shm_fd == -1) {
        perror("shm_open");  // Print system error
        exit(1);
    }

    size_t size = BUFFER_SIZE * sizeof(buffer_item) + 2 * sizeof(int);
    if (ftruncate(shm_fd, size) == -1) {
        perror("ftruncate");  // More useful than cerr
        exit(1);
    }

    ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    buffer_item* buffer = (buffer_item*)ptr;
    for (int i = 0; i < BUFFER_SIZE; i++) {
        buffer[i] = -1;
    }

    int* in = (int*)ptr + BUFFER_SIZE;
    int* out = (int*)ptr + BUFFER_SIZE + 1;
    *in = 0;
    *out = 0;

    cout << "[Shared Memory] Initialized shared memory with buffer size: " << BUFFER_SIZE << endl;
}


void cleanup_shared_memory() {
    // Unlink the shared memory object
    shm_unlink("OS");
    cout << "[Shared Memory] Shared memory object cleaned up." << endl;
}

// =========================== Buffer Operations ============================= //

int insert_item(buffer_item item) {
    // Wait if there are no empty slots available (decrement empty)
    sem_wait(&sem_empty);

    // Acquire the mutex lock to access the buffer
    pthread_mutex_lock(&mutex_lock);

    // Critical section: insert item into the buffer in shared memory
    buffer_item *buffer = (buffer_item *)ptr;
    int* in = (int*)ptr + BUFFER_SIZE;
    *in = (*in + 1) % BUFFER_SIZE;
    buffer[*in] = item;

    // Release the mutex lock
    pthread_mutex_unlock(&mutex_lock);

    // Increment 'full' to signal that a new item is available
    sem_post(&full);

    return 0; // success
}

int remove_item(buffer_item *item) {
    // Wait if there are no full slots (decrement full)
    sem_wait(&full);

    // Acquire the mutex lock to access the buffer
    pthread_mutex_lock(&mutex_lock);

    // Critical section: remove item from the buffer in shared memory
    buffer_item *buffer = (buffer_item *)ptr;
    int* out = (int*)ptr + BUFFER_SIZE + 1;
    *item = buffer[*out];

    if (*item == -1) {  // Check if the item is empty (i.e., not produced)
        pthread_mutex_unlock(&mutex_lock);
        return -1;  // Return error if the slot is empty
    }

    *out = (*out + 1) % BUFFER_SIZE;

    // Release the mutex lock
    pthread_mutex_unlock(&mutex_lock);

    // Increment 'empty' to signal that a slot is now free
    sem_post(&sem_empty);

    return 0; // success
}

// =========================== Producer/Consumer Logic ======================== //

void *producer(void *param) {
    while (true) {
        buffer_item item = (rand() % 5) + 1;  // Always generate between 1 and 5
        cout << "[Producer] Generated item: " << item << endl;

        if (insert_item(item) == 0) {
            cout << "[Producer] Produced item: " << item << endl;
        } else {
            cerr << "Error inserting item by producer\n";
        }
    }
    return nullptr;
}


void *consumer(void *param) {
    while (true) {
        buffer_item item;
        if (remove_item(&item) == 0) {
            cout << "[Consumer] Consumed item: " << item << endl;
        } else {
            cerr << "Error removing item by consumer\n";
        }
    }
    return nullptr;
}


// =========================== main() ======================================== //

int main(int argc, char *argv[]) {
    if (argc != 4) {
        cerr << "Usage: " << argv[0] << " <sleep_time> <num_producers> <num_consumers>\n";
        return 1;
    }

    int sleep_time = atoi(argv[1]);
    int num_producers = atoi(argv[2]);
    int num_consumers = atoi(argv[3]);

    srand((unsigned int)time(NULL));

    // Initialize semaphores
    sem_init(&sem_empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    pthread_mutex_init(&mutex_lock, NULL);

    // Initialize shared memory for buffer
    initialize_shared_memory();

    // Create producer and consumer threads
    vector<pthread_t> producers(num_producers);
    vector<pthread_t> consumers(num_consumers);

    for (int i = 0; i < num_producers; i++) {
        pthread_create(&producers[i], NULL, producer, NULL);
        // Print the thread ID using pthread_self() instead of memory address
        pthread_t thread_id = producers[i];
        cout << "[Main] Created producer thread with ID: " << thread_id << endl;
    }

    for (int i = 0; i < num_consumers; i++) {
        pthread_create(&consumers[i], NULL, consumer, NULL);
        // Print the thread ID using pthread_self() instead of memory address
        pthread_t thread_id = consumers[i];
        cout << "[Main] Created consumer thread with ID: " << thread_id << endl;
    }

    cout << "[Main] Sleeping for " << sleep_time << " seconds..." << endl;
    sleep(sleep_time);

    cout << "\n[Main] Done sleeping for " << sleep_time << "s. Exiting...\n" << endl;
    cleanup_shared_memory();

    pthread_mutex_destroy(&mutex_lock);
    sem_destroy(&sem_empty);
    sem_destroy(&full);

    return 0;
}