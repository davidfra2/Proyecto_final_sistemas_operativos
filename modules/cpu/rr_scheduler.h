#ifndef RR_SCHEDULER_H
#define RR_SCHEDULER_H

#include "pcb.h"
#include <queue>
#include <vector>
#include <iostream>

class RR_Scheduler {
private:
    std::queue<PCB> ready;
    std::vector<PCB> all;
    int quantum;
    int next_pid;
    PCB* running;
    int tick_counter;
    int quantum_used;
public:
    RR_Scheduler(int q);
    int create_process(int bursts);
    void print_processes();
    void kill_process(int pid);
    void tick();
};

#endif
