#include "cmd_parser.h"
#include "bsp_led.h"
#include "bsp_pwm.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "app_brightness.h"
#include "bsp_adc.h"
#include "app_battery.h"
#include "bsp_charge.h"
#include "app_version.h"

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

          app_brightness_set_duty((uint8_t)duty);
        }
        else if(strcmp(cmd_buffer, "status") == 0)
        {
    printf("System Status:\r\n");
printf("Version : %s\r\n", APP_VERSION_STRING);
printf("UART    : OK\r\n");
printf("Flash   : OK\r\n");

printf("Backlight:\r\n");
printf("  Level : %d/%d\r\n",
       app_brightness_get_level(),
       app_brightness_get_max_level());
printf("  Duty  : %d%%\r\n",
       app_brightness_get_duty());

printf("Battery:\r\n");
printf("  Voltage : %dmV\r\n", app_battery_get_voltage_mv());
printf("  Percent : %d%%\r\n", app_battery_get_percent());
printf("  LED     : %d/3\r\n", app_battery_get_led_count());

printf("Charge:\r\n");
printf("  Status  : %s\r\n", bsp_charge_get_status_string());		
					
        }
				else if(strcmp(cmd_buffer, "version") == 0)
{
   printf("GD32 Backlight Controller %s\r\n", APP_VERSION_STRING);
}
else if(strcmp(cmd_buffer, "level") == 0)
{
    printf("Level: %d/%d duty=%d%%\r\n",
           app_brightness_get_level(),
           app_brightness_get_max_level(),
           app_brightness_get_duty());
}
else if(strncmp(cmd_buffer, "level ", 6) == 0)
{
    int level = atoi(&cmd_buffer[6]);

    if(level < 0)
    {
        level = 0;
    }

    if(level > app_brightness_get_max_level())
    {
        level = app_brightness_get_max_level();
    }

    app_brightness_set_level((uint8_t)level);
}
else if(strcmp(cmd_buffer, "factory") == 0)
{
    app_brightness_factory_reset();
}
        else if(strcmp(cmd_buffer, "help") == 0)
        {
            printf("Commands:\r\n");
printf("  help;\r\n");
printf("  status;\r\n");
printf("  version;\r\n");
printf("  level;\r\n");
printf("  level 0~12;\r\n");
printf("  pwm 0~100;\r\n");
printf("  factory;\r\n");
printf("  led1 on;\r\n");
printf("  led1 off;\r\n");
printf("  led2 on;\r\n");
printf("  led2 off;\r\n");
printf("  blink1;\r\n");
printf("  all off;\r\n");
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