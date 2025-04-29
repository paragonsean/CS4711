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
#include <memory>
#include <functional>

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

class Dispatcher;

class SchedulerVisitor {
public:
    virtual void visit(Dispatcher& dispatcher) = 0;
};

class Dispatcher {
public:
    priority_queue<Process, vector<Process>, function<bool(Process, Process)>> jobQueue;
    deque<unique_ptr<Process>> arrivalQueue;
    deque<unique_ptr<Process>> readyQueue;
    vector<Process> terminated;

    Dispatcher() : jobQueue([](Process a, Process b) {
        return a.arrivalTime > b.arrivalTime;
    }) {}

    vector<Process> loadProcesses(const string& filename) {
        vector<Process> processes;
        ifstream file(filename);
        string line;
        getline(file, line);  // Skip header

        int pid = 0;
        while (getline(file, line) && processes.size() < NUM_PROCESSES) {
            istringstream iss(line);
            double arrival, burst;
            if (iss >> arrival >> burst) {
                processes.push_back({pid++, arrival, burst});
            }
        }
        return processes;
    }

    void loadIntoJobQueue(const vector<Process>& processes) {
        while (!jobQueue.empty()) jobQueue.pop();
        for (const auto& p : processes) {
            jobQueue.push(p);
        }
    }

    void moveArrivedProcesses(double currentTime) {
        while (!jobQueue.empty() && jobQueue.top().arrivalTime <= currentTime) {
            auto p = make_unique<Process>(jobQueue.top());
            readyQueue.push_back(move(p));
            arrivalQueue.push_back(make_unique<Process>(jobQueue.top()));
            jobQueue.pop();
        }
    }

    void executeProcess(unique_ptr<Process>& p, double& currentTime) {
        if (currentTime < p->arrivalTime)
            currentTime = p->arrivalTime;

        p->startTime = currentTime;
        p->responseTime = currentTime - p->arrivalTime;
        currentTime += p->burstTime;
        p->finishTime = currentTime;
        p->turnaroundTime = p->finishTime - p->arrivalTime;
        p->waitingTime = p->turnaroundTime - p->burstTime;

        terminated.push_back(*p);
    }

    void resetState() {
        readyQueue.clear();
        arrivalQueue.clear();
        terminated.clear();
        while (!jobQueue.empty()) jobQueue.pop();
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

            auto p = move(dispatcher.readyQueue.front());
            dispatcher.readyQueue.pop_front();
            dispatcher.executeProcess(p, currentTime);
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
                [](const unique_ptr<Process>& a, const unique_ptr<Process>& b) {
                    return a->burstTime < b->burstTime;
                });

            auto p = move(*shortestJob);
            dispatcher.readyQueue.erase(shortestJob);
            dispatcher.executeProcess(p, currentTime);
        }
    }
};

void calculateStats(const Dispatcher& dispatcher, Stats& s) {
    double totalTime = 0, sumBurst = 0, sumWait = 0, sumTurnaround = 0, sumResponse = 0;

    for (const auto& p : dispatcher.terminated) {
        if (p.finishTime > totalTime)
            totalTime = p.finishTime;
        sumBurst += p.burstTime;
        sumWait += p.waitingTime;
        sumTurnaround += p.turnaroundTime;
        sumResponse += p.responseTime;
    }

    s.elapsedTime = totalTime;
    s.throughput = NUM_PROCESSES / totalTime;
    s.cpuUtilization = (sumBurst / totalTime) * 100;
    s.avgWaitingTime = sumWait / NUM_PROCESSES;
    s.avgTurnaroundTime = sumTurnaround / NUM_PROCESSES;
    s.avgResponseTime = sumResponse / NUM_PROCESSES;
}

void displayStats(const string& name, const Stats& s) {
    cout << "\n" << name << " Scheduling Results:\n";
    cout << "Total elapsed time: " << s.elapsedTime << endl;
    cout << "Throughput: " << s.throughput << " processes/unit time" << endl;
    cout << "CPU Utilization: " << s.cpuUtilization << "%" << endl;
    cout << "Average Waiting Time: " << s.avgWaitingTime << endl;
    cout << "Average Turnaround Time: " << s.avgTurnaroundTime << endl;
    cout << "Average Response Time: " << s.avgResponseTime << endl;
}

int main() {
    Stats stats;
    Dispatcher dispatcher;

    auto originalProcesses = dispatcher.loadProcesses("/Users/roto/CLionProjects/CS4711/cpuscheduler/datafile1.txt");

    // FIFO
    cout << "Running FIFO Scheduling...\n";
    dispatcher.loadIntoJobQueue(originalProcesses);
    FCFSScheduler fifoScheduler;
    dispatcher.accept(fifoScheduler);
    calculateStats(dispatcher, stats);
    displayStats("FIFO", stats);

    dispatcher.resetState();

    // SJF
    cout << "\nRunning SJF Scheduling...\n";
    dispatcher.loadIntoJobQueue(originalProcesses);
    SJFScheduler sjfScheduler;
    dispatcher.accept(sjfScheduler);
    calculateStats(dispatcher, stats);
    displayStats("SJF", stats);

    return 0;
}
