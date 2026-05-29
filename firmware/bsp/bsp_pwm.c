#include "bsp_pwm.h"

#define PWM_TIMER              TIMER2
#define PWM_TIMER_RCU          RCU_TIMER2
#define PWM_TIMER_CH           TIMER_CH_0

#define PWM_GPIO_PORT          GPIOA
#define PWM_GPIO_RCU           RCU_GPIOA
#define PWM_GPIO_PIN           GPIO_PIN_6
#define PWM_GPIO_AF            GPIO_AF_1

/* 72MHz / 72 = 1MHz, 1MHz / 100 = 10kHz */
#define PWM_PERIOD             99U

static uint8_t pwm_duty = 0;

void bsp_pwm_init(void)
{
    timer_parameter_struct timer_initpara;
    timer_oc_parameter_struct timer_ocinitpara;

    rcu_periph_clock_enable(PWM_GPIO_RCU);
    rcu_periph_clock_enable(PWM_TIMER_RCU);

    gpio_af_set(PWM_GPIO_PORT, PWM_GPIO_AF, PWM_GPIO_PIN);
    gpio_mode_set(PWM_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, PWM_GPIO_PIN);
    gpio_output_options_set(PWM_GPIO_PORT,
                            GPIO_OTYPE_PP,
                            GPIO_OSPEED_50MHZ,
                            PWM_GPIO_PIN);

    timer_deinit(PWM_TIMER);

    timer_struct_para_init(&timer_initpara);
    timer_initpara.prescaler        = (SystemCoreClock / 1000000U) - 1U;
    timer_initpara.alignedmode      = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection = TIMER_COUNTER_UP;
    timer_initpara.period           = PWM_PERIOD;
    timer_initpara.clockdivision    = TIMER_CKDIV_DIV1;
    timer_init(PWM_TIMER, &timer_initpara);

    timer_channel_output_struct_para_init(&timer_ocinitpara);
    timer_ocinitpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocinitpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocinitpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocinitpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocinitpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    timer_channel_output_config(PWM_TIMER, PWM_TIMER_CH, &timer_ocinitpara);
    timer_channel_output_mode_config(PWM_TIMER, PWM_TIMER_CH, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(PWM_TIMER, PWM_TIMER_CH, TIMER_OC_SHADOW_DISABLE);

    timer_auto_reload_shadow_enable(PWM_TIMER);

    bsp_pwm_set_duty(100);

    timer_enable(PWM_TIMER);
}

void bsp_pwm_set_duty(uint8_t duty)
{
    uint32_t pulse;

    if(duty > 100)
    {
        duty = 100;
    }

    pwm_duty = duty;

    pulse = ((PWM_PERIOD + 1U) * duty) / 100U;

    timer_channel_output_pulse_value_config(PWM_TIMER,
                                             PWM_TIMER_CH,
                                             pulse);
}

uint8_t bsp_pwm_get_duty(void)
{
    return pwm_duty;
}