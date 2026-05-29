#ifndef __BSP_UART_RINGBUFFER_H
#define __BSP_UART_RINGBUFFER_H

#include "gd32e23x.h"

#define UART_RX_BUFFER_SIZE 64

void uart_ringbuffer_init(void);
uint8_t uart_ringbuffer_push(uint8_t data);
uint8_t uart_ringbuffer_pop(uint8_t *data);
uint8_t uart_ringbuffer_is_empty(void);
uint8_t uart_ringbuffer_is_full(void);

#endif