#ifndef BSP_CHARGE_H
#define BSP_CHARGE_H

#include <stdint.h>

typedef enum
{
    CHARGE_STATUS_NOT_CHARGING = 0,
    CHARGE_STATUS_CHARGING,
    CHARGE_STATUS_FULL,
    CHARGE_STATUS_UNKNOWN
} charge_status_t;

void bsp_charge_init(void);

uint8_t bsp_charge_is_charging(void);
uint8_t bsp_charge_is_full(void);
uint8_t bsp_charge_is_usb_connected(void);

charge_status_t bsp_charge_get_status(void);
const char *bsp_charge_get_status_string(void);
const char *bsp_charge_get_usb_string(void);

#endif