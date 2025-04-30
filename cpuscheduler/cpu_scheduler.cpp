#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <deque>

using namespace std;

const int NUM_PROCESSES = 500;

struct Process {
    int pid;
    double arrivalTime;
    double burstTime;
    double startTime = -1;
    double finishTime = 0;
    double waitingTime = 0;
    double turnaroundTime = 0;
    double responseTime = -1;
};

struct Stats {
    double elapsedTime;
    double throughput;
    double cpuUtilization;
    double avgWaitingTime;
    double avgTurnaroundTime;
    double avgResponseTime;
};

priority_queue<Process, vector<Process>, function<bool(Process, Process)>> jobQueue(
    [](Process a, Process b) { return a.arrivalTime > b.arrivalTime; }
);
deque<Process*> readyQueue;
vector<Process> terminated;

vector<Process> loadProcesses(const string& filename) {
    vector<Process> processes;
    ifstream file(filename);
    string line;

    getline(file, line);

    int pid = 0;
    while (getline(file, line) && processes.size() < NUM_PROCESSES) {
        istringstream iss(line);
        double arrival, burst;
        if (iss >> arrival >> burst) {
            Process p;
            p.pid = pid++;
            p.arrivalTime = arrival;
            p.burstTime = burst;
            jobQueue.push(p);
            processes.push_back(p);
        }
    }

    return processes;
}

void moveArrivedProcesses(double currentTime) {
    while (!jobQueue.empty() && jobQueue.top().arrivalTime <= currentTime) {
        Process* p = new Process(jobQueue.top());
        readyQueue.push_back(p);
        jobQueue.pop();
    }
}

void simulateFCFS(vector<Process> &processes) {
    double currentTime = 0;

    while (terminated.size() < NUM_PROCESSES) {
        moveArrivedProcesses(currentTime);

        if (readyQueue.empty()) {
            currentTime++;
            continue;
        }

        Process* p = readyQueue.front();
        readyQueue.pop_front();

        if (currentTime < p->arrivalTime)
            currentTime = p->arrivalTime;

        p->startTime = currentTime;
        p->responseTime = currentTime - p->arrivalTime;
        currentTime += p->burstTime;
        p->finishTime = currentTime;
        p->turnaroundTime = p->finishTime - p->arrivalTime;
        p->waitingTime = p->turnaroundTime - p->burstTime;

        terminated.push_back(*p);
        delete p;
    }
}

void simulateSJF(vector<Process> &processes) {
    double currentTime = 0;

    while (terminated.size() < NUM_PROCESSES) {
        moveArrivedProcesses(currentTime);

        if (readyQueue.empty()) {
            currentTime++;
            continue;
        }

        auto shortestJob = min_element(readyQueue.begin(), readyQueue.end(),
            [](Process* a, Process* b) { return a->burstTime < b->burstTime; });

        Process* p = *shortestJob;
        readyQueue.erase(shortestJob);

        if (currentTime < p->arrivalTime)
            currentTime = p->arrivalTime;

        p->startTime = currentTime;
        p->responseTime = currentTime - p->arrivalTime;
        currentTime += p->burstTime;
        p->finishTime = currentTime;
        p->turnaroundTime = p->finishTime - p->arrivalTime;
        p->waitingTime = p->turnaroundTime - p->burstTime;

        terminated.push_back(*p);
        delete p;
    }
}

void calculateStats(Stats &s) {
    double totalTime = 0, sumBurst = 0, sumWait = 0, sumTurnaround = 0, sumResponse = 0;

    for (const Process &p : terminated) {
        if (p.finishTime > totalTime)
            totalTime = p.finishTime;
        sumBurst += p.burstTime;
        sumWait += p.waitingTime;
        sumTurnaround += p.turnaroundTime;
        sumResponse += p.responseTime;
    }

    s.elapsedTime = totalTime;
    s.throughput = sumBurst / NUM_PROCESSES;
    s.cpuUtilization = (sumBurst / totalTime) * 100;
    s.avgWaitingTime = sumWait / NUM_PROCESSES;
    s.avgTurnaroundTime = sumTurnaround / NUM_PROCESSES;
    s.avgResponseTime = sumResponse / NUM_PROCESSES;
}

int main() {
    Stats stats;
    vector<Process> processes = loadProcesses("/Users/roto/CLionProjects/CS4711/cpuscheduler/datafile1.txt");

    cout << "Select Scheduling Algorithm (default FIFO):\n";
    cout << "0 - FIFO (First In First Out)\n";
    cout << "1 - SJF (Shortest Job First)\n";
    cout << "Enter your choice (press Enter for FIFO): ";

    string input;
    getline(cin, input);

    int choice = 0; // default to FIFO
    if (!input.empty()) {
        choice = stoi(input);
    }

    if (choice == 0) {
        simulateFCFS(processes);
    } else if (choice == 1) {
        simulateSJF(processes);
    } else {
        cout << "Invalid choice. Defaulting to FIFO.\n";
        simulateFCFS(processes);
    }

    calculateStats(stats);

    cout << "Total elapsed time: " << stats.elapsedTime << endl;
    cout << "Throughput: " << stats.throughput << " burst units/process" << endl;
    cout << "CPU Utilization: " << stats.cpuUtilization << "%" << endl;
    cout << "Average Waiting Time: " << stats.avgWaitingTime << endl;
    cout << "Average Turnaround Time: " << stats.avgTurnaroundTime << endl;
    cout << "Average Response Time: " << stats.avgResponseTime << endl;

    return 0;
}
