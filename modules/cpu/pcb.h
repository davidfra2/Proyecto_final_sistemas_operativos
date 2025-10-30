#ifndef PCB_H
#define PCB_H
#include <string>

enum ProcState { NEW, READY, RUNNING, BLOCKED, TERMINATED };

struct PCB {
    int pid;
    ProcState state;
    int bursts_remaining; // CPU bursts remaining (simple model)
    int arrival_time;
    int start_time;
    int finish_time;
};

#endif
