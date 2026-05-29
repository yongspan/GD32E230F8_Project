#include "bsp_uart.h"
#include "bsp_uart_ringbuffer.h"

volatile uint8_t uart_rx_flag = 0;
volatile uint8_t uart_rx_data = 0;

void bsp_uart_init(void)
{
	  uart_ringbuffer_init();
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_USART0);

    /* PA9  = USART0_TX */
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_9);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_9);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);

    /* PA10 = USART0_RX */
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_10);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);

    usart_deinit(USART0);
    usart_baudrate_set(USART0, 115200U);
    usart_word_length_set(USART0, USART_WL_8BIT);
    usart_stop_bit_set(USART0, USART_STB_1BIT);
    usart_parity_config(USART0, USART_PM_NONE);
    usart_hardware_flow_rts_config(USART0, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(USART0, USART_CTS_DISABLE);

    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);

    usart_enable(USART0);
		
		nvic_irq_enable(USART0_IRQn, 2U);
    usart_interrupt_enable(USART0, USART_INT_RBNE);
}

void bsp_uart_send_char(uint8_t ch)
{
    usart_data_transmit(USART0, ch);

    while(RESET == usart_flag_get(USART0, USART_FLAG_TBE))
    {
    }
}

void bsp_uart_send_string(const char *str)
{
    while(*str)
    {
        bsp_uart_send_char((uint8_t)*str);
        str++;
    }
}

uint8_t bsp_uart_get_char(uint8_t *ch)
{
    if(RESET != usart_flag_get(USART0, USART_FLAG_RBNE))
    {
        *ch = (uint8_t)usart_data_receive(USART0);
        return 1;
    }

    return 0;
}

int fputc(int ch, FILE *f)
{
    bsp_uart_send_char((uint8_t)ch);
    return ch;
}