#ifndef LED_H
#define LED_H

typedef enum  {
    BLINK_NOT_OFF = 0,
    BLINK_NOT_MOUNTED = 250,
    BLINK_MOUNTED = 1000,
    BLINK_SUSPENDED = 2500,
} led_state_t;

int led_init(void);
int led_task(led_state_t state);

#endif // LED_H
