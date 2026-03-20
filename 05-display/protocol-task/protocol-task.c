#include "protocol-task.h"
#include "stdio.h"
#include "string.h"

static api_t* api = {0};
static int commands_count = 0;

void protocol_task_init(api_t* device_api) {
    api = device_api;
    int i = 0;
    api_t* temp = device_api;
    while((*temp).command_name != NULL) {
        i++;
        temp += 1;
    }
    commands_count = i;
}

void protocol_task_handle(char* command_string) {
    if(command_string) {
        const char* command_name = command_string;
        const char* command_args = NULL;

        char* space_symbol = strchr(command_string, ' ');
        if (space_symbol)
        {
            *space_symbol = '\0';
            command_args = space_symbol + 1;
        }
        else
        {
            command_args = "";
        }
        printf("recieved command: %s(%s)\n", command_name, command_args);
        uint32_t command_found = 0;
        for(int i = 0; i < commands_count; i++) {
            if(!strcmp(command_name, api[i].command_name)) {
                api[i].command_callback(command_args);
                command_found = 1;
                break;
            }
        }
        if(!command_found) {
            printf("Error: command not found\n");
        }
    }
}

void protocol_task_display() { // Display commands in API
    int i = 0;
    while(api[i].command_name != NULL) {
        printf("'%s' command: %s\n", api[i].command_name, api[i].command_help);
        i++;
    }
}