#ifndef APP_BATTERY_H
#define APP_BATTERY_H

#include <stdint.h>

void app_battery_init(void);
void app_battery_process(void);

uint16_t app_battery_get_voltage_mv(void);
uint8_t app_battery_get_percent(void);
uint8_t app_battery_get_led_count(void);

#endif