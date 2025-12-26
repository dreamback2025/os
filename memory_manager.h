#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_PARTITIONS 10
#define MAX_MEMORY_SIZE 1024

// Partition structure
typedef struct {
    int id;
    int start_addr;
    int size;
    int allocated_size;  // Size actually allocated
    int process_id;      // Process ID assigned to
    bool is_allocated;
} Partition;

// Memory manager structure
typedef struct {
    Partition partitions[MAX_PARTITIONS];
    int partition_count;
    int total_memory;
    int free_memory;
} MemoryManager;

// Function declarations
MemoryManager* init_memory_manager(int total_memory, int* partition_sizes, int count);
int allocate_partition(MemoryManager* mm, int process_id, int size);
int allocate_best_fit(MemoryManager* mm, int process_id, int size);
int allocate_first_fit(MemoryManager* mm, int process_id, int size);
void deallocate_partition(MemoryManager* mm, int process_id);
void compact_memory(MemoryManager* mm);
void print_memory_status(MemoryManager* mm);
void free_memory_manager(MemoryManager* mm);

#endif