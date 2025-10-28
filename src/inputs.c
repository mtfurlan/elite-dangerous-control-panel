#include "inputs.h"

#include <pico/stdlib.h>


static bool sw = false;
static bool dirty = false;
/*
 * gpio: pin number triggering
 * event_mask: enum gpio_irq_level
 */
void gpio_irq(uint gpio, uint32_t event_mask) {
    (void)gpio;
    (void)event_mask;

    sw = (event_mask & GPIO_IRQ_EDGE_FALL);
    dirty = true;

}
int inputs_init(const input_config_t config[], size_t len)
{
    // TODO hardcoding pin 2, replace iwth mcp int pin and do reading or whatever
    gpio_set_irq_enabled_with_callback(2, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_irq);

    int pin;
    for(size_t i = 0; i < len; ++i) {
        pin = config[i].input_pin;
        if(pin >= 0) {
            gpio_init(pin);
            gpio_set_dir(pin, GPIO_IN);
            gpio_pull_up(pin);
        }
    }
    return 0;
}

bool inputs_task(uint32_t* inputs)
{
    *inputs = sw;
    if(dirty) {
        *inputs = sw;
        dirty = false;
        return true;
    }
    return false;
}
