#include "gd32e23x.h"
#include "bsp_led.h"
#include "delay.h"
#include "bsp_uart.h"
#include <stdio.h>
#include "bsp_uart_ringbuffer.h"
#include "cmd_parser.h"
#include "bsp_key.h"
#include "bsp_pwm.h"

int main(void)
{
    uint8_t ch;

	
    bsp_led_init();
    delay_init();
    bsp_uart_init();
    bsp_key_init();
	  bsp_key_exti_init();
	
    bsp_pwm_init();
    bsp_pwm_set_duty(20);
	
    printf("GD32 UART RX Polling Test\r\n");

while(1)
{
    while(uart_ringbuffer_pop(&ch))
    {
        cmd_parser_process_char(ch);
    }

    if(bsp_key_add_scan())
    {
        printf("KEY ADD\r\n");
    }

    if(bsp_key_sub_scan())
    {
        printf("KEY SUB\r\n");
    }
}

}