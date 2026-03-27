#include "meas-task.h"

uint32_t meas_period_us;
uint64_t meas_ts = 0;
static ili9341_display_t* ili9341_display;
double temp_log[4] = {25.25, 25.25, 25.25, 25.25};
double pres_log[4] = {0, 0, 0, 0};

void meas_task_init(uint32_t period_ms, ili9341_display_t* ili9341_display_ctx) {
    meas_period_us = period_ms * 1000;
    ili9341_display = ili9341_display_ctx;
    ili9341_fill_screen(ili9341_display, COLOR_BLACK);
    ili9341_draw_filled_rect(ili9341_display, 10, 10, 3, 12, COLOR_WHITE);
    ili9341_draw_text(ili9341_display, 15, 10, "BME-280 readings", &jetbrains_font, COLOR_WHITE, COLOR_BLACK);
    ili9341_draw_filled_rect(ili9341_display, 10, 10+16*7, 3, 12, COLOR_WHITE);
    ili9341_draw_text(ili9341_display, 15, 10+16*7, "Legend:", &jetbrains_font, COLOR_WHITE, COLOR_BLACK);
    ili9341_draw_filled_rect(ili9341_display, 10, 10+16*8, 3, 12, COLOR_WHITE);
    ili9341_draw_text(ili9341_display, 15, 10+16*8, " -- system", &jetbrains_font, COLOR_WHITE, COLOR_BLACK);
    ili9341_draw_filled_rect(ili9341_display, 10, 10+16*9, 3, 12, COLOR_RED);
    ili9341_draw_text(ili9341_display, 15, 10+16*9, " -- temperature", &jetbrains_font, COLOR_WHITE, COLOR_BLACK);
    ili9341_draw_filled_rect(ili9341_display, 10, 10+16*10, 3, 12, COLOR_BLUE);
    ili9341_draw_text(ili9341_display, 15, 10+16*10, " -- pressure", &jetbrains_font, COLOR_WHITE, COLOR_BLACK);

    ili9341_draw_line(ili9341_display, 200, 10+16, 200, 10+16+60, COLOR_WHITE);
    ili9341_draw_line(ili9341_display, 200, 10+16+60, 200+60, 10+16+60, COLOR_WHITE);
    ili9341_draw_text(ili9341_display, 200, 10, "deg. C", &jetbrains_font, COLOR_WHITE, COLOR_BLACK);
    ili9341_draw_text(ili9341_display, 200+60+10, 10+16+60-16, "sample", &jetbrains_font, COLOR_WHITE, COLOR_BLACK);
    for(int i = 0; i < 4; i++) {
        ili9341_draw_line(ili9341_display, 200, 10+16+60-i*20, 195, 10+16+60-i*20, COLOR_WHITE);
        ili9341_draw_line(ili9341_display, 200+i*20, 10+16+60, 200+i*20, 10+16+60+5, COLOR_WHITE);
    }
    ili9341_draw_text(ili9341_display, 170, 10+8, "40", &jetbrains_font, COLOR_WHITE, COLOR_BLACK);
    ili9341_draw_text(ili9341_display, 170, 10+8+20, "35", &jetbrains_font, COLOR_WHITE, COLOR_BLACK);
    ili9341_draw_text(ili9341_display, 170, 10+8+20*2, "30", &jetbrains_font, COLOR_WHITE, COLOR_BLACK);
    ili9341_draw_text(ili9341_display, 170, 10+8+20*3, "25", &jetbrains_font, COLOR_WHITE, COLOR_BLACK);
    ili9341_draw_text(ili9341_display, 200, 10+16+60+6, "-3", &jetbrains_font, COLOR_WHITE, COLOR_BLACK);
    ili9341_draw_text(ili9341_display, 200+20, 10+16+60+6, "-2", &jetbrains_font, COLOR_WHITE, COLOR_BLACK);
    ili9341_draw_text(ili9341_display, 200+20*2, 10+16+60+6, "-1", &jetbrains_font, COLOR_WHITE, COLOR_BLACK);
    ili9341_draw_text(ili9341_display, 200+20*3, 10+16+60+6, "curr", &jetbrains_font, COLOR_WHITE, COLOR_BLACK);
}

void meas_task_update_period(uint32_t period_ms) {
    meas_period_us = period_ms * 1000;
}

void meas_task_handle() {
    if(time_us_64() > meas_ts) {
        meas_ts = time_us_64() + meas_period_us;
        uint32_t data_raw[2] = {bme280_read_temp_raw(), bme280_read_pres_raw()};
        double data_si[2] = {bme280_read_temp_si(), bme280_read_pres_si()};

        char output[32];
        snprintf(output, sizeof(output), "%u c. u.", data_raw[0]);
        ili9341_draw_filled_rect(ili9341_display, 10, 10+16, 3, 12, COLOR_RED);
        ili9341_draw_text(ili9341_display, 15, 10+16, output, &jetbrains_font, COLOR_WHITE, COLOR_BLACK);
        snprintf(output, sizeof(output), "%u c. u.", data_raw[1]);
        ili9341_draw_filled_rect(ili9341_display, 10, 10+16*2, 3, 12, COLOR_BLUE);
        ili9341_draw_text(ili9341_display, 15, 10+16*2, output, &jetbrains_font, COLOR_WHITE, COLOR_BLACK);
        snprintf(output, sizeof(output), "%f deg. C", data_si[0]);
        ili9341_draw_filled_rect(ili9341_display, 10, 10+16*3, 3, 12, COLOR_RED);
        ili9341_draw_text(ili9341_display, 15, 10+16*3, output, &jetbrains_font, COLOR_WHITE, COLOR_BLACK);
        snprintf(output, sizeof(output), "%f hPa", data_si[1]);
        ili9341_draw_filled_rect(ili9341_display, 10, 10+16*4, 3, 12, COLOR_BLUE);
        ili9341_draw_text(ili9341_display, 15, 10+16*4, output, &jetbrains_font, COLOR_WHITE, COLOR_BLACK);

        for(int i = 0; i < 3; i++) {
            temp_log[i] = temp_log[i + 1];
            pres_log[i] = pres_log[i + 1];
        }
        temp_log[3] = data_si[0];
        pres_log[3] = data_si[1];
        ili9341_draw_filled_rect(ili9341_display, 201, 10+16, 61, 60,   COLOR_BLACK);
        uint32_t x[4];
        uint32_t y[4];
        for(int i = 0; i < 4; i++) {
            x[i] = 201 + i * 20;
            y[i] = 26 + 60 - ((uint32_t)(temp_log[i] * 4) - 25 * 4);
            //snprintf(output, sizeof(output), "%u %u", x[i], y[i]);
            //ili9341_draw_text(ili9341_display, 200, 150+16*i, output, &jetbrains_font, COLOR_WHITE, COLOR_BLACK);
        }
        for(int i = 0; i < 3; i++) {
            ili9341_draw_line(ili9341_display, x[i], y[i], x[i + 1], y[i + 1], COLOR_RED);
        }
    }
}