#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process_manager.h"
#include "memory_manager.h"

#define TIME_QUANTUM 3  // Time quantum size

// Scheduler structure
typedef struct {
    ProcessManager* pm;
    MemoryManager* mm;
    int current_time;
    int time_quantum;
} Scheduler;

// Scheduling algorithm enumeration
typedef enum {
    ROUND_ROBIN = 0,
    FCFS,
    PRIORITY
} SchedulingAlgorithm;

// Function declarations
Scheduler* init_scheduler(ProcessManager* pm, MemoryManager* mm);
void schedule_processes(Scheduler* scheduler, SchedulingAlgorithm algo);
void round_robin_schedule(Scheduler* scheduler);
void fcfs_schedule(Scheduler* scheduler);
void priority_schedule(Scheduler* scheduler);
void execute_process(Scheduler* scheduler, PCB* pcb);
void simulate_time_passage(Scheduler* scheduler);
void print_system_status(Scheduler* scheduler);
void save_execution_log(Scheduler* scheduler, const char* filename);
void load_execution_log(Scheduler* scheduler, const char* filename);
void replay_execution(Scheduler* scheduler, const char* filename);
void free_scheduler(Scheduler* scheduler);

#endif