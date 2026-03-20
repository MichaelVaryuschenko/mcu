#include "mem-task.h"

 uint32_t mem_task_read(uint32_t* memptr) {
    printf("%u\n", *memptr);
    return *memptr;
}

void mem_task_write(uint32_t* memptr, uint32_t value) {
    *memptr = value;
}