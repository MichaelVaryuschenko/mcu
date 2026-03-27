#include "stdlib.h"
#include "pico/stdlib.h"
#include "stdio.h"
#include "stdio-task/stdio-task.h"
#include "protocol-task/protocol-task.h"
#include "led-task/led-task.h"
#include "mem-task/mem-task.h"
#include "display-task/display-task.h"
#include "meas-task/meas-task.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "bme280-driver.h"

#define DEVICE_NAME "pico-device-control"
#define DEVICE_VRSN "v0.0.1"

#define ILI9341_PIN_MISO 4
#define ILI9341_PIN_CS 10
#define ILI9341_PIN_SCK 6
#define ILI9341_PIN_MOSI 7
#define ILI9341_PIN_DC 8
#define ILI9341_PIN_RESET 9

ili9341_display_t ili9341_display = {0};
ili9341_hal_t ili9341_hal = {0};

void rp2040_i2c_read(uint8_t* buffer, uint16_t length)
{
	i2c_read_timeout_us(i2c1, 0x76, buffer, length, false, 100000);
}

void rp2040_i2c_write(uint8_t* data, uint16_t size)
{
	i2c_write_timeout_us(i2c1, 0x76, data, size, false, 100000);
}

void rp2040_spi_write(const uint8_t *data, uint32_t size)
{
	spi_write_blocking(spi0, data, size);
}

void rp2040_spi_read(uint8_t *buffer, uint32_t length)
{
	spi_read_blocking(spi0, 0, buffer, length);
}

void rp2040_gpio_cs_write(bool level)
{
	gpio_put(ILI9341_PIN_CS, level);
}

void rp2040_gpio_dc_write(bool level)
{
	gpio_put(ILI9341_PIN_DC, level);
}

void rp2040_gpio_reset_write(bool level)
{
	gpio_put(ILI9341_PIN_RESET, level);
}

void rp2040_gpio_delay_ms(uint32_t ms)
{
    sleep_ms(ms);
}


void version_callback(const char* args) {
    printf("device name: '%s', firmware version: %s\n", DEVICE_NAME, DEVICE_VRSN);
}

void led_off_callback(const char* args) {
    led_task_state_set(LED_STATE_OFF);
    printf("updated LED state to 'off'\n");
}

void led_on_callback(const char* args) {
    led_task_state_set(LED_STATE_ON);
    printf("updated LED state to 'on'\n");
}

void led_blink_callback(const char* args) {
    led_task_state_set(LED_STATE_BLINK);
    printf("updated LED state to 'blink'\n");
}

void led_blink_set_period_ms_callback(const char* args) {
    uint period_ms = 0;
    sscanf(args, "%u", &period_ms);
    led_task_set_blink_period_ms(period_ms);
    printf("LED blink period is now %u\n", period_ms);
}

void help_callback(const char* args) {
    protocol_task_display();
}

void mem_callback(const char* args) {
    uint32_t addr;
    sscanf(args, "%x", &addr);
    mem_task_read((uint32_t*)addr);
}

void wmem_callback(const char* args) {
    uint32_t addr;
    uint32_t value;
    sscanf(args, "%x %x", &addr, &value);
    printf("%u %u", addr, value);
    mem_task_write((uint32_t*)addr, value);
}

void display_stat_callback(const char* args) {
    printf("width=%u height=%u", ili9341_display.width, ili9341_display.height);
}

void disp_screen_callback(const char* args)
{
	uint32_t c = 0;
	int result = sscanf(args, "%x", &c);
	
	uint16_t color = COLOR_BLACK;
	
	if (result == 1)
	{
		color = RGB888_2_RGB565(c);
	}
	
	ili9341_fill_screen(&ili9341_display, color);
}

void disp_px_callback(const char* args)
{
	uint32_t c = 0;
    uint32_t x = 0;
    uint32_t y = 0;
	int result = sscanf(args, "%u %u %x", &x, &y, &c);
	
	uint16_t color = COLOR_BLACK;
	
	if (result == 3)
	{
		color = RGB888_2_RGB565(c);
	}
	
	ili9341_draw_pixel(&ili9341_display, x, y, color);
}

void disp_line_callback(const char* args)
{
	uint32_t c = 0;
    uint32_t x1 = 0;
    uint32_t y1 = 0;
    uint32_t x2 = 0;
    uint32_t y2 = 0;
	int result = sscanf(args, "%u %u %u %u %x", &x1, &y1, &x2, &y2, &c);
	
	uint16_t color = COLOR_BLACK;
	
	if (result == 5)
	{
		color = RGB888_2_RGB565(c);
	}
	
	ili9341_draw_line(&ili9341_display, x1, y1, x2, y2, color);
}

