#include "gd32e23x.h"
#include "delay.h"
#include "bsp_led.h"
#include "bsp_uart.h"
#include "bsp_uart_ringbuffer.h"
#include "bsp_key.h"
#include "bsp_pwm.h"
#include "bsp_adc.h"
#include "bsp_charge.h"
#include "cmd_parser.h"
#include "app_brightness.h"
#include "app_battery.h"
#include "app_version.h"
#include "app_config.h"
#include <stdio.h>

int main(void)
{
    uint8_t ch;

    delay_init();

    bsp_led_init();

#if APP_ENABLE_UART_COMMANDS
    bsp_uart_init();
    printf("\r\n");
    printf("========== BOOT ==========\r\n");
    printf("GD32 Backlight Controller %s\r\n", APP_VERSION_STRING);
    printf("Mode: DEBUG, PA9/PA10 = UART\r\n");
#endif

#if APP_ENABLE_UART_COMMANDS
    printf("Init charge...\r\n");
#endif
    bsp_charge_init();

#if APP_ENABLE_UART_COMMANDS
    printf("Init key...\r\n");
#endif
    bsp_key_exti_init();

#if APP_ENABLE_UART_COMMANDS
    printf("Init pwm...\r\n");
#endif
    bsp_pwm_init();

#if APP_ENABLE_UART_COMMANDS
    printf("Init adc...\r\n");
#endif
    bsp_adc_init();
#if APP_ENABLE_UART_COMMANDS
    printf("Init adc done.\r\n");
#endif

#if APP_ENABLE_UART_COMMANDS
    printf("Init brightness...\r\n");
#endif
    app_brightness_init();

#if APP_ENABLE_UART_COMMANDS
    printf("Init battery...\r\n");
#endif
    app_battery_init();

#if APP_ENABLE_UART_COMMANDS
    printf("UART RX Ready\r\n");
    printf("Type help; for commands\r\n");
#endif

    while(1)
    {
#if APP_ENABLE_UART_COMMANDS
        while(uart_ringbuffer_pop(&ch))
        {
            cmd_parser_process_char((char)ch);
        }
#else
        (void)ch;
#endif

        app_brightness_process();
        app_battery_process();
    }
}
