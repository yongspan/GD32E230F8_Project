#ifndef DELAY_H
#define DELAY_H

#include "gd32e23x.h"

static volatile uint32_t key_add_tick = 0;
static volatile uint32_t key_sub_tick = 0;

void delay_init(void);
void delay_ms(uint32_t ms);
void delay_decrement(void);
uint32_t millis(void);
	
#endif