void disp_rect_callback(const char* args)
{
	uint32_t c = 0;
    uint32_t x1 = 0;
    uint32_t y1 = 0;
    uint32_t x2 = 0;
    uint32_t y2 = 0;
	int result = sscanf(args, "%u %u %u %u %x", &x1, &y1, &x2, &y2, &c);
	
	uint16_t color = COLOR_BLACK;
	
	if (result == 5)
	{
		color = RGB888_2_RGB565(c);
	}
    if(x2 < x1 || y2 < y1) {
        printf("WARNING: invalid 2nd point coordinates: 2nd point is located above the 1st point and/or to the left of the 1st point. 2nd points coordinates have been adjusted to meet the requirements.");
    }
	
	ili9341_draw_rect(&ili9341_display, x1, y1, abs(x2 - x1), abs(y2 - y1), color);
}

void disp_frect_callback(const char* args)
{
	uint32_t c = 0;
    uint32_t x1 = 0;
    uint32_t y1 = 0;
    uint32_t x2 = 0;
    uint32_t y2 = 0;
	int result = sscanf(args, "%u %u %u %u %x", &x1, &y1, &x2, &y2, &c);
	
	uint16_t color = COLOR_BLACK;
	
	if (result == 5)
	{
		color = RGB888_2_RGB565(c);
	}
    if(x2 < x1 || y2 < y1) {
        printf("WARNING: invalid 2nd point coordinates: 2nd point is located above the 1st point and/or to the left of the 1st point. 2nd points coordinates have been adjusted to meet the requirements.");
    }
	
	ili9341_draw_filled_rect(&ili9341_display, x1, y1, abs(x2 - x1), abs(y2 - y1), color);
}

void disp_text_callback(const char* args)
{
    uint32_t c = 0;
    uint32_t c_bg = 0;
    uint32_t x = 0;
    uint32_t y = 0;
    char buffer[50];
	int result = sscanf(args, "%u %u %s %x %x", &x, &y, &buffer, &c, &c_bg);
	
	uint16_t color = COLOR_GREEN;
	uint16_t color_bg = COLOR_BLACK;
	
	if (result >= 4)
	{
		color = RGB888_2_RGB565(c);
        if(result == 5)
        {
            color_bg = RGB888_2_RGB565(c_bg);
        }
	}
	
    ili9341_draw_text(&ili9341_display, 20, 116, buffer, &jetbrains_font, color, color_bg);
}

void read_regs_callback(const char* args) {
    uint32_t addr;
    uint32_t N;
    sscanf(args, "%x %x", &addr, &N);
    if(addr <= 0xFF && N <= 0xFF && addr + N <= 0x100) {
        uint8_t buffer[256] = {0};
        bme280_read_regs(addr, buffer, N);
        printf("BME280 registers:\n-------+-------\n");
        for (int i = 0; i < N; i++)
        {
            printf("[0x%X] | 0x%X\n", addr + i, buffer[i]);
        }
        printf("-------+-------\n");
    }
    else
        printf("Error: argument value limits violation\n");
}

void write_reg_callback(const char* args) {
    uint32_t addr;
    uint32_t value;
    sscanf(args, "%x %x", &addr, &value);
    if(addr <= 0xFF && value <= 0xFF) {
        bme280_write_reg(addr, value);
        printf("Register [0x%x] is now '0x%x'\n", addr, value);
    }
    else
        printf("Error: argument value limits violation\n");
}

void temp_raw_callback(const char* args) {
    uint32_t data = bme280_read_temp_raw();
    printf("Temperature readings (c. e.): %u\n", data);
}

void temp_si_callback(const char* args) {
    double data = bme280_read_temp_si();
    printf("Temperature readings (deg. C): %f\n", data);
}

void pres_raw_callback(const char* args) {
    uint32_t data = bme280_read_pres_raw();
    printf("Pressure readings (c. e.): %u\n", data);
}

void pres_si_callback(const char* args) {
    double data = bme280_read_pres_si();
    printf("Pressure readings (hPa): %f\n", data);
}

void hum_raw_callback(const char* args) {
    uint16_t data = bme280_read_hum_raw();
    printf("Humidity readings (c. e.): %u\n", data);
}

void set_period_callback(const char* args) {
    uint32_t period_ms;
    sscanf(args, "%u", &period_ms);
    meas_task_update_period(period_ms);
}

