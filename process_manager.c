#include "process_manager.h"
#include <string.h>
#include <time.h>

ProcessManager* init_process_manager() {
    ProcessManager* pm = (ProcessManager*)malloc(sizeof(ProcessManager));
    if (!pm) {
        printf("Error: Process manager memory allocation failed\n");
        return NULL;
    }
    
    pm->process_count = 0;
    pm->current_pid = 1;
    pm->ready_queue = NULL;
    pm->running_process = NULL;
    
    // Initialize process array
    for (int i = 0; i < MAX_PROCESSES; i++) {
        pm->processes[i].pid = -1;
        strcpy(pm->processes[i].name, "");
        pm->processes[i].state = NEW;
        pm->processes[i].priority = 0;
        pm->processes[i].arrival_time = 0;
        pm->processes[i].burst_time = 0;
        pm->processes[i].remaining_time = 0;
        pm->processes[i].memory_size = 0;
        pm->processes[i].memory_addr = -1;
        pm->processes[i].waiting_time = 0;
        pm->processes[i].turnaround_time = 0;
        pm->processes[i].next = NULL;
    }
    
    return pm;
}

PCB* create_process(ProcessManager* pm, const char* name, int priority, int burst_time, int memory_size) {
    if (pm->process_count >= MAX_PROCESSES) {
        printf("Error: Maximum number of processes reached\n");
        return NULL;
    }
    
    PCB* pcb = &pm->processes[pm->process_count];
    pcb->pid = pm->current_pid++;
    strncpy(pcb->name, name, PROCESS_NAME_LEN - 1);
    pcb->name[PROCESS_NAME_LEN - 1] = '\0';
    pcb->state = NEW;
    pcb->priority = priority;
    pcb->arrival_time = time(NULL) % 100; // 使用时间作为到达时间的模拟
    pcb->burst_time = burst_time;
    pcb->remaining_time = burst_time;
    pcb->memory_size = memory_size;
    pcb->memory_addr = -1;
    pcb->waiting_time = 0;
    pcb->turnaround_time = 0;
    pcb->next = NULL;
    
    pm->process_count++;
    
    // 将进程添加到就绪队列
    add_to_ready_queue(pm, pcb);
    update_process_state(pcb, READY);
    
    return pcb;
}

PCB* create_process_manual(ProcessManager* pm, int pid, const char* name, int priority, int burst_time, int memory_size) {
    if (pm->process_count >= MAX_PROCESSES) {
        printf("Error: Maximum number of processes reached\n");
        return NULL;
    }
    
    PCB* pcb = &pm->processes[pm->process_count];
    pcb->pid = pid;
    strncpy(pcb->name, name, PROCESS_NAME_LEN - 1);
    pcb->name[PROCESS_NAME_LEN - 1] = '\0';
    pcb->state = NEW;
    pcb->priority = priority;
    pcb->arrival_time = time(NULL) % 100; // 使用时间作为到达时间的模拟
    pcb->burst_time = burst_time;
    pcb->remaining_time = burst_time;
    pcb->memory_size = memory_size;
    pcb->memory_addr = -1;
    pcb->waiting_time = 0;
    pcb->turnaround_time = 0;
    pcb->next = NULL;
    
    pm->process_count++;
    
    // 将进程添加到就绪队列
    add_to_ready_queue(pm, pcb);
    update_process_state(pcb, READY);
    
    return pcb;
}

void add_to_ready_queue(ProcessManager* pm, PCB* pcb) {
    if (!pm->ready_queue) {
        pm->ready_queue = pcb;
    } else {
        // 简单地添加到队列末尾
        PCB* current = pm->ready_queue;
        while (current->next) {
            current = current->next;
        }
        current->next = pcb;
    }
}

PCB* remove_from_ready_queue(ProcessManager* pm) {
    if (!pm->ready_queue) {
        return NULL;
    }
    
    PCB* pcb = pm->ready_queue;
    pm->ready_queue = pm->ready_queue->next;
    pcb->next = NULL;
    return pcb;
}

void update_process_state(PCB* pcb, ProcessState new_state) {
    pcb->state = new_state;
}

void print_process_status(ProcessManager* pm) {
    printf("\n=== 进程状态 ===\n");
    printf("PID\t名称\t状态\t优先级\t到达时间\t执行时间\t剩余时间\t内存大小\t内存地址\n");
    
    for (int i = 0; i < pm->process_count; i++) {
        PCB* pcb = &pm->processes[i];
        if (pcb->pid != -1) {
            printf("%d\t%s\t", pcb->pid, pcb->name);
            
            switch (pcb->state) {
                case NEW: printf("新建\t"); break;
                case READY: printf("就绪\t"); break;
                case RUNNING: printf("运行\t"); break;
                case WAITING: printf("等待\t"); break;
                case TERMINATED: printf("终止\t"); break;
                default: printf("未知\t"); break;
            }
            
            printf("%d\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n", 
                   pcb->priority, pcb->arrival_time, pcb->burst_time, 
                   pcb->remaining_time, pcb->memory_size, pcb->memory_addr);
        }
    }
    printf("================\n");
}

void print_process_details(PCB* pcb) {
    if (!pcb) return;
    
    printf("进程详情:\n");
    printf("  PID: %d\n", pcb->pid);
    printf("  名称: %s\n", pcb->name);
    printf("  状态: ");
    switch (pcb->state) {
        case NEW: printf("新建\n"); break;
        case READY: printf("就绪\n"); break;
        case RUNNING: printf("运行\n"); break;
        case WAITING: printf("等待\n"); break;
        case TERMINATED: printf("终止\n"); break;
        default: printf("未知\n"); break;
    }
    printf("  优先级: %d\n", pcb->priority);
    printf("  到达时间: %d\n", pcb->arrival_time);
    printf("  执行时间: %d\n", pcb->burst_time);
    printf("  剩余时间: %d\n", pcb->remaining_time);
    printf("  内存大小: %d\n", pcb->memory_size);
    printf("  内存地址: %d\n", pcb->memory_addr);
}

void free_process_manager(ProcessManager* pm) {
    if (pm) {
        free(pm);
    }
}

void auto_generate_processes(ProcessManager* pm, int count) {
    srand(time(NULL));
    
    for (int i = 0; i < count; i++) {
        char name[PROCESS_NAME_LEN];
        sprintf(name, "P%d", pm->current_pid);
        
        int priority = rand() % 10 + 1;  // 1-10之间的优先级
        int burst_time = rand() % 10 + 1;  // 1-10之间的执行时间
        int memory_size = rand() % 100 + 10;  // 10-110之间的内存需求
        
        create_process(pm, name, priority, burst_time, memory_size);
    }
    
    printf("自动生成了 %d 个进程\n", count);
}