#include "cmd_parser.h"
#include "bsp_led.h"
#include <stdio.h>
#include <string.h>

#define CMD_BUFFER_SIZE 32

static char cmd_buffer[CMD_BUFFER_SIZE];
static uint8_t cmd_index = 0;

void cmd_parser_process_char(char ch)
{
	  
    if(ch == ';')
    {
        cmd_buffer[cmd_index] = '\0';

        printf("CMD:%s\r\n", cmd_buffer);

        if(strcmp(cmd_buffer, "led on") == 0)
{
    bsp_led1_on();
    printf("LED ON\r\n");
}
else if(strcmp(cmd_buffer, "led off") == 0)
{
    bsp_led1_off();
    printf("LED OFF\r\n");
}

        cmd_index = 0;
    }
    else
    {
        if(cmd_index < CMD_BUFFER_SIZE - 1)
        {
            cmd_buffer[cmd_index++] = ch;
        }
    }
}