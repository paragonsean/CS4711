/**************************************************************
 * producer_consumer.cpp
 *
 * Bounded-buffer (size 5) producer–consumer solution using:
 *   - Pthreads (pthread_create)
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
 
 using namespace std;
 
 // =========================== Bounded Buffer Setup =========================== //
 
 // We'll define these directly here. In a bigger program, they'd be in buffer.h
 typedef int buffer_item;        // The type of item produced/consumed
 #define BUFFER_SIZE 5           // Size of the circular buffer
 
 // The circular buffer and indices
 buffer_item buffer[BUFFER_SIZE];
 int in = 0;   // Next position to insert
 int out = 0;  // Next position to remove
 
 // =========================== Synchronization Objects ======================== //
 
 // Counting semaphores
 sem_t empty;  // how many empty slots
 sem_t full;   // how many full slots
 
 // Mutex lock for buffer access
 pthread_mutex_t mutex_lock;
 
 // =========================== Producer and Consumer Functions ================ //
 
 void *producer(void *param);
 void *consumer(void *param);
 
 // Insert item into the circular buffer
 int insert_item(buffer_item item);
 // Remove item from the circular buffer
 int remove_item(buffer_item *item);
 
 // =========================== Producer/Consumer Logic ======================== //
 
 // Producer thread function
 void *producer(void *param)
 {
     while (true) {
         // Sleep for a random time (0-3 seconds)
         int r = rand() % 4;
         sleep(r);
 
         // Generate a random number as the "product"
         buffer_item item = rand() % 100;
 
         // Insert item into buffer
         if (insert_item(item) == 0) {
             cout << "Producer produced " << item << endl;
         } else {
             cerr << "Error inserting item by producer\n";
         }
     }
     return nullptr;
 }
 
 // Consumer thread function
 void *consumer(void *param)
 {
     while (true) {
         // Sleep for a random time (0-3 seconds)
         int r = rand() % 4;
         sleep(r);
 
         buffer_item item;
         // Remove item from buffer
         if (remove_item(&item) == 0) {
             cout << "Consumer consumed " << item << endl;
         } else {
             cerr << "Error removing item by consumer\n";
         }
     }
     return nullptr;
 }
 
 // =========================== Buffer Operations ============================= //
 
 // insert_item
 int insert_item(buffer_item item)
 {
     // Decrement empty (wait if no empty slots)
     sem_wait(&empty);
 
     // Acquire the mutex lock
     pthread_mutex_lock(&mutex_lock);
 
     // Critical section
     buffer[in] = item;
     in = (in + 1) % BUFFER_SIZE;
 
     // Release the mutex
     pthread_mutex_unlock(&mutex_lock);
 
     // Increment 'full'
     sem_post(&full);
 
     return 0; // success
 }
 
 // remove_item
 int remove_item(buffer_item *item)
 {
     // Wait if there are no full slots
     sem_wait(&full);
 
     // Acquire the mutex
     pthread_mutex_lock(&mutex_lock);
 
     // Critical section
     *item = buffer[out];
     out = (out + 1) % BUFFER_SIZE;
 
     // Release the mutex
     pthread_mutex_unlock(&mutex_lock);
 
     // Increment 'empty'
     sem_post(&empty);
 
     return 0; // success
 }
 
 // =========================== main() ======================================== //
 
 int main(int argc, char *argv[])
 {
     if (argc != 4) {
         cerr << "Usage: " << argv[0] << " <sleep_time> <num_producers> <num_consumers>\n";
         return 1;
     }
 
     int sleep_time = atoi(argv[1]);
     int num_producers = atoi(argv[2]);
     int num_consumers = atoi(argv[3]);
 
     // Initialize rand
     srand((unsigned int)time(NULL));
 
     // Initialize the semaphores
     // Initially, buffer is empty => empty=BUFFER_SIZE, full=0
     sem_init(&empty, 0, BUFFER_SIZE);
     sem_init(&full, 0, 0);
 
     // Initialize the mutex
     pthread_mutex_init(&mutex_lock, NULL);
 
     // Create producer and consumer threads
     vector<pthread_t> producers(num_producers);
     vector<pthread_t> consumers(num_consumers);
 
     // Producer threads
     for (int i = 0; i < num_producers; i++) {
         pthread_create(&producers[i], NULL, producer, NULL);
     }
     // Consumer threads
     for (int i = 0; i < num_consumers; i++) {
         pthread_create(&consumers[i], NULL, consumer, NULL);
     }
 
     // Main thread sleeps for 'sleep_time' seconds
     sleep(sleep_time);
 
     cout << "\n[Main] Done sleeping for " << sleep_time << "s. Exiting...\n" << endl;
 
     // Cleanup: In a real scenario, you might cancel the threads or
     // set a global flag telling them to exit gracefully. 
     // For demonstration, we simply exit.
 
     // Destroy semaphores, mutex
     pthread_mutex_destroy(&mutex_lock);
     sem_destroy(&empty);
     sem_destroy(&full);
 
     return 0;
 }
 