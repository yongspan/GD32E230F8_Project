#ifndef __BSP_UART_H
#define __BSP_UART_H

#include "gd32e23x.h"
#include <stdio.h>

void bsp_uart_init(void);
void bsp_uart_send_char(uint8_t ch);
void bsp_uart_send_string(const char *str);
uint8_t bsp_uart_get_char(uint8_t *ch);
#endif