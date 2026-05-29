#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#include "gd32e23x.h"

#define KEY_ADD_GPIO_PORT   GPIOA
#define KEY_ADD_GPIO_PIN    GPIO_PIN_2

#define KEY_SUB_GPIO_PORT   GPIOA
#define KEY_SUB_GPIO_PIN    GPIO_PIN_3

#define KEY_PRESSED         0
#define KEY_RELEASED        1

void bsp_key_init(void);
uint8_t bsp_key_add_read(void);
uint8_t bsp_key_sub_read(void);

uint8_t bsp_key_add_scan(void);
uint8_t bsp_key_sub_scan(void);

#endif