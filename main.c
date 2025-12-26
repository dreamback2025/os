#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory_manager.h"
#include "process_manager.h"
#include "scheduler.h"

void print_menu() {
    printf("\n=== 多道程序设计技术演示系统 ===\n");
    printf("1. 显示内存状态\n");
    printf("2. 显示进程状态\n");
    printf("3. 自动创建进程\n");
    printf("4. 手动创建进程\n");
    printf("5. 执行时间片轮转调度\n");
    printf("6. 执行先来先服务调度\n");
    printf("7. 执行优先级调度\n");
    printf("8. 保存执行日志\n");
    printf("9. 读取并重放执行日志\n");
    printf("10. 紧凑内存\n");
    printf("0. 退出系统\n");
    printf("================================\n");
    printf("请选择操作: ");
}

int main() {
    printf("欢迎使用多道程序设计技术演示系统！\n");
    
    // 初始化内存管理器 - 创建固定分区
    int partition_sizes[] = {100, 200, 150, 300, 120};
    int partition_count = sizeof(partition_sizes) / sizeof(partition_sizes[0]);
    MemoryManager* mm = init_memory_manager(870, partition_sizes, partition_count);
    
    if (!mm) {
        printf("内存管理器初始化失败\n");
        return 1;
    }
    
    // 初始化进程管理器
    ProcessManager* pm = init_process_manager();
    if (!pm) {
        printf("进程管理器初始化失败\n");
        free_memory_manager(mm);
        return 1;
    }
    
    // 初始化调度器
    Scheduler* scheduler = init_scheduler(pm, mm);
    if (!scheduler) {
        printf("调度器初始化失败\n");
        free_process_manager(pm);
        free_memory_manager(mm);
        return 1;
    }
    
    int choice;
    do {
        print_menu();
        scanf("%d", &choice);
        getchar(); // 消费换行符
        
        switch (choice) {
            case 1:
                print_memory_status(mm);
                break;
                
            case 2:
                print_process_status(pm);
                break;
                
            case 3: {
                int count;
                printf("请输入要创建的进程数量: ");
                scanf("%d", &count);
                auto_generate_processes(pm, count);
                break;
            }
            
            case 4: {
                char name[PROCESS_NAME_LEN];
                int priority, burst_time, memory_size;
                
                printf("请输入进程名称: ");
                scanf("%s", name);
                printf("请输入优先级 (1-10): ");
                scanf("%d", &priority);
                printf("请输入执行时间: ");
                scanf("%d", &burst_time);
                printf("请输入内存需求: ");
                scanf("%d", &memory_size);
                
                PCB* pcb = create_process_manual(pm, pm->current_pid, name, priority, burst_time, memory_size);
                if (pcb) {
                    printf("成功创建进程: %s (PID: %d)\n", name, pcb->pid);
                } else {
                    printf("创建进程失败\n");
                }
                break;
            }
            
            case 5:
                if (pm->process_count > 0) {
                    schedule_processes(scheduler, ROUND_ROBIN);
                } else {
                    printf("没有可调度的进程，请先创建进程\n");
                }
                break;
                
            case 6:
                if (pm->process_count > 0) {
                    schedule_processes(scheduler, FCFS);
                } else {
                    printf("没有可调度的进程，请先创建进程\n");
                }
                break;
                
            case 7:
                if (pm->process_count > 0) {
                    schedule_processes(scheduler, PRIORITY);
                } else {
                    printf("没有可调度的进程，请先创建进程\n");
                }
                break;
                
            case 8:
                save_execution_log(scheduler, "execution_log.txt");
                break;
                
            case 9:
                replay_execution(scheduler, "execution_log.txt");
                break;
                
            case 10:
                compact_memory(mm);
                printf("内存紧凑完成\n");
                break;
                
            case 0:
                printf("退出系统，谢谢使用！\n");
                break;
                
            default:
                printf("无效选择，请重新输入\n");
                break;
        }
    } while (choice != 0);
    
    // 释放资源
    free_scheduler(scheduler);
    free_process_manager(pm);
    free_memory_manager(mm);
    
    return 0;
}