#include "queues.hpp"
#include <fstream>
#include <iostream>
#include <cstdlib>

// ------------------ Global Variable Definitions ------------------
int Computation_start  = 0;
int Computation_end    = 0;
int Computation_idle   = 0;

evalPointer evals[MAX_ALGORITHM_NUM];
int         cur_eval_num = 0;

processPointer jobQueue[MAX_PROCESS_NUM];
int            cur_proc_num_JQ = 0;

processPointer cloneJobQueue[MAX_PROCESS_NUM];
int            cur_proc_num_clone_JQ = 0;

processPointer readyQueue[MAX_PROCESS_NUM];
int            cur_proc_num_RQ = 0;

processPointer waitingQueue[MAX_PROCESS_NUM];
int            cur_proc_num_WQ = 0;

processPointer terminated[MAX_PROCESS_NUM];
int            cur_proc_num_T = 0;

processPointer runningProcess = nullptr;
int            timeConsumed   = 0;

// ------------------ Evaluation Routines ------------------
void init_evals() {
    cur_eval_num = 0;
    for(int i = 0; i < MAX_ALGORITHM_NUM; i++){
        evals[i] = nullptr;
    }
}

void clear_evals() {
    for(int i = 0; i < MAX_ALGORITHM_NUM; i++){
        free(evals[i]);
        evals[i] = nullptr;
    }
    cur_eval_num = 0;
}

// ------------------ Job Queue (JQ) ------------------
void init_JQ() {
    cur_proc_num_JQ = 0;
    for(int i=0; i<MAX_PROCESS_NUM; i++){
        jobQueue[i] = nullptr;
    }
}

int getProcByPid_JQ(int givenPid) {
    for(int i=0; i<cur_proc_num_JQ; i++){
        if(jobQueue[i]->pid == givenPid) return i;
    }
    return -1;
}

void insertInto_JQ(processPointer proc) {
    if(cur_proc_num_JQ < MAX_PROCESS_NUM) {
        if(getProcByPid_JQ(proc->pid) != -1){
            printf("<ERROR> Process pid:%d is already in Job Queue\n", proc->pid);
            return;
        }
        jobQueue[cur_proc_num_JQ++] = proc;
    } else {
        puts("<ERROR> Job Queue is full");
    }
}

processPointer removeFrom_JQ(processPointer proc) {
    if(cur_proc_num_JQ>0) {
        int idx = getProcByPid_JQ(proc->pid);
        if(idx == -1) {
            printf("<ERROR> Cannot find pid:%d in Job Queue\n", proc->pid);
            return nullptr;
        }
        processPointer removed = jobQueue[idx];
        for(int i=idx; i<cur_proc_num_JQ-1; i++){
            jobQueue[i] = jobQueue[i+1];
        }
        jobQueue[cur_proc_num_JQ-1] = nullptr;
        cur_proc_num_JQ--;
        return removed;
    } else {
        puts("<ERROR> Job Queue empty");
        return nullptr;
    }
}

void clear_JQ() {
    for(int i=0; i<MAX_PROCESS_NUM; i++){
        free(jobQueue[i]);
        jobQueue[i] = nullptr;
    }
    cur_proc_num_JQ=0;
}

// ------------------ Cloning the JQ ------------------
void clone_JQ() {
    for(int i=0; i<MAX_PROCESS_NUM; i++){
        cloneJobQueue[i] = nullptr;
    }
    for(int i=0; i<cur_proc_num_JQ; i++){
        processPointer newProcess = (processPointer)malloc(sizeof(myProcess));
        *newProcess = *jobQueue[i];
        cloneJobQueue[i] = newProcess;
    }
    cur_proc_num_clone_JQ = cur_proc_num_JQ;
}

void loadClone_JQ() {
    clear_JQ();
    for(int i=0; i<cur_proc_num_clone_JQ; i++){
        processPointer newProcess = (processPointer)malloc(sizeof(myProcess));
        *newProcess = *cloneJobQueue[i];
        jobQueue[i] = newProcess;
    }
    cur_proc_num_JQ = cur_proc_num_clone_JQ;
}

void clearClone_JQ() {
    for(int i=0; i<MAX_PROCESS_NUM; i++){
        free(cloneJobQueue[i]);
        cloneJobQueue[i] = nullptr;
    }
}

// ------------------ Ready Queue (RQ) ------------------
void init_RQ() {
    cur_proc_num_RQ=0;
    for(int i=0; i<MAX_PROCESS_NUM; i++){
        readyQueue[i] = nullptr;
    }
}

int getProcByPid_RQ(int pid) {
    for(int i=0; i<cur_proc_num_RQ; i++){
        if(readyQueue[i]->pid == pid) return i;
    }
    return -1;
}

void insertInto_RQ(processPointer proc) {
    if(cur_proc_num_RQ < MAX_PROCESS_NUM) {
        if(getProcByPid_RQ(proc->pid) != -1) {
            printf("<ERROR> Process pid:%d already in RQ\n", proc->pid);
            return;
        }
        readyQueue[cur_proc_num_RQ++] = proc;
    } else {
        puts("<ERROR> Ready Queue full");
    }
}

