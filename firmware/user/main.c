#include "gd32e23x.h"
#include "bsp_led.h"
#include "delay.h"
#include "bsp_uart.h"
#include <stdio.h>

int main(void)
{
    uint8_t ch;

    bsp_led_init();
    delay_init();
    bsp_uart_init();

    printf("GD32 UART RX Polling Test\r\n");

    while(1)
    {
        if(bsp_uart_get_char(&ch))
        {
            printf("RX:%c\r\n", ch);
        }

        bsp_led_toggle();
        delay_ms(500);
    }
}