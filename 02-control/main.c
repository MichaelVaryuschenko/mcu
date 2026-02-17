#include "stdlib.h"
#include "pico/stdlib.h"
#include "stdio.h"
#include "stdio-task/stdio-task.h"
#include "protocol-task/protocol-task.h"
#include "led-task/led-task.h"
#include "mem-task/mem-task.h"

#define DEVICE_NAME "pico-device-control"
#define DEVICE_VRSN "v0.0.1"

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
	{NULL, NULL, NULL},
};

int main() {
    stdio_init_all();
    stdio_task_init();
    protocol_task_init(device_api);
    led_task_init();
    

    while(1) {
        char* command = stdio_task_handle();
        protocol_task_handle(command);
        led_task_handle();
    }
}