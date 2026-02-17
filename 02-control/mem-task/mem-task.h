#pragma once

#include "stdlib.h"
#include "pico/stdlib.h"
#include "stdio.h"

uint32_t mem_task_read(uint32_t* memptr);
void mem_task_write(uint32_t* memptr, uint32_t value);