#include "scheduler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Scheduler* init_scheduler(ProcessManager* pm, MemoryManager* mm) {
    Scheduler* scheduler = (Scheduler*)malloc(sizeof(Scheduler));
    if (!scheduler) {
        printf("错误: 调度器内存分配失败\n");
        return NULL;
    }
    
    scheduler->pm = pm;
    scheduler->mm = mm;
    scheduler->current_time = 0;
    scheduler->time_quantum = TIME_QUANTUM;
    
    return scheduler;
}

void round_robin_schedule(Scheduler* scheduler) {
    printf("\n执行时间片轮转调度算法...\n");
    
    while (scheduler->pm->ready_queue || scheduler->pm->running_process) {
        // 检查是否有进程在运行
        if (scheduler->pm->running_process) {
            PCB* current_process = scheduler->pm->running_process;
            
            // 执行一个时间单位
            current_process->remaining_time--;
            current_process->waiting_time++; // 增加等待时间（对于当前运行的进程，这是周转时间的一部分）
            
            printf("时间 %d: 进程 %s (PID: %d) 正在执行, 剩余时间: %d\n", 
                   scheduler->current_time, current_process->name, 
                   current_process->pid, current_process->remaining_time);
            
            // 检查进程是否完成
            if (current_process->remaining_time <= 0) {
                printf("进程 %s (PID: %d) 执行完成\n", 
                       current_process->name, current_process->pid);
                
                // 释放内存
                if (current_process->memory_addr != -1) {
                    deallocate_partition(scheduler->mm, current_process->pid);
                    current_process->memory_addr = -1;
                }
                
                update_process_state(current_process, TERMINATED);
                current_process->turnaround_time = scheduler->current_time - current_process->arrival_time;
                
                // 从运行状态移除
                scheduler->pm->running_process = NULL;
            } else {
                // 时间片用完，将进程放回就绪队列末尾
                add_to_ready_queue(scheduler->pm, current_process);
                scheduler->pm->running_process = NULL;
            }
        }
        
        // 如果没有运行中的进程，从就绪队列取一个
        if (!scheduler->pm->running_process && scheduler->pm->ready_queue) {
            PCB* next_process = remove_from_ready_queue(scheduler->pm);
            
            // 检查进程是否已分配内存，如果没有则分配
            if (next_process->memory_addr == -1) {
                int addr = allocate_partition(scheduler->mm, next_process->pid, next_process->memory_size);
                if (addr != -1) {
                    next_process->memory_addr = addr;
                    printf("为进程 %s 分配内存地址: %d\n", next_process->name, addr);
                } else {
                    printf("为进程 %s 分配内存失败，放回就绪队列\n", next_process->name);
                    add_to_ready_queue(scheduler->pm, next_process);
                    next_process = remove_from_ready_queue(scheduler->pm);
                    if (!next_process) break; // 如果没有可运行的进程，退出
                }
            }
            
            scheduler->pm->running_process = next_process;
            update_process_state(next_process, RUNNING);
        }
        
        // 显示系统状态
        print_system_status(scheduler);
        
        // 模拟时间流逝
        scheduler->current_time++;
        
        // 暂停一下以便观察
        printf("按回车键继续...\n");
        getchar();
    }
    
    printf("所有进程执行完成！\n");
}

