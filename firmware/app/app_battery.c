#include "app_battery.h"
#include "app_config.h"
#include "bsp_adc.h"
#include "bsp_battery_led.h"
#include "bsp_charge.h"
#include "bsp_pwm.h"
#include "delay.h"
#include "gd32e23x.h"

/*
 * Battery display policy for product behavior:
 *
 * Battery only, no USB:
 *   - SWITCH_DET(PF0) OFF: battery LEDs stay OFF to save power.
 *   - SWITCH_DET(PF0) OFF -> ON: show battery LEDs for a short time.
 *   - Key / command while switch is ON: show battery LEDs for a short time.
 *   - Turn all battery LEDs off after timeout to save power.
 *   - Low battery keeps LED1 slow blinking, even after timeout.
 *   - Critical low battery forces PWM duty to 0% to protect battery.
 *
 * USB charging:
 *   - Charging: keep battery LEDs active and animate continuously.
 *   - Full: keep all available battery LEDs ON continuously.
 *   - USB removed: return to battery-only policy; if switch is ON, show battery briefly.
 *
 * SWITCH_DET hardware:
 *   SYS_BAT+ -- 100k -- PF0 -- 100k -- GND
 */

#define BATTERY_FULL_MV              4200U
#define BATTERY_EMPTY_MV             3300U

/* Only treat STDBY/full as real full when ADC is also near full.
 * This prevents a false STDBY level from making 4 LEDs stay ON at ~3.9V.
 */
#define BATTERY_FULL_DISPLAY_MV      4150U

/* 4-cell display thresholds. In debug mode only 3 LEDs are available. */
/* 4th LED threshold is intentionally above 4.00V to avoid 4/3 flicker
 * around the ADC boundary when a 4.0V battery is measured as 399x~400x mV.
 */
#define BATTERY_LED4_MV              4050U
#define BATTERY_LED3_MV              3850U
#define BATTERY_LED2_MV              3700U
#define BATTERY_LED1_MV              3500U

#define BATTERY_LOW_WARN_MV          3500U
#define BATTERY_LOW_CUTOFF_MV        3300U

#define BATTERY_UPDATE_MS            1000U
#define BATTERY_CHARGE_ANIM_MS       500U
#define BATTERY_LOW_BLINK_MS         1000U

#define SWITCH_DET_PORT              GPIOF
#define SWITCH_DET_PIN               GPIO_PIN_0

static uint16_t battery_voltage_mv = 0;
static uint8_t battery_percent = 0;
static uint8_t battery_led_count = 0;

static uint8_t led_visible_request = 0;
static uint32_t led_hide_deadline_ms = 0;

static uint8_t charge_blink_state = 0;
static uint8_t low_blink_state = 0;
static uint8_t low_blink_half_count = 0;
static uint8_t low_cutoff_active = 0;

static charge_status_t last_charge_status = CHARGE_STATUS_UNKNOWN;
static uint8_t last_usb_active = 0;
static uint8_t last_switch_on = 0;

static void app_battery_switch_det_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOF);

    /* External 100k/100k divider already defines high/low level. */
    gpio_mode_set(SWITCH_DET_PORT,
                  GPIO_MODE_INPUT,
                  GPIO_PUPD_NONE,
                  SWITCH_DET_PIN);
}

uint8_t app_battery_is_switch_on(void)
{
    return (gpio_input_bit_get(SWITCH_DET_PORT, SWITCH_DET_PIN) == SET) ? 1U : 0U;
}

static uint8_t battery_voltage_to_percent(uint16_t mv)
{
    uint32_t percent;

    if(mv >= BATTERY_FULL_MV)
    {
        return 100U;
    }

    if(mv <= BATTERY_EMPTY_MV)
    {
        return 0U;
    }

    percent = (uint32_t)(mv - BATTERY_EMPTY_MV) * 100U;
    percent = percent / (BATTERY_FULL_MV - BATTERY_EMPTY_MV);

    return (uint8_t)percent;
}

static uint8_t battery_voltage_to_led_count(uint16_t mv)
{
    uint8_t max_count = bsp_battery_led_get_max_count();
    uint8_t count;

    if(mv >= BATTERY_LED4_MV)
    {
        count = 4U;
    }
    else if(mv >= BATTERY_LED3_MV)
    {
        count = 3U;
    }
    else if(mv >= BATTERY_LED2_MV)
    {
        count = 2U;
    }
    else if(mv >= BATTERY_LED1_MV)
    {
        count = 1U;
    }
    else
    {
        count = 0U;
    }

    if(count > max_count)
    {
        count = max_count;
    }

    return count;
}

