#ifndef __BSP_PWM_H
#define __BSP_PWM_H

#include "gd32e23x.h"

void bsp_pwm_init(void);
void bsp_pwm_set_duty(uint8_t duty);
uint8_t bsp_pwm_get_duty(void);

#endif