void fcfs_schedule(Scheduler* scheduler) {
    printf("\n执行先来先服务调度算法...\n");
    
    while (scheduler->pm->ready_queue || scheduler->pm->running_process) {
        // 如果没有运行中的进程，从就绪队列取一个
        if (!scheduler->pm->running_process && scheduler->pm->ready_queue) {
            PCB* next_process = remove_from_ready_queue(scheduler->pm);
            
            // 检查进程是否已分配内存，如果没有则分配
            if (next_process->memory_addr == -1) {
                int addr = allocate_partition(scheduler->mm, next_process->pid, next_process->memory_size);
                if (addr != -1) {
                    next_process->memory_addr = addr;
                    printf("为进程 %s 分配内存地址: %d\n", next_process->name, addr);
                } else {
                    printf("为进程 %s 分配内存失败，放回就绪队列\n", next_process->name);
                    add_to_ready_queue(scheduler->pm, next_process);
                    next_process = remove_from_ready_queue(scheduler->pm);
                    if (!next_process) break; // 如果没有可运行的进程，退出
                }
            }
            
            scheduler->pm->running_process = next_process;
            update_process_state(next_process, RUNNING);
        }
        
        // 如果有运行中的进程，执行它
        if (scheduler->pm->running_process) {
            PCB* current_process = scheduler->pm->running_process;
            
            // 执行一个时间单位
            current_process->remaining_time--;
            
            printf("时间 %d: 进程 %s (PID: %d) 正在执行, 剩余时间: %d\n", 
                   scheduler->current_time, current_process->name, 
                   current_process->pid, current_process->remaining_time);
            
            // 检查进程是否完成
            if (current_process->remaining_time <= 0) {
                printf("进程 %s (PID: %d) 执行完成\n", 
                       current_process->name, current_process->pid);
                
                // 释放内存
                if (current_process->memory_addr != -1) {
                    deallocate_partition(scheduler->mm, current_process->pid);
                    current_process->memory_addr = -1;
                }
                
                update_process_state(current_process, TERMINATED);
                current_process->turnaround_time = scheduler->current_time - current_process->arrival_time;
                
                // 从运行状态移除
                scheduler->pm->running_process = NULL;
            }
        }
        
        // 显示系统状态
        print_system_status(scheduler);
        
        // 模拟时间流逝
        scheduler->current_time++;
        
        // 暂停一下以便观察
        printf("按回车键继续...\n");
        getchar();
    }
    
    printf("所有进程执行完成！\n");
}

void priority_schedule(Scheduler* scheduler) {
    printf("\n执行优先级调度算法...\n");
    
    while (scheduler->pm->ready_queue || scheduler->pm->running_process) {
        // 如果没有运行中的进程，从就绪队列中选择优先级最高的进程
        if (!scheduler->pm->running_process && scheduler->pm->ready_queue) {
            // 找到优先级最高的进程
            PCB* highest_priority = scheduler->pm->ready_queue;
            PCB* prev = NULL;
            PCB* current = scheduler->pm->ready_queue;
            PCB* prev_of_highest = NULL;
            
            while (current) {
                if (current->priority > highest_priority->priority) {
                    highest_priority = current;
                    prev_of_highest = prev;
                }
                prev = current;
                current = current->next;
            }
            
            // 从就绪队列中移除最高优先级的进程
            if (prev_of_highest) {
                prev_of_highest->next = highest_priority->next;
            } else {
                scheduler->pm->ready_queue = highest_priority->next;
            }
            highest_priority->next = NULL;
            
            // 检查进程是否已分配内存，如果没有则分配
            if (highest_priority->memory_addr == -1) {
                int addr = allocate_partition(scheduler->mm, highest_priority->pid, highest_priority->memory_size);
                if (addr != -1) {
                    highest_priority->memory_addr = addr;
                    printf("为进程 %s 分配内存地址: %d\n", highest_priority->name, addr);
                } else {
                    printf("为进程 %s 分配内存失败，放回就绪队列\n", highest_priority->name);
                    add_to_ready_queue(scheduler->pm, highest_priority);
                    // 重新查找最高优先级进程
                    continue;
                }
            }
            
            scheduler->pm->running_process = highest_priority;
            update_process_state(highest_priority, RUNNING);
        }
        
        // 如果有运行中的进程，执行它
        if (scheduler->pm->running_process) {
            PCB* current_process = scheduler->pm->running_process;
            
            // 执行一个时间单位
            current_process->remaining_time--;
            
            printf("时间 %d: 进程 %s (PID: %d, 优先级: %d) 正在执行, 剩余时间: %d\n", 
                   scheduler->current_time, current_process->name, 
                   current_process->pid, current_process->priority, current_process->remaining_time);
            
            // 检查进程是否完成
            if (current_process->remaining_time <= 0) {
                printf("进程 %s (PID: %d) 执行完成\n", 
                       current_process->name, current_process->pid);
                
                // 释放内存
                if (current_process->memory_addr != -1) {
                    deallocate_partition(scheduler->mm, current_process->pid);
                    current_process->memory_addr = -1;
                }
                
                update_process_state(current_process, TERMINATED);
                current_process->turnaround_time = scheduler->current_time - current_process->arrival_time;
                
                // 从运行状态移除
                scheduler->pm->running_process = NULL;
            }
        }
        
        // 显示系统状态
        print_system_status(scheduler);
        
        // 模拟时间流逝
        scheduler->current_time++;
        
        // 暂停一下以便观察
        printf("按回车键继续...\n");
        getchar();
    }
    
    printf("所有进程执行完成！\n");
}

