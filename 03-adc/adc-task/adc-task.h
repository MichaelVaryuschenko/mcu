#pragma once

#include "hardware/adc.h"
#include "pico/stdlib.h"
#include "stdio.h"

typedef enum {
    ADC_TASK_STATE_IDLE,
    ADC_TASK_STATE_RUN
} adc_task_state_t;

void adc_task_init();
void adc_task_set_state(adc_task_state_t new_adc_state);
void adc_task_handle();
float adc_task_measure();
float adc_task_measure_temp();