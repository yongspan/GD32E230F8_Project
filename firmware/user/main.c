#include "gd32e23x.h"
#include "systick.h"
#include "bsp_led.h"

int main(void)
{
    systick_config();

    bsp_led_init();

    while(1)
    {
        bsp_led1_toggle();

        delay_1ms(500);
    }
}