void schedule_processes(Scheduler* scheduler, SchedulingAlgorithm algo) {
    switch (algo) {
        case ROUND_ROBIN:
            round_robin_schedule(scheduler);
            break;
        case FCFS:
            fcfs_schedule(scheduler);
            break;
        case PRIORITY:
            priority_schedule(scheduler);
            break;
        default:
            printf("未知的调度算法\n");
            break;
    }
}

void execute_process(Scheduler* scheduler, PCB* pcb) {
    if (!pcb) return;
    
    // 检查进程是否已分配内存，如果没有则分配
    if (pcb->memory_addr == -1) {
        int addr = allocate_partition(scheduler->mm, pcb->pid, pcb->memory_size);
        if (addr != -1) {
            pcb->memory_addr = addr;
            printf("为进程 %s 分配内存地址: %d\n", pcb->name, addr);
        } else {
            printf("为进程 %s 分配内存失败\n", pcb->name);
            return;
        }
    }
    
    // 执行进程
    pcb->remaining_time--;
    printf("执行进程 %s (PID: %d), 剩余时间: %d\n", pcb->name, pcb->pid, pcb->remaining_time);
    
    // 检查是否完成
    if (pcb->remaining_time <= 0) {
        printf("进程 %s (PID: %d) 执行完成\n", pcb->name, pcb->pid);
        deallocate_partition(scheduler->mm, pcb->pid);
        pcb->memory_addr = -1;
        update_process_state(pcb, TERMINATED);
        pcb->turnaround_time = scheduler->current_time - pcb->arrival_time;
    }
}

void simulate_time_passage(Scheduler* scheduler) {
    scheduler->current_time++;
}

void print_system_status(Scheduler* scheduler) {
    printf("\n=== 系统状态 (时间: %d) ===\n", scheduler->current_time);
    
    // 打印内存状态
    print_memory_status(scheduler->mm);
    
    // 打印进程状态
    print_process_status(scheduler->pm);
    
    if (scheduler->pm->running_process) {
        printf("当前运行进程: %s (PID: %d)\n", 
               scheduler->pm->running_process->name, 
               scheduler->pm->running_process->pid);
    } else {
        printf("当前无运行进程\n");
    }
    
    printf("========================\n");
}

void save_execution_log(Scheduler* scheduler, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("错误: 无法创建日志文件 %s\n", filename);
        return;
    }
    
    fprintf(file, "系统执行日志\n");
    fprintf(file, "时间: %d\n", scheduler->current_time);
    fprintf(file, "进程数量: %d\n", scheduler->pm->process_count);
    
    for (int i = 0; i < scheduler->pm->process_count; i++) {
        PCB* pcb = &scheduler->pm->processes[i];
        if (pcb->pid != -1) {
            fprintf(file, "进程ID: %d, 名称: %s, 状态: %d, 优先级: %d, 到达时间: %d, 执行时间: %d, 剩余时间: %d, 内存大小: %d, 内存地址: %d\n",
                    pcb->pid, pcb->name, pcb->state, pcb->priority, pcb->arrival_time,
                    pcb->burst_time, pcb->remaining_time, pcb->memory_size, pcb->memory_addr);
        }
    }
    
    fclose(file);
    printf("执行日志已保存到 %s\n", filename);
}

void load_execution_log(Scheduler* scheduler, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("错误: 无法打开日志文件 %s\n", filename);
        return;
    }
    
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file)) {
        printf("%s", buffer);
    }
    
    fclose(file);
    printf("执行日志已从 %s 读取\n", filename);
    
    // 使用参数以避免警告
    (void)scheduler;
}

void replay_execution(Scheduler* scheduler, const char* filename) {
    printf("重放执行过程从文件 %s\n", filename);
    load_execution_log(scheduler, filename);
}

void free_scheduler(Scheduler* scheduler) {
    if (scheduler) {
        free(scheduler);
    }
}