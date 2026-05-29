#include "gd32e23x.h"
#include "bsp_led.h"
#include "delay.h"
#include "bsp_uart.h"
#include <stdio.h>

int main(void)
{
    bsp_led_init();
    delay_init();
    bsp_uart_init();

    printf("GD32 UART OK\r\n");

    while(1)
    {
        bsp_led_toggle();
        printf("LED toggle\r\n");
        delay_ms(500);
    }
}