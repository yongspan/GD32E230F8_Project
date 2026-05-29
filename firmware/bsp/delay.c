#include "delay.h"

static volatile uint32_t delay_count = 0;

void delay_init(void)
{
    SysTick_Config(SystemCoreClock / 1000);
}

void delay_decrement(void)
{
    if(delay_count > 0)
    {
        delay_count--;
    }
}

void delay_ms(uint32_t ms)
{
    delay_count = ms;
    while(delay_count != 0)
    {
    }
}