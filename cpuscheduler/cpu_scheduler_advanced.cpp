/*
* ============================================
 * About
 * --------------------------------------------
 * Caiden Petty
 * Computer Science Student
 * Old Dominion University
 * cpett007@odu.edu
 * ============================================
 */
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
    int startTime = -1;
    int finishTime = 0;
    int waitingTime = 0;
    int turnaroundTime = 0;
    int responseTime = -1;
};

struct Stats {
    double elapsedTime;
    double throughput;
    double cpuUtilization;
    double avgWaitingTime;
    double avgTurnaroundTime;
    double avgResponseTime;
};

class Dispatcher;

class SchedulerVisitor {
public:
    virtual void visit(Dispatcher& dispatcher) = 0;
};

class Dispatcher {
public:
    priority_queue<Process, vector<Process>, function<bool(Process, Process)>> jobQueue;
    deque<Process*> arrivalQueue;
    deque<Process*> readyQueue;
    vector<Process> terminated;

    Dispatcher()
        : jobQueue([](Process a, Process b) { return a.arrivalTime > b.arrivalTime; }) {}

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
            arrivalQueue.push_back(p);
            jobQueue.pop();
        }
    }

    void resetState() {
        while (!readyQueue.empty()) {
            delete readyQueue.front();
            readyQueue.pop_front();
        }
        while (!arrivalQueue.empty()) {
            delete arrivalQueue.front();
            arrivalQueue.pop_front();
        }
        while (!jobQueue.empty()) jobQueue.pop();
        terminated.clear();
    }

    void accept(SchedulerVisitor& scheduler) {
        scheduler.visit(*this);
    }
};

class FCFSScheduler : public SchedulerVisitor {
public:
    void visit(Dispatcher& dispatcher) override {
        double currentTime = 0;

        while (dispatcher.terminated.size() < NUM_PROCESSES) {
            dispatcher.moveArrivedProcesses(currentTime);

            if (dispatcher.readyQueue.empty()) {
                currentTime++;
                continue;
            }

            Process* p = dispatcher.readyQueue.front();
            dispatcher.readyQueue.pop_front();

            if (currentTime < p->arrivalTime)
                currentTime = p->arrivalTime;

            p->startTime = currentTime;
            p->responseTime = p->startTime - p->arrivalTime;
            currentTime += p->burstTime;
            p->finishTime = currentTime;
            p->turnaroundTime = p->finishTime - p->arrivalTime;
            p->waitingTime = p->turnaroundTime - p->burstTime;

            dispatcher.terminated.push_back(*p);
            delete p;
        }
    }
};

class SJFScheduler : public SchedulerVisitor {
public:
    void visit(Dispatcher& dispatcher) override {
        double currentTime = 0;

        while (dispatcher.terminated.size() < NUM_PROCESSES) {
            dispatcher.moveArrivedProcesses(currentTime);

            if (dispatcher.readyQueue.empty()) {
                currentTime++;
                continue;
            }

            auto shortestJob = min_element(dispatcher.readyQueue.begin(), dispatcher.readyQueue.end(),
                [](Process* a, Process* b) { return a->burstTime < b->burstTime; });

            Process* p = *shortestJob;
            dispatcher.readyQueue.erase(shortestJob);

            if (currentTime < p->arrivalTime)
                currentTime = p->arrivalTime;

            p->startTime = currentTime;
            p->responseTime = p->startTime - p->arrivalTime;
            currentTime += p->burstTime;
            p->finishTime = currentTime;
            p->turnaroundTime = p->finishTime - p->arrivalTime;
            p->waitingTime = p->turnaroundTime - p->burstTime;

            dispatcher.terminated.push_back(*p);
            delete p;
        }
    }
};

void calculateStats(Dispatcher& dispatcher, Stats& s) {
    double totalTime = 0, sumBurst = 0, sumWait = 0, sumTurnaround = 0, sumResponse = 0;

    for (const Process &p : dispatcher.terminated) {
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
    Dispatcher dispatcher;

    cout << "Running FIFO Scheduling...\n";
    vector<Process> processes = dispatcher.loadProcesses("/Users/roto/CLionProjects/CS4711/cpuscheduler/datafile1.txt");
    FCFSScheduler fifoScheduler;
    dispatcher.accept(fifoScheduler);
    calculateStats(dispatcher, stats);

    cout << "\nFIFO Scheduling Results:\n";
    cout << "Total elapsed time: " << stats.elapsedTime << endl;
    cout << "Throughput: " << stats.throughput << " burst units/process" << endl;
    cout << "CPU Utilization: " << stats.cpuUtilization << "%" << endl;
    cout << "Average Waiting Time: " << stats.avgWaitingTime << endl;
    cout << "Average Turnaround Time: " << stats.avgTurnaroundTime << endl;
    cout << "Average Response Time: " << stats.avgResponseTime << endl;

    dispatcher.resetState();

    cout << "\nRunning SJF Scheduling...\n";
    processes = dispatcher.loadProcesses("/Users/roto/CLionProjects/CS4711/cpuscheduler/datafile1.txt");
    SJFScheduler sjfScheduler;
    dispatcher.accept(sjfScheduler);
    calculateStats(dispatcher, stats);

    cout << "\nSJF Scheduling Results:\n";
    cout << "Total elapsed time: " << stats.elapsedTime << endl;
    cout << "Throughput: " << stats.throughput << " burst units/process" << endl;
    cout << "CPU Utilization: " << stats.cpuUtilization << "%" << endl;
    cout << "Average Waiting Time: " << stats.avgWaitingTime << endl;
    cout << "Average Turnaround Time: " << stats.avgTurnaroundTime << endl;
    cout << "Average Response Time: " << stats.avgResponseTime << endl;

    return 0;
}
