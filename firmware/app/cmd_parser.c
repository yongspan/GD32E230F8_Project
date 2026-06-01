#include "cmd_parser.h"
#include "bsp_led.h"
#include "app_brightness.h"
#include "app_battery.h"
#include "bsp_charge.h"
#include "app_version.h"
#include "app_config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CMD_BUFFER_SIZE 32

static char cmd_buffer[CMD_BUFFER_SIZE];
static uint8_t cmd_index = 0;
static uint8_t led_blink_enable = 0;

static void cmd_print_status(void)
{
    printf("System Status:\r\n");
    printf("Version : %s\r\n", APP_VERSION_STRING);
    printf("UART    : OK\r\n");
    printf("Flash   : OK\r\n");

    printf("Backlight:\r\n");
    printf("  Level : %d/%d\r\n",
           app_brightness_get_level(),
           app_brightness_get_max_level());
    printf("  Duty  : %d%%\r\n", app_brightness_get_duty());

    printf("Battery:\r\n");
    printf("  Voltage : %dmV\r\n", app_battery_get_voltage_mv());
    printf("  Percent : %d%%\r\n", app_battery_get_percent());
    printf("  LED     : %d/%d\r\n",
           app_battery_get_led_count(),
           app_battery_get_led_max_count());
    printf("  Display : %s\r\n", app_battery_led_is_visible() ? "ON" : "OFF");
    printf("  Low     : %s\r\n", app_battery_is_low() ? "YES" : "NO");
    printf("  Cutoff  : %s\r\n", app_battery_is_cutoff() ? "YES" : "NO");
    printf("  Switch  : %s\r\n", app_battery_is_switch_on() ? "ON" : "OFF");

    printf("Charge:\r\n");
    printf("  Status  : %s\r\n", bsp_charge_get_status_string());
    printf("  USB_DET : %s\r\n", bsp_charge_get_usb_string());
}

static void cmd_print_help(void)
{
    printf("Commands:\r\n");
    printf("  help;\r\n");
    printf("  status;\r\n");
    printf("  version;\r\n");
    printf("  level;\r\n");
    printf("  level 0~12;\r\n");
    printf("  pwm 0~100;\r\n");
    printf("  bat;\r\n");
    printf("  charge;\r\n");
    printf("  switch;\r\n");
    printf("  display on;\r\n");
    printf("  display off;\r\n");
    printf("  factory;\r\n");
    printf("  led1 on;\r\n");
    printf("  led1 off;\r\n");
    printf("  led2 on;\r\n");
    printf("  led2 off;\r\n");
    printf("  blink1;\r\n");
    printf("  all off;\r\n");
}

void cmd_parser_process_char(char ch)
{
    if((ch == '\r') || (ch == '\n'))
    {
        return;
    }

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
            cmd_print_status();
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
        else if(strcmp(cmd_buffer, "bat") == 0)
        {
            app_battery_led_show_for(APP_BATTERY_LED_SHOW_CMD_MS);
            printf("Battery: %dmV %d%% LED=%d/%d display=%s low=%s cutoff=%s\r\n",
                   app_battery_get_voltage_mv(),
                   app_battery_get_percent(),
                   app_battery_get_led_count(),
                   app_battery_get_led_max_count(),
                   app_battery_led_is_visible() ? "ON" : "OFF",
                   app_battery_is_low() ? "YES" : "NO",
                   app_battery_is_cutoff() ? "YES" : "NO");
        }
        else if(strcmp(cmd_buffer, "charge") == 0)
        {
            app_battery_led_show_for(APP_BATTERY_LED_SHOW_CMD_MS);
            printf("Charge: %s USB_DET=%s\r\n",
                   bsp_charge_get_status_string(),
                   bsp_charge_get_usb_string());
        }
        else if(strcmp(cmd_buffer, "switch") == 0)
        {
            printf("Switch: %s\r\n", app_battery_is_switch_on() ? "ON" : "OFF");
        }
        else if(strcmp(cmd_buffer, "display on") == 0)
        {
            app_battery_led_show_for(APP_BATTERY_LED_SHOW_CMD_MS);
            printf("Battery display ON for %d ms\r\n", APP_BATTERY_LED_SHOW_CMD_MS);
        }
        else if(strcmp(cmd_buffer, "display off") == 0)
        {
            app_battery_led_hide();
            printf("Battery display OFF\r\n");
        }
        else if(strcmp(cmd_buffer, "factory") == 0)
        {
            app_brightness_factory_reset();
        }
        else if(strcmp(cmd_buffer, "help") == 0)
        {
            cmd_print_help();
        }
        else
        {
            printf("Unknown CMD:%s\r\n", cmd_buffer);
        }

        cmd_index = 0;
    }
    else
    {
        if(cmd_index < (CMD_BUFFER_SIZE - 1U))
        {
            cmd_buffer[cmd_index++] = ch;
        }
        else
        {
            cmd_index = 0;
            printf("CMD too long, cleared\r\n");
        }
    }
}

uint8_t cmd_parser_get_blink_enable(void)
{
    return led_blink_enable;
}
