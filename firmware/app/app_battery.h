#ifndef APP_BATTERY_H
#define APP_BATTERY_H

#include <stdint.h>

void app_battery_init(void);
void app_battery_process(void);
void app_battery_led_show_for(uint32_t show_ms);
void app_battery_led_hide(void);
uint8_t app_battery_led_is_visible(void);
uint8_t app_battery_is_low(void);
uint8_t app_battery_is_cutoff(void);
uint8_t app_battery_is_switch_on(void);

uint16_t app_battery_get_voltage_mv(void);
uint8_t app_battery_get_percent(void);
uint8_t app_battery_get_led_count(void);
uint8_t app_battery_get_led_max_count(void);

#endif
