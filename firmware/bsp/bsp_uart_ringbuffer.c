#include "bsp_uart_ringbuffer.h"

static volatile uint8_t uart_rx_buffer[UART_RX_BUFFER_SIZE];
static volatile uint16_t uart_rx_head = 0;
static volatile uint16_t uart_rx_tail = 0;

void uart_ringbuffer_init(void)
{
    uart_rx_head = 0;
    uart_rx_tail = 0;
}

uint8_t uart_ringbuffer_is_empty(void)
{
    return (uart_rx_head == uart_rx_tail);
}

uint8_t uart_ringbuffer_is_full(void)
{
    return (((uart_rx_head + 1) % UART_RX_BUFFER_SIZE) == uart_rx_tail);
}

uint8_t uart_ringbuffer_push(uint8_t data)
{
    if(uart_ringbuffer_is_full())
    {
        return 0;
    }

    uart_rx_buffer[uart_rx_head] = data;
    uart_rx_head = (uart_rx_head + 1) % UART_RX_BUFFER_SIZE;

    return 1;
}

uint8_t uart_ringbuffer_pop(uint8_t *data)
{
    if(uart_ringbuffer_is_empty())
    {
        return 0;
    }

    *data = uart_rx_buffer[uart_rx_tail];
    uart_rx_tail = (uart_rx_tail + 1) % UART_RX_BUFFER_SIZE;

    return 1;
}