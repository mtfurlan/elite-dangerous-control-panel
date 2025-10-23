#include "inputs.h"
#include <pico/stdlib.h>

#define INPUT_PIN 2

int inputs_init(void)
{
    gpio_init(INPUT_PIN);
    gpio_set_dir(INPUT_PIN, GPIO_IN);
    gpio_pull_up(INPUT_PIN);
}

bool inputs_task(uint8_t* inputs)
{
    if(gpio_get(INPUT_PIN) != (*inputs & 0x1)) {
        *inputs ^= 0x1; // toggle
        return true;
    }
    return false;
}
