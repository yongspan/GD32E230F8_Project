#include "bsp_charge.h"
#include "gd32e23x.h"
#include "app_config.h"

/*
 * PA4  = CHRG,  low = charging
 * PA5  = STDBY, low = full/standby
 * PA10 = USB_DET, only available when APP_ENABLE_UART_COMMANDS == 0
 *
 * PA10 is USART0_RX in debug mode. Do not use PA10 as USB_DET while UART RX is enabled.
 */
#define CHRG_PORT       GPIOA
#define CHRG_PIN        GPIO_PIN_4

#define STDBY_PORT      GPIOA
#define STDBY_PIN       GPIO_PIN_5

#define USB_DET_PORT    GPIOA
#define USB_DET_PIN     GPIO_PIN_10

void bsp_charge_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);

    gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP,
                  CHRG_PIN | STDBY_PIN);

#if !APP_ENABLE_UART_COMMANDS
    /* Assume USB_DET is high when USB is connected. */
    gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, USB_DET_PIN);
#endif
}

uint8_t bsp_charge_is_charging(void)
{
    return (gpio_input_bit_get(CHRG_PORT, CHRG_PIN) == RESET) ? 1U : 0U;
}

uint8_t bsp_charge_is_full(void)
{
    return (gpio_input_bit_get(STDBY_PORT, STDBY_PIN) == RESET) ? 1U : 0U;
}

uint8_t bsp_charge_is_usb_connected(void)
{
#if APP_ENABLE_UART_COMMANDS
    return 0U;
#else
    return (gpio_input_bit_get(USB_DET_PORT, USB_DET_PIN) == SET) ? 1U : 0U;
#endif
}

charge_status_t bsp_charge_get_status(void)
{
    uint8_t chrg_low;
    uint8_t stdby_low;

    chrg_low = bsp_charge_is_charging();
    stdby_low = bsp_charge_is_full();

    if((chrg_low == 1U) && (stdby_low == 0U))
    {
        return CHARGE_STATUS_CHARGING;
    }

    if((chrg_low == 0U) && (stdby_low == 1U))
    {
        return CHARGE_STATUS_FULL;
    }

    if((chrg_low == 0U) && (stdby_low == 0U))
    {
        return CHARGE_STATUS_NOT_CHARGING;
    }

    return CHARGE_STATUS_UNKNOWN;
}

const char *bsp_charge_get_status_string(void)
{
    switch(bsp_charge_get_status())
    {
        case CHARGE_STATUS_CHARGING:
            return "Charging";

        case CHARGE_STATUS_FULL:
            return "Full";

        case CHARGE_STATUS_NOT_CHARGING:
            return "Not charging";

        default:
            return "Unknown";
    }
}

const char *bsp_charge_get_usb_string(void)
{
#if APP_ENABLE_UART_COMMANDS
    return "Disabled, PA10 used by UART RX";
#else
    return bsp_charge_is_usb_connected() ? "Connected" : "Disconnected";
#endif
}