static uint8_t app_battery_is_usb_active(void)
{
    charge_status_t charge_status = bsp_charge_get_status();

    if(bsp_charge_is_usb_connected())
    {
        return 1U;
    }

    /* In debug mode PA10/USB_DET is disabled, so CHRG/STDBY can still imply USB. */
    if((charge_status == CHARGE_STATUS_CHARGING) ||
       (charge_status == CHARGE_STATUS_FULL))
    {
        return 1U;
    }

    return 0U;
}

static charge_status_t app_battery_get_effective_charge_status(void)
{
    charge_status_t raw_status = bsp_charge_get_status();
    uint8_t usb_active = app_battery_is_usb_active();

    if(!usb_active)
    {
        return CHARGE_STATUS_NOT_CHARGING;
    }

    /*
     * Product-mode rule:
     * - Full/all-on requires BOTH STDBY/full status and ADC voltage near full.
     * - If USB is present but the battery is only around 3.9V, show charging
     *   animation even if STDBY is falsely read as active.
     */
    if((raw_status == CHARGE_STATUS_FULL) &&
       (battery_voltage_mv >= BATTERY_FULL_DISPLAY_MV))
    {
        return CHARGE_STATUS_FULL;
    }

    return CHARGE_STATUS_CHARGING;
}

static void app_battery_update_value(void)
{
    battery_voltage_mv = bsp_battery_read_mv_average(10U);
    battery_percent = battery_voltage_to_percent(battery_voltage_mv);
    battery_led_count = battery_voltage_to_led_count(battery_voltage_mv);
}

static uint8_t app_battery_get_full_mask(void)
{
    uint8_t max_count = bsp_battery_led_get_max_count();

    if(max_count >= 4U)
    {
        return 0x0FU;
    }

    return 0x07U;
}

static uint8_t app_battery_count_to_mask(uint8_t count)
{
    uint8_t max_count = bsp_battery_led_get_max_count();

    if(count > max_count)
    {
        count = max_count;
    }

    if(count == 0U)
    {
        return 0U;
    }

    return (uint8_t)((1U << count) - 1U);
}

static void app_battery_led_update(void)
{
    charge_status_t charge_status = app_battery_get_effective_charge_status();
    uint8_t usb_active = app_battery_is_usb_active();
    uint8_t switch_on = app_battery_is_switch_on();
    uint8_t current_mask;
    uint8_t next_mask;
    uint8_t max_count;

    if(usb_active && (charge_status == CHARGE_STATUS_FULL))
    {
        /* USB inserted and really full: keep all LEDs ON. */
        bsp_battery_led_set_mask(app_battery_get_full_mask());
        return;
    }

    if(usb_active && (charge_status == CHARGE_STATUS_CHARGING))
    {
        /*
         * Charging animation:
         * - Filled LEDs stay ON.
         * - The next LED blinks.
         * - Do NOT show 4 LEDs steady unless effective status is FULL.
         */
        max_count = bsp_battery_led_get_max_count();

        if(max_count == 0U)
        {
            bsp_battery_led_all_off();
            return;
        }

        if(battery_led_count >= max_count)
        {
            current_mask = app_battery_count_to_mask((uint8_t)(max_count - 1U));
            next_mask = app_battery_get_full_mask();
        }
        else
        {
            current_mask = app_battery_count_to_mask(battery_led_count);
            next_mask = (uint8_t)(current_mask | (1U << battery_led_count));
        }

        bsp_battery_led_set_mask(charge_blink_state ? next_mask : current_mask);
        return;
    }

    (void)switch_on;

    /*
     * Battery-only display policy:
     * - SWITCH_DET is used to generate ON/OFF events, but it must not force
     *   LEDs off immediately.
     * - OFF -> ON: show current battery for 5 seconds.
     * - ON -> OFF: also show current battery for 5 seconds, then turn off.
     * - USB removal: show current battery for 5 seconds, then turn off.
     * - Low battery warning overrides the normal timeout.
     */
    if(battery_voltage_mv < BATTERY_LOW_WARN_MV)
    {
        bsp_battery_led_set_mask(low_blink_state ? 0x01U : 0x00U);
        return;
    }

    if(led_visible_request)
    {
        bsp_battery_led_set_count(battery_led_count);
    }
    else
    {
        bsp_battery_led_all_off();
    }
}

void app_battery_led_show_for(uint32_t show_ms)
{
    uint32_t now = millis();

    /* Refresh ADC immediately before showing LEDs. This avoids a brief
     * stale display such as 4 LEDs first, then 3 LEDs after the next
     * 1-second battery update.
     */
    app_battery_update_value();

    led_visible_request = 1U;
    led_hide_deadline_ms = now + show_ms;
    app_battery_led_update();
}

void app_battery_led_hide(void)
{
    led_visible_request = 0U;
    bsp_battery_led_all_off();
}

uint8_t app_battery_led_is_visible(void)
{
    if(app_battery_is_usb_active())
    {
        return 1U;
    }

    if(battery_voltage_mv < BATTERY_LOW_WARN_MV)
    {
        return 1U;
    }

    return led_visible_request;
}

