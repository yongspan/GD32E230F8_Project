#ifndef BSP_BATTERY_LED_H
#define BSP_BATTERY_LED_H

#include <stdint.h>

void bsp_battery_led_init(void);
void bsp_battery_led_all_off(void);
void bsp_battery_led_set_count(uint8_t count);

/* ??:? bit ?? 3 ? LED */
void bsp_battery_led_set_mask(uint8_t mask);

#endif