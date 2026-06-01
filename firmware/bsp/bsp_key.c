#include "bsp_key.h"
#include "delay.h"

/*
 * Key design for product behavior:
 * - One physical press generates only one event.
 * - Holding a key does not continuously change brightness.
 * - The next event is allowed only after the key has been released stably.
 *
 * Why this is needed:
 * A mechanical key can generate several falling edges during one press because
 * of contact bounce. A simple EXTI time debounce is not enough when the key
 * bounces again after tens of milliseconds. The pressed->released->pressed
 * state lock below prevents multiple brightness changes from one press.
 */
#define KEY_PRESS_DEBOUNCE_MS       80U
#define KEY_RELEASE_DEBOUNCE_MS     80U

static uint8_t key_add_last_state = KEY_RELEASED;
static uint8_t key_sub_last_state = KEY_RELEASED;

static volatile uint8_t key_event = KEY_EVENT_NONE;
static volatile uint8_t key_irq_pending = 0;

static volatile uint8_t key_add_armed = 1U;
static volatile uint8_t key_sub_armed = 1U;
static volatile uint32_t key_add_last_press_ms = 0U;
static volatile uint32_t key_sub_last_press_ms = 0U;

static uint32_t key_add_release_start_ms = 0U;
static uint32_t key_sub_release_start_ms = 0U;

void bsp_key_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);

    gpio_mode_set(GPIOA,
                  GPIO_MODE_INPUT,
                  GPIO_PUPD_PULLUP,
                  KEY_ADD_GPIO_PIN | KEY_SUB_GPIO_PIN);
}

uint8_t bsp_key_add_read(void)
{
    return gpio_input_bit_get(KEY_ADD_GPIO_PORT, KEY_ADD_GPIO_PIN);
}

uint8_t bsp_key_sub_read(void)
{
    return gpio_input_bit_get(KEY_SUB_GPIO_PORT, KEY_SUB_GPIO_PIN);
}

uint8_t bsp_key_add_scan(void)
{
    uint8_t current_state = bsp_key_add_read();

    if((key_add_last_state == KEY_RELEASED) && (current_state == KEY_PRESSED))
    {
        delay_ms(20);

        if(bsp_key_add_read() == KEY_PRESSED)
        {
            key_add_last_state = KEY_PRESSED;
            return 1;
        }
    }

    if(current_state == KEY_RELEASED)
    {
        key_add_last_state = KEY_RELEASED;
    }

    return 0;
}

uint8_t bsp_key_sub_scan(void)
{
    uint8_t current_state = bsp_key_sub_read();

    if((key_sub_last_state == KEY_RELEASED) && (current_state == KEY_PRESSED))
    {
        delay_ms(20);

        if(bsp_key_sub_read() == KEY_PRESSED)
        {
            key_sub_last_state = KEY_PRESSED;
            return 1;
        }
    }

    if(current_state == KEY_RELEASED)
    {
        key_sub_last_state = KEY_RELEASED;
    }

    return 0;
}

void bsp_key_exti_init(void)
{
    bsp_key_init();

    key_event = KEY_EVENT_NONE;
    key_irq_pending = 0U;
    key_add_armed = 1U;
    key_sub_armed = 1U;
    key_add_last_press_ms = 0U;
    key_sub_last_press_ms = 0U;
    key_add_release_start_ms = 0U;
    key_sub_release_start_ms = 0U;

    rcu_periph_clock_enable(RCU_CFGCMP);

    syscfg_exti_line_config(EXTI_SOURCE_GPIOA, EXTI_SOURCE_PIN2);
    syscfg_exti_line_config(EXTI_SOURCE_GPIOA, EXTI_SOURCE_PIN3);

    exti_init(EXTI_2, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
    exti_init(EXTI_3, EXTI_INTERRUPT, EXTI_TRIG_FALLING);

    exti_interrupt_flag_clear(EXTI_2);
    exti_interrupt_flag_clear(EXTI_3);

    nvic_irq_enable(EXTI2_3_IRQn, 2U);
}

static void bsp_key_rearm_service(void)
{
    uint32_t now = millis();

    if(bsp_key_add_read() == KEY_RELEASED)
    {
        if(key_add_release_start_ms == 0U)
        {
            key_add_release_start_ms = now;
        }
        else if((uint32_t)(now - key_add_release_start_ms) >= KEY_RELEASE_DEBOUNCE_MS)
        {
            key_add_armed = 1U;
        }
    }
    else
    {
        key_add_release_start_ms = 0U;
    }

    if(bsp_key_sub_read() == KEY_RELEASED)
    {
        if(key_sub_release_start_ms == 0U)
        {
            key_sub_release_start_ms = now;
        }
        else if((uint32_t)(now - key_sub_release_start_ms) >= KEY_RELEASE_DEBOUNCE_MS)
        {
            key_sub_armed = 1U;
        }
    }
    else
    {
        key_sub_release_start_ms = 0U;
    }
}

void bsp_key_exti_irq_handler(void)
{
    uint32_t now = millis();

    if(RESET != exti_interrupt_flag_get(EXTI_2))
    {
        exti_interrupt_flag_clear(EXTI_2);
        key_irq_pending = 1U;

        if((key_add_armed != 0U) &&
           (bsp_key_add_read() == KEY_PRESSED) &&
           ((uint32_t)(now - key_add_last_press_ms) >= KEY_PRESS_DEBOUNCE_MS))
        {
            key_add_armed = 0U;
            key_add_release_start_ms = 0U;
            key_add_last_press_ms = now;
            key_event = KEY_EVENT_ADD;
        }
    }

    if(RESET != exti_interrupt_flag_get(EXTI_3))
    {
        exti_interrupt_flag_clear(EXTI_3);
        key_irq_pending = 1U;

        if((key_sub_armed != 0U) &&
           (bsp_key_sub_read() == KEY_PRESSED) &&
           ((uint32_t)(now - key_sub_last_press_ms) >= KEY_PRESS_DEBOUNCE_MS))
        {
            key_sub_armed = 0U;
            key_sub_release_start_ms = 0U;
            key_sub_last_press_ms = now;
            key_event = KEY_EVENT_SUB;
        }
    }
}

uint8_t bsp_key_get_irq_pending(void)
{
    bsp_key_rearm_service();

    if(key_irq_pending)
    {
        key_irq_pending = 0U;
        return 1U;
    }

    return 0U;
}

uint8_t bsp_key_get_event(void)
{
    uint8_t event;

    bsp_key_rearm_service();

    event = key_event;
    key_event = KEY_EVENT_NONE;

    return event;
}