api_t device_api[] =
{
	{"version", version_callback, "get device name and firmware version"},
    {"off", led_off_callback, "change LED state to 'off'"},
    {"on", led_on_callback, "change LED state to 'on'"},
    {"blink", led_blink_callback, "change LED state to 'blink'"},
    {"blink_set_period_ms", led_blink_set_period_ms_callback, "change LED blink period to the argument value"},
    {"help", help_callback, "display available commands alongside with their description"},
    {"mem", mem_callback, "display value stored at the argument address"},
    {"wmem", wmem_callback, "write value (2nd argument) at the address (1st argument)"}, //To put LED on you need to send 'wmem 0xd0000014 0x02000000'
	{"display_stat", display_stat_callback, "DEBUG: display ili9341 context struct values"},
    {"disp_screen", disp_screen_callback, "fill display screen with speified color (black if none specified)"},
    {"disp_px", disp_px_callback, "set the color of specified display pixel"},
    {"disp_line", disp_line_callback, "draw line of specified color"},
    {"disp_rect", disp_rect_callback, "draw rectangular frame of specified color"},
    {"disp_frect", disp_frect_callback, "draw filled rectangle of specified color"},
    {"disp_text", disp_text_callback, "display specified text"},
    {"read_regs", read_regs_callback, "read data from BME280 registers"},
    {"write_reg", write_reg_callback, "write data into specified BME280 register"},
    {"temp_raw", temp_raw_callback, "get temperature readings measured in conventional units"},
    {"temp_si", temp_si_callback, "get temperature readings measured in Celsius degrees"},
    {"pres_raw", pres_raw_callback, "get pressure readings measured in conventional units"},
    {"pres_si", pres_si_callback, "get pressure readings measured in Pascals"},
    {"hum_raw", hum_raw_callback, "get humidity readings measured in conventional units"},
    {"set_period", set_period_callback, "set BME-280 measurements period (ms)"},
    {NULL, NULL, NULL},
};

int main() {
    stdio_init_all();

    spi_init(spi0, 62500000);
    gpio_set_function(ILI9341_PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(ILI9341_PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(ILI9341_PIN_SCK, GPIO_FUNC_SPI);
    gpio_init(ILI9341_PIN_CS);
    gpio_init(ILI9341_PIN_DC);
    gpio_init(ILI9341_PIN_RESET);
    gpio_set_dir(ILI9341_PIN_CS, GPIO_OUT);
    gpio_set_dir(ILI9341_PIN_DC, GPIO_OUT);
    gpio_set_dir(ILI9341_PIN_RESET, GPIO_OUT);
    gpio_put(ILI9341_PIN_CS, 1);
    gpio_put(ILI9341_PIN_DC, 0);
    gpio_put(ILI9341_PIN_RESET, 0);
    ili9341_hal.spi_write = rp2040_spi_write;
    ili9341_hal.spi_read = rp2040_spi_read;
    ili9341_hal.gpio_cs_write = rp2040_gpio_cs_write;
    ili9341_hal.gpio_dc_write = rp2040_gpio_dc_write;
    ili9341_hal.gpio_reset_write = rp2040_gpio_reset_write;
    ili9341_hal.delay_ms = rp2040_gpio_delay_ms;
    ili9341_init(&ili9341_display, &ili9341_hal);
    ili9341_set_rotation(&ili9341_display, ILI9341_ROTATION_90);

    i2c_init(i2c1, 100000);
    gpio_set_function(14, GPIO_FUNC_I2C);
    gpio_set_function(15, GPIO_FUNC_I2C);
    bme280_init(rp2040_i2c_read, rp2040_i2c_write);

    ili9341_fill_screen(&ili9341_display, COLOR_BLACK);
    sleep_ms(300);
    // Startup picture
    /* 2. Coloured rectangles */
    ili9341_draw_filled_rect(&ili9341_display, 10, 10, 100, 60, COLOR_RED);
    ili9341_draw_filled_rect(&ili9341_display, 120, 10, 100, 60, COLOR_GREEN);
    ili9341_draw_filled_rect(&ili9341_display, 230, 10, 80, 60, COLOR_BLUE);
    /* 3. Hollow rectangle outline */
    ili9341_draw_rect(&ili9341_display, 10, 90, 300, 80, COLOR_WHITE);
    /* 4. Diagonal lines */
    ili9341_draw_line(&ili9341_display, 0, 0, 319, 239, COLOR_YELLOW);
    ili9341_draw_line(&ili9341_display, 319, 0, 0, 239, COLOR_CYAN);
    /* 5. Text */
    ili9341_draw_text(&ili9341_display, 20, 100, "Hello, ILI9341!", &jetbrains_font, COLOR_WHITE, COLOR_BLACK);
    ili9341_draw_text(&ili9341_display, 20, 116, "RP2040 / Pico SDK", &jetbrains_font, COLOR_YELLOW, COLOR_BLACK);
    sleep_ms(100);

    stdio_task_init();
    protocol_task_init(device_api);
    led_task_init();
    meas_task_init(100, &ili9341_display);

    

    while(1) {
        char* command = stdio_task_handle();
        protocol_task_handle(command);
        led_task_handle();
        meas_task_handle();
        //ili9341_fill_screen(&ili9341_display, COLOR_BLACK);
    }
}