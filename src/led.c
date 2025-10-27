#include "led.h"

#include <pico/stdlib.h>
#include <bsp/board_api.h>

#define LED_R 18
#define LED_G 19
#define LED_B 20

int led_task(led_state_t state)
{
  // TODO state handling better
    static uint32_t start_ms = 0;
    static bool led_state = false;

    // blink is disabled
    if (!state) return 0;

    // Blink every interval ms
    if ( board_millis() - start_ms < state) return 0; // not enough time
    start_ms += state;

    gpio_put(LED_R, led_state);
    led_state = 1 - led_state; // toggle
    return 0;
}

int led_init(const input_config_t config[], size_t len)
{
    gpio_init(LED_R);
    gpio_set_dir(LED_R, GPIO_OUT);
    gpio_init(LED_G);
    gpio_set_dir(LED_G, GPIO_OUT);
    gpio_init(LED_B);
    gpio_set_dir(LED_B, GPIO_OUT);

    gpio_put(LED_R, 1);
    gpio_put(LED_G, 1);
    gpio_put(LED_B, 1);

    int pin;
    for(size_t i = 0; i < len; ++i) {
        pin = config[i].output_pin;
        if(pin >= 0) {
            gpio_init(pin);
            gpio_set_dir(pin, GPIO_OUT);
            gpio_put(LED_R, 0);
        }
    }
    return 0;
}
