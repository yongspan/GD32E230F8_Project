#ifndef APP_BRIGHTNESS_H
#define APP_BRIGHTNESS_H

#include <stdint.h>

void app_brightness_init(void);
void app_brightness_process(void);

uint8_t app_brightness_get_level(void);
uint8_t app_brightness_get_duty(void);
uint8_t app_brightness_get_max_level(void);

void app_brightness_set_level(uint8_t level);
void app_brightness_set_duty(uint8_t duty);
void app_brightness_factory_reset(void);

#endif