#include "delay.h"

static volatile uint32_t delay_count = 0;
static volatile uint32_t system_ms = 0;
void delay_init(void)
{
    SysTick_Config(SystemCoreClock / 1000);
}

void delay_decrement(void)
{
    system_ms++;

    if(delay_count > 0)
    {
        delay_count--;
    }
}

uint32_t millis(void)
{
    return system_ms;
}

void delay_ms(uint32_t ms)
{
    delay_count = ms;
    while(delay_count != 0)
    {
    }
}