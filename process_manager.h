#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define MAX_PROCESSES 50
#define PROCESS_NAME_LEN 20

// 进程状态枚举
typedef enum {
    NEW = 0,
    READY,
    RUNNING,
    WAITING,
    TERMINATED
} ProcessState;

// 进程控制块结构
typedef struct PCB {
    int pid;                    // 进程ID
    char name[PROCESS_NAME_LEN]; // 进程名
    ProcessState state;         // 进程状态
    int priority;              // 优先级
    int arrival_time;          // 到达时间
    int burst_time;            // 执行时间
    int remaining_time;        // 剩余执行时间
    int memory_size;           // 需要的内存大小
    int memory_addr;           // 分配的内存地址
    int waiting_time;          // 等待时间
    int turnaround_time;       // 周转时间
    struct PCB* next;          // 指向下一个进程的指针
} PCB;

// 进程管理器结构
typedef struct {
    PCB processes[MAX_PROCESSES];
    int process_count;
    int current_pid;
    PCB* ready_queue;          // 就绪队列
    PCB* running_process;      // 当前运行的进程
} ProcessManager;

// 函数声明
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