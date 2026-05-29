#include "gd32e23x.h"
#include "bsp_led.h"
#include "delay.h"
#include "bsp_uart.h"
#include <stdio.h>
#include "bsp_uart_ringbuffer.h"
#include "cmd_parser.h"
#include "bsp_key.h"

int main(void)
{
    uint8_t ch;

	
    bsp_led_init();
    delay_init();
    bsp_uart_init();
    bsp_key_init();
	  bsp_key_exti_init();
    printf("GD32 UART RX Polling Test\r\n");

  while(1)
{
    while(uart_ringbuffer_pop(&ch))
    {
        cmd_parser_process_char(ch);
    }

    if(cmd_parser_get_blink_enable())
{
    bsp_led1_toggle();
    delay_ms(500);
}

//if(bsp_key_add_scan())
//{
//    printf("KEY ADD\r\n");
//}

//if(bsp_key_sub_scan())
//{
//    printf("KEY SUB\r\n");
//}

if(bsp_key_get_irq_pending())
{
    if(bsp_key_add_scan())
    {
        printf("KEY ADD IRQ\r\n");
    }

    if(bsp_key_sub_scan())
    {
        printf("KEY SUB IRQ\r\n");
    }
}

}

}