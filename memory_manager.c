#include "memory_manager.h"
#include <string.h>

MemoryManager* init_memory_manager(int total_memory, int* partition_sizes, int count) {
    if (count > MAX_PARTITIONS) {
        printf("Error: Number of partitions exceeds maximum limit\n");
        return NULL;
    }
    
    MemoryManager* mm = (MemoryManager*)malloc(sizeof(MemoryManager));
    if (!mm) {
        printf("Error: Memory allocation failed\n");
        return NULL;
    }
    
    mm->partition_count = count;
    mm->total_memory = total_memory;
    mm->free_memory = total_memory;
    
    // 初始化分区
    int current_addr = 0;
    for (int i = 0; i < count; i++) {
        mm->partitions[i].id = i;
        mm->partitions[i].start_addr = current_addr;
        mm->partitions[i].size = partition_sizes[i];
        mm->partitions[i].allocated_size = 0;
        mm->partitions[i].process_id = -1;
        mm->partitions[i].is_allocated = false;
        current_addr += partition_sizes[i];
    }
    
    // 检查总内存是否超过限制
    if (current_addr > MAX_MEMORY_SIZE) {
        printf("Error: Total memory size exceeds limit\n");
        free(mm);
        return NULL;
    }
    
    return mm;
}

int allocate_partition(MemoryManager* mm, int process_id, int size) {
    // 使用首次适应算法
    return allocate_first_fit(mm, process_id, size);
}

int allocate_first_fit(MemoryManager* mm, int process_id, int size) {
    for (int i = 0; i < mm->partition_count; i++) {
        if (!mm->partitions[i].is_allocated && mm->partitions[i].size >= size) {
            mm->partitions[i].is_allocated = true;
            mm->partitions[i].process_id = process_id;
            mm->partitions[i].allocated_size = size;
            mm->free_memory -= size;
            return mm->partitions[i].start_addr;
        }
    }
    return -1; // 分配失败
}

int allocate_best_fit(MemoryManager* mm, int process_id, int size) {
    int best_idx = -1;
    int min_size_diff = MAX_MEMORY_SIZE + 1;
    
    for (int i = 0; i < mm->partition_count; i++) {
        if (!mm->partitions[i].is_allocated && mm->partitions[i].size >= size) {
            int size_diff = mm->partitions[i].size - size;
            if (size_diff < min_size_diff) {
                min_size_diff = size_diff;
                best_idx = i;
            }
        }
    }
    
    if (best_idx != -1) {
        mm->partitions[best_idx].is_allocated = true;
        mm->partitions[best_idx].process_id = process_id;
        mm->partitions[best_idx].allocated_size = size;
        mm->free_memory -= size;
        return mm->partitions[best_idx].start_addr;
    }
    
    return -1; // 分配失败
}

void deallocate_partition(MemoryManager* mm, int process_id) {
    for (int i = 0; i < mm->partition_count; i++) {
        if (mm->partitions[i].is_allocated && mm->partitions[i].process_id == process_id) {
            mm->free_memory += mm->partitions[i].allocated_size;
            mm->partitions[i].is_allocated = false;
            mm->partitions[i].process_id = -1;
            mm->partitions[i].allocated_size = 0;
            break;
        }
    }
}

void compact_memory(MemoryManager* mm) {
    // 简单的紧凑算法实现
    // 将所有已分配的分区移到内存的开始部分
    int current_addr = 0;
    for (int i = 0; i < mm->partition_count; i++) {
        if (mm->partitions[i].is_allocated) {
            mm->partitions[i].start_addr = current_addr;
            current_addr += mm->partitions[i].size;
        }
    }
    // 更新剩余的未分配空间
    mm->free_memory = mm->total_memory - current_addr;
}

void print_memory_status(MemoryManager* mm) {
    printf("\n=== 内存分配状态 ===\n");
    printf("分区ID\t起始地址\t大小\t已分配大小\t进程ID\t状态\n");
    for (int i = 0; i < mm->partition_count; i++) {
        printf("%d\t%d\t\t%d\t%d\t\t", 
               mm->partitions[i].id,
               mm->partitions[i].start_addr,
               mm->partitions[i].size,
               mm->partitions[i].allocated_size);
        
        if (mm->partitions[i].is_allocated) {
            printf("%d\t", mm->partitions[i].process_id);
            printf("已分配");
        } else {
            printf("-1\t");
            printf("空闲");
        }
        printf("\n");
    }
    printf("总内存: %d, 已用: %d, 空闲: %d\n", 
           mm->total_memory, 
           mm->total_memory - mm->free_memory, 
           mm->free_memory);
    printf("==================\n");
}

void free_memory_manager(MemoryManager* mm) {
    if (mm) {
        free(mm);
    }
}