uint8_t app_battery_is_low(void)
{
    return (battery_voltage_mv < BATTERY_LOW_WARN_MV) ? 1U : 0U;
}

uint8_t app_battery_is_cutoff(void)
{
    return low_cutoff_active;
}

void app_battery_init(void)
{
    app_battery_switch_det_init();
    bsp_battery_led_init();
    app_battery_update_value();

    last_charge_status = app_battery_get_effective_charge_status();
    last_usb_active = app_battery_is_usb_active();
    last_switch_on = app_battery_is_switch_on();

    /* No product-mode LED流水自检 anymore. */
    if(last_usb_active)
    {
        led_visible_request = 1U;
    }
    else if(last_switch_on)
    {
        app_battery_led_show_for(APP_BATTERY_LED_SHOW_BOOT_MS);
    }
    else
    {
        app_battery_led_hide();
    }
}

void app_battery_process(void)
{
    static uint32_t last_update_ms = 0;
    static uint32_t last_anim_ms = 0;
    uint32_t now = millis();
    charge_status_t charge_status;
    uint8_t usb_active;
    uint8_t switch_on;

    usb_active = app_battery_is_usb_active();
    switch_on = app_battery_is_switch_on();

    /* Handle USB insert/remove immediately, not only on the 1-second ADC task. */
    if(usb_active != last_usb_active)
    {
        last_usb_active = usb_active;

        if(usb_active)
        {
            /* USB inserted: charging/full logic owns the LEDs continuously. */
            led_visible_request = 1U;
            app_battery_update_value();
            last_charge_status = app_battery_get_effective_charge_status();
        }
        else
        {
            /* USB removed: always show current battery for 5 seconds, then off. */
            app_battery_led_show_for(APP_BATTERY_LED_SHOW_BOOT_MS);
            last_charge_status = CHARGE_STATUS_NOT_CHARGING;
        }
    }

    /* Handle main switch edge when USB is not active. Both directions show 5 seconds. */
    if((switch_on != last_switch_on) && (!usb_active))
    {
        last_switch_on = switch_on;

        /* OFF -> ON: show battery. ON -> OFF: also show battery, then timeout. */
        app_battery_led_show_for(APP_BATTERY_LED_SHOW_BOOT_MS);
    }
    else
    {
        last_switch_on = switch_on;
    }

    if((!usb_active) && led_visible_request && ((int32_t)(now - led_hide_deadline_ms) >= 0))
    {
        led_visible_request = 0U;
    }

    if((uint32_t)(now - last_update_ms) >= BATTERY_UPDATE_MS)
    {
        last_update_ms = now;
        app_battery_update_value();

        charge_status = app_battery_get_effective_charge_status();
        usb_active = app_battery_is_usb_active();

        if(charge_status != last_charge_status)
        {
            last_charge_status = charge_status;

            if(usb_active)
            {
                /* Charging/full state changed: keep LEDs controlled by charge state. */
                led_visible_request = 1U;
            }
        }

        if((!usb_active) && (battery_voltage_mv <= BATTERY_LOW_CUTOFF_MV))
        {
            bsp_pwm_set_duty(0U);
            low_cutoff_active = 1U;
        }
        else if(battery_voltage_mv > (BATTERY_LOW_CUTOFF_MV + 100U))
        {
            low_cutoff_active = 0U;
        }

        app_battery_led_update();
    }

    if((uint32_t)(now - last_anim_ms) >= BATTERY_CHARGE_ANIM_MS)
    {
        last_anim_ms = now;

        if(app_battery_is_usb_active() && (app_battery_get_effective_charge_status() == CHARGE_STATUS_CHARGING))
        {
            charge_blink_state = !charge_blink_state;
        }
        else
        {
            charge_blink_state = 0U;
        }

        if(battery_voltage_mv < BATTERY_LOW_WARN_MV)
        {
            /* Full warning cycle is about 2 seconds: LED1 ON 1s, OFF 1s. */
            low_blink_half_count++;
            if(low_blink_half_count >= (BATTERY_LOW_BLINK_MS / BATTERY_CHARGE_ANIM_MS))
            {
                low_blink_half_count = 0U;
                low_blink_state = !low_blink_state;
            }
        }
        else
        {
            low_blink_half_count = 0U;
            low_blink_state = 0U;
        }

        app_battery_led_update();
    }
}

uint16_t app_battery_get_voltage_mv(void)
{
    return battery_voltage_mv;
}

uint8_t app_battery_get_percent(void)
{
    return battery_percent;
}

uint8_t app_battery_get_led_count(void)
{
    return battery_led_count;
}

uint8_t app_battery_get_led_max_count(void)
{
    return bsp_battery_led_get_max_count();
}
