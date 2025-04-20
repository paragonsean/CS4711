// combined_scheduler.cpp - Updated to match CS471 Project Requirements with Priority Queue Enhancement and Refactored Simulation

#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include <algorithm>
#include <string>
#include <iomanip>

using namespace std;

struct Process {
    int pid, arrivalTime, cpuBurst;
    int startTime = -1, finishTime = -1;
    int waitingTime = 0, turnaroundTime = 0, responseTime = -1;
    int remainingTime;
    bool dispatched = false; // mark to avoid rechecking
};

const int NUM_PROCESSES = 500;

// Using priority_queue for job management based on arrival time
struct ArrivalCompare {
    bool operator()(const Process& a, const Process& b) {
        return a.arrivalTime > b.arrivalTime; // min-heap by arrival time
    }
};
priority_queue<Process, vector<Process>, ArrivalCompare> jobQueue;

vector<Process*> readyQueue;
vector<Process> dispatchedJobs;
vector<Process> terminated;

bool loadProcesses(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return false;

    string header;
    getline(file, header); // skip header

    for (int i = 0; i < NUM_PROCESSES; ++i) {
        Process p;
        file >> p.arrivalTime >> p.cpuBurst;
        p.pid = i + 1;
        p.remainingTime = p.cpuBurst;
        if (!file) return false;
        jobQueue.push(p);
    }
    return true;
}

void moveArrivedProcesses(int currentTime) {
    // As jobQueue is a min-heap, we'll use a temp buffer to collect non-dispatched jobs
    vector<Process> buffer;
    while (!jobQueue.empty()) {
        Process top = jobQueue.top();
        jobQueue.pop();

        if (top.dispatched) {
            buffer.push_back(top);
            continue;
        }

        if (top.arrivalTime == currentTime) {
            Process dispatched = top;
            dispatched.dispatched = true;
            readyQueue.push_back(new Process(dispatched));
            dispatchedJobs.push_back(dispatched);
        } else {
            buffer.push_back(top);
        }
    }
    for (const auto& p : buffer) jobQueue.push(p);
}

Process* selectFIFO() {
    if (!readyQueue.empty()) {
        Process* p = readyQueue.front();
        readyQueue.erase(readyQueue.begin());
        return p;
    }
    return nullptr;
}

Process* selectSJF() {
    if (readyQueue.empty()) return nullptr;
    auto shortest = min_element(readyQueue.begin(), readyQueue.end(),
        [](Process* a, Process* b) { return a->remainingTime < b->remainingTime; });
    Process* selected = *shortest;
    readyQueue.erase(shortest);
    return selected;
}

void updateWaitingAndTurnaround() {
    for (auto* p : readyQueue) {
        p->waitingTime++;
        p->turnaroundTime++;
    }
}

void finalizeProcess(Process*& running, int currentTime, int& totalBurst) {
    running->finishTime = currentTime + 1;
    terminated.push_back(*running);
    totalBurst += running->cpuBurst;
    delete running;
    running = nullptr;
}

struct Stats {
    double elapsedTime = 0, throughput = 0, cpuUtil = 0;
    double avgWait = 0, avgTurnaround = 0, avgResponse = 0;
};

Stats calculateStats(int time, int totalBurst) {
    Stats s;
    s.elapsedTime = time;
    s.throughput = static_cast<double>(NUM_PROCESSES) / time;
    s.cpuUtil = static_cast<double>(totalBurst) / time;

    double totalWait = 0, totalTurn = 0, totalResp = 0;
    for (const auto& p : terminated) {
        totalWait += p.waitingTime;
        totalTurn += p.turnaroundTime;
        totalResp += p.responseTime;
    }

    s.avgWait = totalWait / NUM_PROCESSES;
    s.avgTurnaround = totalTurn / NUM_PROCESSES;
    s.avgResponse = totalResp / NUM_PROCESSES;
    return s;
}

Stats runSimulation(int alg) {
    int time = 0;
    Process* running = nullptr;
    int completed = 0;
    int totalBurst = 0;

    while (completed < NUM_PROCESSES) {
        moveArrivedProcesses(time);

        if (!running) {
            running = (alg == 0) ? selectFIFO() : selectSJF();
            if (running) {
                running->startTime = time;
                running->responseTime = time - running->arrivalTime;
            }
        }

        updateWaitingAndTurnaround();

        if (running) {
            running->remainingTime--;
            running->turnaroundTime++;

            if (running->remainingTime == 0) {
                finalizeProcess(running, time, totalBurst);
                completed++;
            }
        }

        ++time;
    }

    return calculateStats(time, totalBurst);
}

void printStats(const string& label, const Stats& s) {
    cout << "\n--- " << label << " ---\n";
    cout << fixed << setprecision(2);
    cout << "Total Elapsed Time:  " << s.elapsedTime << endl;
    cout << "Throughput:          " << s.throughput << endl;
    cout << "CPU Utilization:     " << s.cpuUtil * 100 << "%\n";
    cout << "Avg Waiting Time:    " << s.avgWait << endl;
    cout << "Avg Turnaround Time: " << s.avgTurnaround << endl;
    cout << "Avg Response Time:   " << s.avgResponse << endl;
}

int main() {
    if (!loadProcesses("datafile1.txt")) {
        cerr << "Failed to load datafile1.txt\n";
        return 1;
    }

    cout << "Simulating FIFO..." << endl;
    Stats fifoStats = runSimulation(0);
    printStats("FIFO Scheduling", fifoStats);

    // Clear and reload for SJF
    while (!jobQueue.empty()) jobQueue.pop();
    for (auto* p : readyQueue) delete p;
    readyQueue.clear();
    terminated.clear();
    dispatchedJobs.clear();

    if (!loadProcesses("datafile1.txt")) return 1;

    cout << "\nSimulating SJF..." << endl;
    Stats sjfStats = runSimulation(1);
    printStats("SJF Scheduling", sjfStats);

    return 0;
}
