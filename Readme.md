# 🖥️ Operating Systems Project Suite

This repository contains two major simulations demonstrating fundamental operating system concepts:

1. **CPU Scheduler** — Simulates FIFO and SJF scheduling algorithms  
   📺 [Watch Demo](https://youtu.be/0kP-x2PfL1c)

2. **Producer-Consumer** — Solves the bounded buffer problem using POSIX threads, semaphores, and shared memory  
   📺 [Watch Demo](https://youtu.be/lCTqOw4wVdc)

---

## 📂 Directory Structure

```
.
├── Makefile                         # Unified Makefile to build both projects
├── Readme.md                        # Root README (this file)
├── cpuscheduler/                    # CPU Scheduling simulation
│   ├── FIFOoutput.txt               # Example FIFO output
│   ├── cpu_scheduler.cpp            # Basic scheduler (FIFO & SJF)
│   ├── cpu_scheduler_advanced.cpp   # Advanced scheduler using Visitor Pattern
│   ├── datafile1.txt                # Input: arrival and burst times
│   ├── outputSJF.txt                # Example SJF output
│   └── readme.md                    # CPU Scheduler-specific documentation
└── producer-consumer/               # Producer-Consumer shared memory simulation
    ├── Readme.md                    # Producer-Consumer-specific documentation
    ├── producer_consumer.cpp        # Optimized version with pthreads
    ├── producer_consumer_original.cpp # Original baseline implementation
    ├── report/                      # Performance analysis and visualizations
    │   ├── README.md
    │   ├── Untitled.ipynb
    │   ├── Untitled1.ipynb
    │   ├── output_2_0.png
    │   ├── output_2_1.png
    │   ├── output_2_2.png
    │   ├── output_5_0.png
    │   ├── results_timing_all.csv
    │   ├── results_timing_all_fast.csv
    │   └── results_timing_allnormal.csv
    ├── test.sh                      # Test script: speed & throughput
    ├── test2.sh                     # Alternate configuration test
    └── test3.sh                     # Additional benchmark tests
```

---

## ⚙️ Build Instructions

To build both projects from the root directory, run:

```bash
make
```

This will:

- Compile `cpuscheduler/cpu_scheduler.cpp` using `g++`
- Compile `producer-consumer/producer_consumer.cpp` using `clang++ -pthread`

Both use the **C++17** standard and optimization flags for performance.

---

## 🚀 Running the Simulations

### 🧠 CPU Scheduler

To run the CPU scheduler (FIFO and SJF):

```bash
make run_scheduler
```

This program loads `datafile1.txt`, simulates the scheduling logic, and prints:

- Turnaround Time
- Waiting Time
- Response Time
- Throughput

> Ensure `datafile1.txt` contains **500 lines** of arrival and burst time pairs.

---

### 🔄 Producer-Consumer with Shared Memory

To run the producer-consumer simulation:

```bash
make run_producer
```

Or run manually:

```bash
./producer-consumer/producer_consumer_shared_memory <sleep_time> <num_producers> <num_consumers>
```

**Example:**

```bash
./producer-consumer/producer_consumer_shared_memory 10 2 2
```

This runs for 10 seconds with 2 producers and 2 consumers using:

- Shared memory (`shm_open`, `mmap`)
- Semaphores (`sem_wait`, `sem_post`)
- Mutexes for synchronization

---

## 🧹 Cleaning Up

To remove compiled binaries:

```bash
make clean
```

Removes:

- `cpuscheduler/cpu_scheduler`
- `producer-consumer/producer_consumer_shared_memory`

---

## 📘 Concepts Covered

### 🧠 CPU Scheduler

- **FIFO (First-In-First-Out)** Scheduling
- **SJF (Shortest Job First)** Scheduling
- Process metrics:
  - Waiting Time
  - Turnaround Time
  - Response Time
- CPU Utilization and Throughput Measurement

### 🔄 Producer-Consumer

- POSIX **multi-threading** using `pthread`
- Thread synchronization using **semaphores** and **mutex locks**
- **Circular buffer** implementation in shared memory
- Inter-process communication via:
  - `shm_open`
  - `mmap`
  - `ftruncate`

---

## 🎓 Educational Context

This project was developed as part of:

**CS471 – Operating Systems**  
**Old Dominion University**  
**Spring 2025**

It demonstrates **real-world OS principles** such as scheduling, synchronization, inter-process communication, and concurrency management.

---

## ✍️ Author

**Sean Baker**  
Computer Science Student  
Old Dominion University  
📧 [sbake021@odu.edu](mailto:sbake021@odu.edu)

---

## 📜 License

This project is intended for **academic and educational use only**.

---
