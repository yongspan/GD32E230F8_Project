#include "gd32e23x.h"
#include "bsp_led.h"
#include "delay.h"
#include "bsp_uart.h"
#include "bsp_uart_ringbuffer.h"
#include "cmd_parser.h"
#include "bsp_key.h"
#include "bsp_pwm.h"
#include "app_brightness.h"
#include <stdio.h>
#include "bsp_adc.h"
#include "app_battery.h"
#include "bsp_charge.h"
#include "app_version.h"

int main(void)
{
	uint8_t ch;
delay_init();

bsp_led_init();
bsp_uart_init();
bsp_charge_init();	
app_battery_init();
	
printf("\r\n");
printf("========== BOOT ==========\r\n");

printf("Init key...\r\n");
bsp_key_exti_init();

printf("Init pwm...\r\n");
bsp_pwm_init();

printf("Init brightness...\r\n");
app_brightness_init();

printf("Init adc...\r\n");
bsp_adc_init();
printf("Init adc done.\r\n");

printf("GD32 Backlight Controller %s\r\n", APP_VERSION_STRING);
printf("UART RX Ready\r\n");

    while(1)
    {
        while(uart_ringbuffer_pop(&ch))
        {
            cmd_parser_process_char(ch);
        }
         app_battery_process();
        app_brightness_process();
    }
}