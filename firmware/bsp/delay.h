#ifndef DELAY_H
#define DELAY_H

#include "gd32e23x.h"
#include <stdint.h>

void delay_init(void);
void delay_ms(uint32_t ms);
void delay_decrement(void);
uint32_t millis(void);

#endif
