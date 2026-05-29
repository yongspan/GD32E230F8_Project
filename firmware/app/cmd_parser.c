#include "cmd_parser.h"
#include "bsp_led.h"
#include "bsp_pwm.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CMD_BUFFER_SIZE 32

static char cmd_buffer[CMD_BUFFER_SIZE];
static uint8_t cmd_index = 0;
static uint8_t led_blink_enable = 0;

void cmd_parser_process_char(char ch)
{
    if(ch == ';')
    {
        cmd_buffer[cmd_index] = '\0';

        printf("CMD:%s\r\n", cmd_buffer);

        if(strcmp(cmd_buffer, "led1 on") == 0)
        {
            led_blink_enable = 0;
            bsp_led1_on();
            printf("LED1 ON\r\n");
        }
        else if(strcmp(cmd_buffer, "led1 off") == 0)
        {
            led_blink_enable = 0;
            bsp_led1_off();
            printf("LED1 OFF\r\n");
        }
        else if(strcmp(cmd_buffer, "led2 on") == 0)
        {
            led_blink_enable = 0;
            bsp_led2_on();
            printf("LED2 ON\r\n");
        }
        else if(strcmp(cmd_buffer, "led2 off") == 0)
        {
            led_blink_enable = 0;
            bsp_led2_off();
            printf("LED2 OFF\r\n");
        }
        else if(strcmp(cmd_buffer, "blink1") == 0)
        {
            led_blink_enable = 1;
            printf("LED1 BLINK ENABLE\r\n");
        }
        else if(strcmp(cmd_buffer, "all off") == 0)
        {
            led_blink_enable = 0;
            bsp_led1_off();
            bsp_led2_off();
            printf("ALL LED OFF\r\n");
        }
        else if(strncmp(cmd_buffer, "pwm ", 4) == 0)
        {
            int duty = atoi(&cmd_buffer[4]);

            if(duty < 0)
            {
                duty = 0;
            }

            if(duty > 100)
            {
                duty = 100;
            }

            bsp_pwm_set_duty((uint8_t)duty);
            printf("PWM duty:%d%%\r\n", duty);
        }
        else if(strcmp(cmd_buffer, "status") == 0)
        {
            printf("System Status:\r\n");
            printf("Clock: IRC8M / SystemCoreClock\r\n");
            printf("UART: OK\r\n");
            printf("Command End: ;\r\n");
            printf("Blink1: %s\r\n", led_blink_enable ? "ON" : "OFF");
            printf("PWM Duty: %d%%\r\n", bsp_pwm_get_duty());
        }
        else if(strcmp(cmd_buffer, "help") == 0)
        {
            printf("Commands:\r\n");
            printf("led1 on;\r\n");
            printf("led1 off;\r\n");
            printf("led2 on;\r\n");
            printf("led2 off;\r\n");
            printf("blink1;\r\n");
            printf("all off;\r\n");
            printf("pwm 0;\r\n");
            printf("pwm 1;\r\n");
            printf("pwm 10;\r\n");
            printf("pwm 50;\r\n");
            printf("pwm 100;\r\n");
            printf("status;\r\n");
            printf("help;\r\n");
        }
        else
        {
            printf("Unknown CMD:%s\r\n", cmd_buffer);
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

uint8_t cmd_parser_get_blink_enable(void)
{
    return led_blink_enable;
}