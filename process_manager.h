#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define MAX_PROCESSES 50
#define PROCESS_NAME_LEN 20

// Process state enumeration
typedef enum {
    NEW = 0,
    READY,
    RUNNING,
    WAITING,
    TERMINATED
} ProcessState;

// Process Control Block structure
typedef struct PCB {
    int pid;                    // Process ID
    char name[PROCESS_NAME_LEN]; // Process name
    ProcessState state;         // Process state
    int priority;              // Priority
    int arrival_time;          // Arrival time
    int burst_time;            // Burst time
    int remaining_time;        // Remaining burst time
    int memory_size;           // Required memory size
    int memory_addr;           // Allocated memory address
    int waiting_time;          // Waiting time
    int turnaround_time;       // Turnaround time
    struct PCB* next;          // Pointer to next process
} PCB;

// Process manager structure
typedef struct {
    PCB processes[MAX_PROCESSES];
    int process_count;
    int current_pid;
    PCB* ready_queue;          // Ready queue
    PCB* running_process;      // Currently running process
} ProcessManager;

// Function declarations
ProcessManager* init_process_manager();
PCB* create_process(ProcessManager* pm, const char* name, int priority, int burst_time, int memory_size);
PCB* create_process_manual(ProcessManager* pm, int pid, const char* name, int priority, int burst_time, int memory_size);
void add_to_ready_queue(ProcessManager* pm, PCB* pcb);
PCB* remove_from_ready_queue(ProcessManager* pm);
void update_process_state(PCB* pcb, ProcessState new_state);
void print_process_status(ProcessManager* pm);
void print_process_details(PCB* pcb);
void free_process_manager(ProcessManager* pm);
void auto_generate_processes(ProcessManager* pm, int count);

#endif