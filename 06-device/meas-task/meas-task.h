#pragma once

#include "stdlib.h"
#include "pico/stdlib.h"
#include "bme280-driver.h"
#include "ili9341-driver.h"
#include "ili9341-display.h"

void meas_task_init(uint32_t period_ms, ili9341_display_t* ili9341_display_ctx);
void meas_task_update_period(uint32_t period_ms);
void meas_task_handle();