#include "bsp_charge.h"
#include "gd32e23x.h"

/*
 * PA4  = CHRG
 * PA5  = STDBY
 * PA10 = USB_DET
 *
 * Important:
 * PA10 is also USART0_RX in the current debug version.
 *
 * 0 = keep PA10 as UART RX, USB_DET disabled
 * 1 = use PA10 as USB_DET, UART RX will not work
 */
#define ENABLE_USB_DET_ON_PA10    0

#define CHRG_PORT       GPIOA
#define CHRG_PIN        GPIO_PIN_4

#define STDBY_PORT      GPIOA
#define STDBY_PIN       GPIO_PIN_5

#define USB_DET_PORT    GPIOA
#define USB_DET_PIN     GPIO_PIN_10

void bsp_charge_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);

    /*
     * CHRG / STDBY are usually open-drain outputs.
     * Use pull-up input.
     */
    gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP,
                  CHRG_PIN | STDBY_PIN);

#if ENABLE_USB_DET_ON_PA10
    /*
     * PA10 as USB_DET input.
     * Warning: this disables normal UART RX usage on PA10.
     */
    gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, USB_DET_PIN);
#endif
}

uint8_t bsp_charge_is_charging(void)
{
    return (gpio_input_bit_get(CHRG_PORT, CHRG_PIN) == RESET) ? 1 : 0;
}

uint8_t bsp_charge_is_full(void)
{
    return (gpio_input_bit_get(STDBY_PORT, STDBY_PIN) == RESET) ? 1 : 0;
}

uint8_t bsp_charge_is_usb_connected(void)
{
#if ENABLE_USB_DET_ON_PA10
    /*
     * 假设 USB_DET 插入 USB 时为高电平。
     * 如果你实际测量相反，后面把 SET 改成 RESET。
     */
    return (gpio_input_bit_get(USB_DET_PORT, USB_DET_PIN) == SET) ? 1 : 0;
#else
    /*
     * 当前调试版 PA10 保留给 UART RX。
     */
    return 0;
#endif
}

charge_status_t bsp_charge_get_status(void)
{
    uint8_t chrg_low;
    uint8_t stdby_low;

    chrg_low = bsp_charge_is_charging();
    stdby_low = bsp_charge_is_full();

    if((chrg_low == 1) && (stdby_low == 0))
    {
        return CHARGE_STATUS_CHARGING;
    }

    if((chrg_low == 0) && (stdby_low == 1))
    {
        return CHARGE_STATUS_FULL;
    }

    if((chrg_low == 0) && (stdby_low == 0))
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