#ifndef LED_H
#define LED_H

#include "config.h"

#include <stddef.h>

typedef enum  {
    BLINK_NOT_OFF = 0,
    BLINK_NOT_MOUNTED = 250,
    BLINK_MOUNTED = 1000,
    BLINK_SUSPENDED = 2500,
} led_state_t;

int led_init(const input_config_t config[], size_t len);
int led_task(led_state_t state);

#endif // LED_H
