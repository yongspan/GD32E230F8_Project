#include "app_battery.h"
#include "bsp_adc.h"
#include "bsp_battery_led.h"
#include "bsp_charge.h"
#include "delay.h"

#define BATTERY_FULL_MV       4200
#define BATTERY_EMPTY_MV      3300

#define BATTERY_UPDATE_TICKS  300000
#define BATTERY_BLINK_TICKS   100000

static uint16_t battery_voltage_mv = 0;
static uint8_t battery_percent = 0;
static uint8_t battery_led_count = 0;

static uint8_t charge_blink_state = 0;

static uint8_t battery_voltage_to_percent(uint16_t mv)
{
    uint32_t percent;

    if(mv >= BATTERY_FULL_MV)
    {
        return 100;
    }

    if(mv <= BATTERY_EMPTY_MV)
    {
        return 0;
    }

    percent = (uint32_t)(mv - BATTERY_EMPTY_MV) * 100;
    percent = percent / (BATTERY_FULL_MV - BATTERY_EMPTY_MV);

    return (uint8_t)percent;
}

static uint8_t battery_percent_to_led_count(uint8_t percent)
{
    if(percent == 0)
    {
        return 0;
    }
    else if(percent <= 30)
    {
        return 1;
    }
    else if(percent <= 70)
    {
        return 2;
    }
    else
    {
        return 3;
    }
}

static uint8_t battery_count_to_mask(uint8_t count)
{
    if(count == 0)
    {
        return 0x00;
    }
    else if(count == 1)
    {
        return 0x01;
    }
    else if(count == 2)
    {
        return 0x03;
    }
    else
    {
        return 0x07;
    }
}

static void app_battery_update_value(void)
{
    battery_voltage_mv = bsp_battery_read_mv_average(10);
    battery_percent = battery_voltage_to_percent(battery_voltage_mv);
    battery_led_count = battery_percent_to_led_count(battery_percent);
}

static void app_battery_led_update(void)
{
    charge_status_t charge_status;
    uint8_t mask;

    charge_status = bsp_charge_get_status();

    if(charge_status == CHARGE_STATUS_FULL)
    {
        /*
         * 充满：3颗 LED 全亮
         */
        bsp_battery_led_set_mask(0x07);
    }
    else if(charge_status == CHARGE_STATUS_CHARGING)
    {
        /*
         * 充电中：
         * 0格：LED1 闪
         * 1格：LED1 常亮，LED2 闪
         * 2格：LED1+LED2 常亮，LED3 闪
         * 3格：LED1+LED2 常亮，LED3 闪
         */
        if(battery_led_count == 0)
        {
            mask = charge_blink_state ? 0x01 : 0x00;
        }
        else if(battery_led_count == 1)
        {
            mask = charge_blink_state ? 0x03 : 0x01;
        }
        else
        {
            mask = charge_blink_state ? 0x07 : 0x03;
        }

        bsp_battery_led_set_mask(mask);
    }
    else
    {
        /*
         * 未充电：按当前电量常亮
         */
        bsp_battery_led_set_count(battery_led_count);
    }
}

void app_battery_init(void)
{
    bsp_battery_led_init();

    app_battery_update_value();
    app_battery_led_update();
}

void app_battery_process(void)
{
    static uint32_t update_tick = 0;
    static uint32_t blink_tick = 0;

    update_tick++;
    blink_tick++;

    /*
     * 定时刷新电池电压和百分比
     */
    if(update_tick >= BATTERY_UPDATE_TICKS)
    {
        update_tick = 0;

        app_battery_update_value();
        app_battery_led_update();
    }

    /*
     * 充电时 LED 闪烁
     */
    if(blink_tick >= BATTERY_BLINK_TICKS)
    {
        blink_tick = 0;

        if(bsp_charge_get_status() == CHARGE_STATUS_CHARGING)
        {
            charge_blink_state = !charge_blink_state;
            app_battery_led_update();
        }
        else
        {
            charge_blink_state = 0;
            app_battery_led_update();
        }
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