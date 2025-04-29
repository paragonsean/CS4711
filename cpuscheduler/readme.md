# 🧠 CPU Scheduling Simulator – FIFO & SJF

This project is a C++ simulation of CPU scheduling algorithms, focusing on **First-In-First-Out (FIFO)** and **Shortest Job First (SJF)** strategies. It processes a workload of 500 jobs, analyzing performance based on response time, turnaround time, CPU utilization, and more.

---

## 📁 File Structure


---

## 🚀 How It Works
https://youtu.be/0kP-x2PfL1c
The simulation reads a set of processes from a file, queues them based on their arrival times, and schedules them using FIFO and SJF algorithms in two separate runs. Each run tracks and outputs critical performance metrics.

### ✅ Features
- Simulates 500 processes (configurable via `NUM_PROCESSES`)
- Uses a **priority queue** to manage job arrival order
- Maintains **readyQueue** for CPU scheduling
- Implements:
  - **FIFO (First-In-First-Out)**
  - **SJF (Shortest Job First)** (non-preemptive)
- Tracks:
  - Waiting time
  - Turnaround time
  - Response time
  - CPU Utilization
  - Throughput

---

## 🖥️ Input Format

The simulator reads from a file named `datafile1.txt`.

### Expected Format:


- No header is required, but if present, the first line is skipped.
- The file must contain exactly `NUM_PROCESSES` (500) lines of integer pairs.

---

## 🧪 Running the Simulation

### 🔧 Compile
Use any modern C++ compiler:

```bash
g++ -std=c++17 -o scheduler combined_scheduler.cpp

