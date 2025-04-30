# CPU Scheduler Simulation (FIFO and SJF)

This program simulates **CPU scheduling** using two major algorithms:

- **FIFO** (First-In-First-Out)
- **SJF** (Shortest Job First)

It reads **500 processes** from an input file containing **arrival times** and **CPU burst times**, schedules them based on the chosen policy, and computes important performance metrics such as **waiting time**, **turnaround time**, **response time**, **CPU utilization**, and **throughput**.

---
https://youtu.be/0kP-x2PfL1c
## 📂 Files

| File | Purpose |
| :--- | :------ |
| `cpu_scheduler.cpp` | Main simulation program |
| `datafile1.txt` | Input file containing 500 arrival and burst times (one per line) |
| `FIFOoutput.txt` | Example output for FIFO simulation |
| `outputSJF.txt` | Example output for SJF simulation |

---

## 📈 Scheduling Algorithms Implemented

### FIFO (First-In-First-Out)

- **Processes are scheduled in the order they arrive**.
- No preemption — once a process starts running, it runs to completion.
- Simulates a **simple batch system**.

### SJF (Shortest Job First)

- **Among ready processes**, the one with the **shortest CPU burst time** is scheduled first.
- Non-preemptive: the shortest job is chosen when the CPU becomes idle.
- **Optimizes average waiting time** but may cause longer jobs to wait.

---

## ⚙️ How the Program Works

1. **Load processes**:
  - Reads from `datafile1.txt`.
  - Each line has two numbers: **arrival time** and **burst time**.

2. **Scheduling simulation**:
  - If FIFO is selected, processes are run by order of arrival.
  - If SJF is selected, processes with the shortest burst time among ready processes are scheduled first.

3. **Metrics Calculated**:
  - **Waiting Time**: Time a process spends waiting in the ready queue.
  - **Turnaround Time**: Time from arrival to completion.
  - **Response Time**: Time from arrival to first CPU execution.
  - **CPU Utilization**: Percentage of time the CPU was actually working.
  - **Throughput**: Average burst units completed per process.

4. **Display results**:
  - Metrics are printed to the console at the end of the simulation.

---

## 🖥️ How to Run

### 1. Build the Program

Compile using `g++` with C++17 support:

```bash
g++ -std=c++17 -o cpu_scheduler cpu_scheduler.cpp
```

### 2. Prepare Input

Ensure the file `datafile1.txt` exists in the correct location (adjust the path if needed).

Each line should have two numbers separated by whitespace:

```
0 5
1 3
2 8
...
```
- First number: Arrival Time
- Second number: CPU Burst Time

At least **500 lines** are expected.

### 3. Run the Program

Execute the program:

```bash
./cpu_scheduler
```

You will be prompted:

```
Select Scheduling Algorithm (default FIFO):
0 - FIFO (First In First Out)
1 - SJF (Shortest Job First)
Enter your choice (press Enter for FIFO):
```

- Enter `0` and press Enter for **FIFO**.
- Enter `1` and press Enter for **SJF**.
- If invalid input or blank, defaults to FIFO.

---

## 📊 Example Output

```
Select Scheduling Algorithm (default FIFO):
0 - FIFO (First In First Out)
1 - SJF (Shortest Job First)
Enter your choice (press Enter for FIFO): 0

Total elapsed time: 15700
Throughput: 30.5 burst units/process
CPU Utilization: 98.5%
Average Waiting Time: 12.3
Average Turnaround Time: 17.5
Average Response Time: 12.3
```

(Example values — actual results will vary based on input.)

---

## 📚 Concepts Illustrated

- **Scheduling Algorithms**: FIFO vs SJF behavior comparison
- **Queues and Deques**: Managing ready and waiting processes
- **Priority Queues**: Managing jobs arriving over time
- **Performance Metrics**:
  - CPU Utilization
  - Average Waiting Time
  - Turnaround Time
  - Response Time
- **Resource Management**:
  - Dynamic memory for processes
  - Careful deletion to avoid memory leaks

---

## ⚠️ Notes and Assumptions

- **Processes are independent**: No dependencies between processes.
- **Non-preemptive scheduling**: Once a process starts, it runs until completion.
- **No context-switching overhead** is modeled.
- Assumes **input data** is clean and well-formatted.

---

## 🛠️ Future Improvements (Optional)

- Add **preemptive SJF (Shortest Remaining Time First)**.
- Support **Round Robin** scheduling with time slices.
- Add **context-switching costs**.
- Generate **Gantt charts** for visual scheduling diagrams.
- Output detailed process-by-process logs.

---

## ✍️ Author


**Caiden Petty**  
Computer Science Student  
Old Dominion University  
📧 [cpett007@odu.edu](mailto:cpett007@odu.edu)
---



