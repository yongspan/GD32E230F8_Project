#ifndef __BSP_LED_H
#define __BSP_LED_H

#include "gd32e23x.h"

#define LED1_GPIO_PORT        GPIOA
#define LED1_GPIO_PIN         GPIO_PIN_6

#define LED2_GPIO_PORT        GPIOA
#define LED2_GPIO_PIN         GPIO_PIN_7

void bsp_led_init(void);

void bsp_led1_on(void);
void bsp_led1_off(void);
void bsp_led1_toggle(void);

void bsp_led2_on(void);
void bsp_led2_off(void);
void bsp_led2_toggle(void);

#endif