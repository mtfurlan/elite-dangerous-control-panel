#ifndef LED_H
#define LED_H

#include <stddef.h>
#include <mcp23017.h>

typedef enum  {
    BLINK_NOT_OFF = 0,
    BLINK_NOT_MOUNTED = 250,
    BLINK_MOUNTED = 1000,
    BLINK_SUSPENDED = 2500,
} led_state_t;

int led_init(i2c_inst* i2c, uint8_t addr);
int led_task(led_state_t state, uint16_t states);

#endif // LED_H
