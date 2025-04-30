# Producer-Consumer with Shared Memory

---

## 📺 Overview

This project demonstrates a **producer-consumer model** where multiple producer and consumer threads interact through a **shared memory buffer**.

- **Shared Memory** is implemented using `shm_open()` and `mmap()`.
- **Semaphores** manage empty and full slots.
- **Mutex Locks** ensure mutual exclusion on buffer access.

📺 **Video Walkthrough**:  
[https://youtu.be/lCTqOw4wVdc](https://youtu.be/lCTqOw4wVdc)

---

## 🧠 Key Concepts

- **Producer-Consumer Problem**:
    - Multiple producers generate items and place them into a shared buffer.
    - Multiple consumers remove items from the same buffer.
    - The buffer has a **limited capacity** (5 items).

- **Shared Memory**:
    - Buffer is stored in a memory region shared by all threads.

- **Semaphores**:
    - Manage synchronization:
        - `sem_empty`: counts available empty slots.
        - `full`: counts available filled slots.

- **Mutex Lock**:
    - Protects the critical section to prevent **race conditions**.

---

## 📋 Requirements

- **Operating System**: macOS (uses UNIX-like system calls).
- **Compiler**: `clang` or `g++`.
- **Libraries**:
    - POSIX Threads (`pthread`)
    - POSIX Semaphores (`semaphore.h`)
    - Shared Memory (`sys/mman.h`, `fcntl.h`)

---

## ⚙️ Compilation

Compile the program using:

```bash
clang++ -o producer_consumer_shared_memory producer_consumer.cpp -lpthread
```

Or using `g++`:

```bash
g++ -o producer_consumer_shared_memory producer_consumer.cpp -pthread -lrt
```

---

## 🚀 Usage

Run the program with:

```bash
./producer_consumer_shared_memory <sleep_time> <num_producers> <num_consumers>
```

Where:

- `<sleep_time>`: How long (in seconds) the main thread should sleep before terminating.
- `<num_producers>`: Number of producer threads.
- `<num_consumers>`: Number of consumer threads.

### Example

```bash
./producer_consumer_shared_memory 10 2 2
```

- Creates 2 producers and 2 consumers.
- Main thread sleeps for 10 seconds.
- After sleeping, all resources are cleaned up and the program exits.

---

## 📈 Program Flow

### Initialization

- **Shared memory** is initialized with `shm_open()` and `mmap()`.
- A circular buffer of size 5 is created.
- Two control integers `in` and `out` are initialized.
- **Semaphores** (`sem_empty`, `full`) and **mutex** (`mutex_lock`) are initialized.

### Producer Threads

Each producer:

1. Waits for an empty slot (`sem_wait(sem_empty)`).
2. Locks the buffer (`pthread_mutex_lock`).
3. Inserts an item into the buffer at the `in` index.
4. Increments `in = (in + 1) % BUFFER_SIZE`.
5. Unlocks the buffer (`pthread_mutex_unlock`).
6. Signals the presence of a new full slot (`sem_post(full)`).

### Consumer Threads

Each consumer:

1. Waits for a full slot (`sem_wait(full)`).
2. Locks the buffer (`pthread_mutex_lock`).
3. Removes an item from the buffer at the `out` index.
4. Increments `out = (out + 1) % BUFFER_SIZE`.
5. Unlocks the buffer (`pthread_mutex_unlock`).
6. Signals a newly freed empty slot (`sem_post(sem_empty)`).

### Main Thread

- Creates the specified number of producer and consumer threads.
- Sleeps for `sleep_time` seconds.
- After sleeping:
    - Shared memory is unlinked (`shm_unlink`).
    - Semaphores and mutex are destroyed.

---

## 📦 Buffer Structure (in Shared Memory)

| Memory Layout |
| :------------- |
| `buffer[0] buffer[1] buffer[2] buffer[3] buffer[4] in out` |

- `buffer[i]` stores the produced items.
- `in` and `out` manage where to insert and remove items.

---

## 🧠 Concepts Illustrated

- **Multithreading** with pthreads.
- **Shared Memory Communication** (`shm_open`, `mmap`).
- **Synchronization Primitives**:
    - Semaphores for resource management.
    - Mutex for critical sections.
- **Bounded Buffer Management**.
- **System Resource Cleanup** after process termination.

---

## ⚠️ Notes and Assumptions

- Buffer size is fixed at 5.
- Threads are terminated automatically with the process (no thread joining implemented yet).
- No explicit graceful shutdown signaling to threads — designed for educational demonstration purposes.
- Program output provides clear logs for each item produced and consumed.

---

## ✍️ Author

**Sean Baker**  
Computer Science Student  
Old Dominion University  
📧 [sbake021@odu.edu](mailto:sbake021@odu.edu)

---

