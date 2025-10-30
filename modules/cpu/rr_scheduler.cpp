#include "rr_scheduler.h"
#include <algorithm>
#include <iostream>

RR_Scheduler::RR_Scheduler(int q): quantum(q), next_pid(1), running(nullptr), tick_counter(0), quantum_used(0) {}

int RR_Scheduler::create_process(int bursts){
    PCB p;
    p.pid = next_pid++;
    p.state = READY;
    p.bursts_remaining = bursts;
    p.arrival_time = tick_counter;
    p.start_time = -1;
    p.finish_time = -1;
    ready.push(p);
    all.push_back(p);
    return p.pid;
}

void RR_Scheduler::print_processes(){
    std::cout<<"PID\tState\tBursts"<<std::endl;
    std::queue<PCB> copy = ready;
    while(!copy.empty()){
        PCB p = copy.front(); copy.pop();
        std::cout<<p.pid<<"\tREADY\t"<<p.bursts_remaining<<std::endl;
    }
    if(running) std::cout<<running->pid<<"\tRUNNING\t"<<running->bursts_remaining<<std::endl;
}

void RR_Scheduler::kill_process(int pid){
    // mark terminated in all and remove from ready queue
    for(auto &p: all) if(p.pid==pid){ p.state=TERMINATED; p.finish_time = tick_counter; }
    std::queue<PCB> newq;
    while(!ready.empty()){
        PCB p = ready.front(); ready.pop();
        if(p.pid!=pid) newq.push(p);
    }
    ready = std::move(newq);
    if(running && running->pid==pid){
        running->state = TERMINATED;
        running->finish_time = tick_counter;
        running = nullptr;
    }
    std::cout<<"Killed "<<pid<<std::endl;
}

void RR_Scheduler::tick(){
    tick_counter++;
    // if no running, pop from ready
    if(!running || running->state!=RUNNING){
        if(!ready.empty()){
            PCB p = ready.front(); ready.pop();
            // move p into running (store in all vector slot)
            for(auto &pp: all) if(pp.pid==p.pid){
                pp.state = RUNNING;
                if(pp.start_time==-1) pp.start_time = tick_counter;
                running = &pp;
                break;
            }
            quantum_used = 0;
        } else {
            std::cout<<"[tick "<<tick_counter<<"] CPU idle"<<std::endl;
            return;
        }
    }
    // run one tick
    if(running){
        running->bursts_remaining--;
        quantum_used++;
        std::cout<<"[tick "<<tick_counter<<"] Running PID "<<running->pid<<", bursts left "<<running->bursts_remaining<<", quantum "<<quantum_used<<"/"<<quantum<<std::endl;
        if(running->bursts_remaining<=0){
            running->state = TERMINATED;
            running->finish_time = tick_counter;
            std::cout<<"PID "<<running->pid<<" finished."<<std::endl;
            running = nullptr;
        } else if(quantum_used>=quantum){
            // preempt
            running->state = READY;
            // push copy into ready
            PCB copy = *running;
            ready.push(copy);
            std::cout<<"PID "<<copy.pid<<" preempted, back to ready."<<std::endl;
            running = nullptr;
        }
    }
}