processPointer removeFrom_RQ(processPointer proc) {
    if(cur_proc_num_RQ>0){
        int idx = getProcByPid_RQ(proc->pid);
        if(idx == -1){
            printf("<ERROR> No process pid:%d in RQ\n", proc->pid);
            return nullptr;
        }
        processPointer removed = readyQueue[idx];
        for(int i=idx; i<cur_proc_num_RQ-1; i++){
            readyQueue[i] = readyQueue[i+1];
        }
        readyQueue[cur_proc_num_RQ-1] = nullptr;
        cur_proc_num_RQ--;
        return removed;
    } else {
        puts("<ERROR> RQ is empty");
        return nullptr;
    }
}

void clear_RQ() {
    for(int i=0; i<MAX_PROCESS_NUM; i++){
        free(readyQueue[i]);
        readyQueue[i] = nullptr;
    }
    cur_proc_num_RQ=0;
}

// ------------------ Waiting Queue (WQ) ------------------
void init_WQ() {
    cur_proc_num_WQ=0;
    for(int i=0; i<MAX_PROCESS_NUM; i++){
        waitingQueue[i] = nullptr;
    }
}

int getProcByPid_WQ(int pid) {
    for(int i=0; i<cur_proc_num_WQ; i++){
        if(waitingQueue[i]->pid == pid) return i;
    }
    return -1;
}

void insertInto_WQ(processPointer proc) {
    if(cur_proc_num_WQ<MAX_PROCESS_NUM){
        if(getProcByPid_WQ(proc->pid) != -1){
            printf("<ERROR> Process pid:%d in WQ\n", proc->pid);
            return;
        }
        waitingQueue[cur_proc_num_WQ++] = proc;
    } else {
        puts("<ERROR> WQ full");
    }
}

processPointer removeFrom_WQ(processPointer proc) {
    if(cur_proc_num_WQ>0){
        int idx = getProcByPid_WQ(proc->pid);
        if(idx == -1){
            printf("<ERROR> No process pid:%d in WQ\n", proc->pid);
            return nullptr;
        }
        processPointer removed = waitingQueue[idx];
        for(int i=idx; i<cur_proc_num_WQ-1; i++){
            waitingQueue[i] = waitingQueue[i+1];
        }
        waitingQueue[cur_proc_num_WQ-1] = nullptr;
        cur_proc_num_WQ--;
        return removed;
    } else {
        puts("<ERROR> WQ empty");
        return nullptr;
    }
}

void clear_WQ() {
    for(int i=0; i<MAX_PROCESS_NUM; i++){
        free(waitingQueue[i]);
        waitingQueue[i] = nullptr;
    }
    cur_proc_num_WQ=0;
}

// ------------------ Terminated Queue (T) ------------------
void init_T() {
    cur_proc_num_T=0;
    for(int i=0; i<MAX_PROCESS_NUM; i++){
        terminated[i] = nullptr;
    }
}

void clear_T() {
    for(int i=0; i<MAX_PROCESS_NUM; i++){
        free(terminated[i]);
        terminated[i] = nullptr;
    }
    cur_proc_num_T=0;
}

void insertInto_T(processPointer proc) {
    if(cur_proc_num_T<MAX_PROCESS_NUM){
        terminated[cur_proc_num_T++] = proc;
    } else {
        puts("<ERROR> Cannot terminate process");
    }
}

// ------------------ Scheduling Algorithms ------------------
processPointer FCFS_alg() {
    // If no ready processes, keep the CPU as is
    if(cur_proc_num_RQ <= 0) {
        return runningProcess;
    }
    processPointer earliest = readyQueue[0];
    if(earliest) {
        if(runningProcess) {
            // No preemption in FCFS
            return runningProcess;
        }
        return removeFrom_RQ(earliest);
    }
    return runningProcess;
}

processPointer SJF_alg() {
    // If no ready processes, keep the CPU as is
    if(cur_proc_num_RQ <= 0) {
        return runningProcess;
    }
    // Find the min CPUremainingTime in RQ
    processPointer shortest = readyQueue[0];
    for(int i=1; i<cur_proc_num_RQ; i++){
        if(readyQueue[i]->CPUremainingTime < shortest->CPUremainingTime){
            shortest = readyQueue[i];
        } else if(readyQueue[i]->CPUremainingTime == shortest->CPUremainingTime){
            if(readyQueue[i]->arrivalTime < shortest->arrivalTime){
                shortest = readyQueue[i];
            }
        }
    }
    if(runningProcess) {
        // No preemption in non-preemptive SJF
        return runningProcess;
    }
    return removeFrom_RQ(shortest);
}

processPointer schedule(int alg) {
    switch(alg) {
        case FCFS: return FCFS_alg();
        case SJF:  return SJF_alg();
        default:   return nullptr;
    }
}
