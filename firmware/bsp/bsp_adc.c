#include "bsp_adc.h"
#include "gd32e23x.h"
#include "delay.h"

/*
 * BAT+ -> R_UP -> PA0 -> R_DOWN -> GND
 *
 * ???? = ADC?? * (R_UP + R_DOWN) / R_DOWN
 *
 * ?????:
 * R_UP   = 100K
 * R_DOWN = 100K
 *
 * ????????????,?????????
 */
#define BAT_R_UP_KOHM      100
#define BAT_R_DOWN_KOHM    100

#define ADC_REF_MV         3300
#define ADC_MAX_VALUE      4095

void bsp_adc_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);

    /*
     * ADC clock config
     */
    rcu_adc_clock_config(RCU_ADCCK_APB2_DIV6);
    rcu_periph_clock_enable(RCU_ADC);

    /*
     * PA0 = ADC_CHANNEL_0
     */
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_0);

    adc_deinit();

    adc_resolution_config(ADC_RESOLUTION_12B);
    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);

    adc_special_function_config(ADC_SCAN_MODE, DISABLE);
    adc_special_function_config(ADC_CONTINUOUS_MODE, DISABLE);

    adc_channel_length_config(ADC_REGULAR_CHANNEL, 1);

    /*
     * ???? ADC:
     * trigger source = NONE
     * trigger config = ENABLE
     */
    adc_external_trigger_source_config(ADC_REGULAR_CHANNEL,
                                       ADC_EXTTRIG_REGULAR_NONE);

    adc_external_trigger_config(ADC_REGULAR_CHANNEL, ENABLE);

    adc_enable();

    delay_ms(1);

    /*
     * ???????,??????
     */
    // adc_calibration_enable();
}

uint16_t bsp_adc_read_raw(void)
{
    uint32_t timeout = 100000;

    adc_regular_channel_config(0,
                               ADC_CHANNEL_0,
                               ADC_SAMPLETIME_239POINT5);

    adc_flag_clear(ADC_FLAG_EOC);

    adc_software_trigger_enable(ADC_REGULAR_CHANNEL);

    while(RESET == adc_flag_get(ADC_FLAG_EOC))
    {
        if(timeout-- == 0)
        {
            return 0;
        }
    }

    adc_flag_clear(ADC_FLAG_EOC);

    return adc_regular_data_read();
}

uint16_t bsp_adc_read_mv(void)
{
    uint32_t raw;
    uint32_t mv;

    raw = bsp_adc_read_raw();

    mv = raw * ADC_REF_MV / ADC_MAX_VALUE;

    return (uint16_t)mv;
}

uint16_t bsp_battery_read_mv(void)
{
    uint32_t adc_mv;
    uint32_t bat_mv;

    adc_mv = bsp_adc_read_mv();

    bat_mv = adc_mv * (BAT_R_UP_KOHM + BAT_R_DOWN_KOHM) / BAT_R_DOWN_KOHM;

    return (uint16_t)bat_mv;
}
uint16_t bsp_battery_read_mv_average(uint8_t times)
{
    uint8_t i;
    uint32_t sum = 0;

    if(times == 0)
    {
        times = 1;
    }

    for(i = 0; i < times; i++)
    {
        sum += bsp_battery_read_mv();
        delay_ms(2);
    }

    return (uint16_t)(sum / times);
}