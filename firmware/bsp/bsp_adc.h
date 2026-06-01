#ifndef BSP_ADC_H
#define BSP_ADC_H

#include <stdint.h>

void bsp_adc_init(void);
uint16_t bsp_adc_read_raw(void);
uint16_t bsp_adc_read_mv(void);
uint16_t bsp_battery_read_mv(void);
uint16_t bsp_battery_read_mv_average(uint8_t times);
#endif