#include "inputs.h"

#include <pico/stdlib.h>

int inputs_init(const input_config_t config[], size_t len)
{
    int pin;
    for(size_t i = 0; i < len; ++i) {
        pin = config[i].output_pin;
        if(pin >= 0) {
            gpio_init(pin);
            gpio_set_dir(pin, GPIO_IN);
            gpio_pull_up(pin);
        }
    }
}

bool inputs_task(uint8_t* inputs)
{
    if(gpio_get(2) != (*inputs & 0x1)) {
        *inputs ^= 0x1; // toggle
        return true;
    }
    return false;
}
