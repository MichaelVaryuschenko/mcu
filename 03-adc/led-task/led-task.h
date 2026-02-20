#pragma once

#include "pico/stdlib.h"

typedef enum {
    LED_STATE_OFF,
    LED_STATE_ON,
    LED_STATE_BLINK,
} led_state_t;

void led_task_init();
void led_task_handle();
void led_task_state_set(led_state_t state);
void led_task_set_blink_period_ms(uint32_t period_ms);