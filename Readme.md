# Operating Systems Project Suite

This repository contains two major simulations related to core operating system concepts:

1. **CPU Scheduler** — Implements FIFO and SJF scheduling algorithms.  
2. **Producer-Consumer** — Solves the bounded buffer problem using POSIX threads, semaphores, and shared memory.

---

## Directory Structure

```
.
├── Makefile                         # Unified Makefile to build both projects
├── cpuscheduler/                   # CPU scheduling simulation
│   ├── a.out
│   ├── cpu_scheduler.cpp           # Main simulation logic
│   ├── cpu_scheduler.sh            # Shell script (optional)
│   ├── datafile1.txt               # Input data: arrival and burst times
│   ├── readme.md                   # Scheduler-specific documentation
│   └── cpu_scheduler               # Compiled output binary
├── producer_consumer.cpp           # Optional standalone version (if used earlier)
├── producer-consumer/              # Producer-consumer shared memory simulation
│   ├── report                      # report with files
│   ├── producer_consumer.cpp # optimized logic with pthreads
│   ├── producer_consumer_shared_memory.cpp# original code used in video
│   ├── Readme.md                           # Producer-consumer-specific README
│   ├──test.sh            # testing script used to determine speed and throughput
│   └── test2.sh            #additional testing script used to determine speed and throughput
```

---

## Build Instructions

To build both simulations from the root directory, use the Makefile:

```bash
make
```

This will:

- Compile `cpu_scheduler.cpp` using `g++`
- Compile `producer_consumer_shared_memory.cpp` using `clang++ -pthread`  
  Both use the C++17 standard and optimization flags.

---

## Running the Simulations

### CPU Scheduler

To run the CPU scheduling simulation:

```bash
make run_scheduler
```

This executes FIFO and SJF scheduling using data from `datafile1.txt`. Metrics such as turnaround time, response time, and throughput are printed to the console.

> Ensure that `datafile1.txt` exists and contains 500 lines of arrival time and CPU burst pairs.

---

### Producer-Consumer with Shared Memory

To run the shared memory producer-consumer simulation:

```bash
make run_producer
```

Alternatively, run the binary directly:

```bash
./producer-consumer/producer_consumer_shared_memory <sleep_time> <num_producers> <num_consumers>
```

**Example:**

```bash
./producer-consumer/producer_consumer_shared_memory 10 2 2
```

This starts 2 producers and 2 consumers, runs for 10 seconds, and uses POSIX shared memory for buffer coordination.

---

## Cleaning Up

To remove compiled binaries:

```bash
make clean
```

This deletes:

- `cpuscheduler/cpu_scheduler`
- `producer-consumer/producer_consumer_shared_memory`

---

## Concepts Covered

### CPU Scheduler

- First-In-First-Out (FIFO) Scheduling  
- Shortest Job First (SJF) Scheduling  
- Process metrics: waiting time, turnaround time, response time  
- CPU utilization and throughput tracking

### Producer-Consumer

- Multi-threading with POSIX `pthread`  
- Semaphores and mutex locks for thread synchronization  
- Circular buffer implementation in shared memory  
- Inter-process communication using `shm_open`, `mmap`, `ftruncate`

---

## Educational Use

This project was developed for **CS471 – Operating Systems** at **Old Dominion University** in **Spring 2025**. It demonstrates core principles of process scheduling and inter-process/thread communication.

---

## Author

**Sean Baker**  
Computer Science Student  
Old Dominion University  
Email: [sbake021@odu.edu](mailto:sbake021@odu.edu)

---

## License

This project is provided for academic and educational use.

