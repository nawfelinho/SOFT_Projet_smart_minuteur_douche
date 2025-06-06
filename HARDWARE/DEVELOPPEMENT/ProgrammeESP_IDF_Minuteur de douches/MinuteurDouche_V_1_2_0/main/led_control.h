#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include "driver/gpio.h"

#define LED_GPIO GPIO_NUM_2  

void led_init(void);
void led_on(void);
void led_off(void);
void led_toggle(void);
int  led_get_state(void);

#endif // LED_CONTROL_H
