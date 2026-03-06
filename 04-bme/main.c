#include "stdlib.h"
#include "pico/stdlib.h"
#include "stdio.h"
#include "stdio-task/stdio-task.h"
#include "protocol-task.h"
#include "led-task/led-task.h"
#include "mem-task/mem-task.h"
#include "hardware/i2c.h"
#include "bme280-driver.h"

#define DEVICE_NAME "pico-device-control"
#define DEVICE_VRSN "v0.0.1"

void rp2040_i2c_read(uint8_t* buffer, uint16_t length)
{
	i2c_read_timeout_us(i2c1, 0x76, buffer, length, false, 100000);
}

void rp2040_i2c_write(uint8_t* data, uint16_t size)
{
	i2c_write_timeout_us(i2c1, 0x76, data, size, false, 100000);
}

// API command callbacks

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
	{"read_regs", read_regs_callback, "read data from BME280 registers"},
    {"write_reg", write_reg_callback, "write data into specified BME280 register"},
    {"temp_raw", temp_raw_callback, "get temperature readings measured in conventional units"},
    {"temp_si", temp_si_callback, "get temperature readings measured in Celsius degrees"},
    {"pres_raw", pres_raw_callback, "get pressure readings measured in conventional units"},
    {"pres_si", pres_si_callback, "get pressure readings measured in Pascals"},
    {"hum_raw", hum_raw_callback, "get humidity readings measured in conventional units"},
    {NULL, NULL, NULL},
};

int main() {
    stdio_init_all();
    stdio_task_init();
    protocol_task_init(device_api);
    led_task_init();
    i2c_init(i2c1, 100000);
    gpio_set_function(14, GPIO_FUNC_I2C);
    gpio_set_function(15, GPIO_FUNC_I2C);
    bme280_init(rp2040_i2c_read, rp2040_i2c_write);
    

    while(1) {
        char* command = stdio_task_handle();
        protocol_task_handle(command);
        led_task_handle();
    }
}