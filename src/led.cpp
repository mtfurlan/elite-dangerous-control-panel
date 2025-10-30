#include "led.h"

#include <pico/stdlib.h>
#include <bsp/board_api.h>

#define LED_R 18
#define LED_G 19
#define LED_B 20

static Mcp23017* mcp_0;


int led_init(i2c_inst* i2c, uint8_t addr)
{
    // static cause I dunno how to separate declaration and initialization
    static Mcp23017 _mcp(i2c, addr);
    mcp_0 = &_mcp;

    gpio_init(LED_R);
    gpio_set_dir(LED_R, GPIO_OUT);
    gpio_init(LED_G);
    gpio_set_dir(LED_G, GPIO_OUT);
    gpio_init(LED_B);
    gpio_set_dir(LED_B, GPIO_OUT);

    gpio_put(LED_R, 1);
    gpio_put(LED_G, 1);
    gpio_put(LED_B, 1);

    int result = 0;

    result |= mcp_0->set_io_direction(0x0000);
    result |= mcp_0->set_pullup(0xFFFF);

    if(result) {
        printf("failed to init led mcp\n");
        return 1;
    }
    return 0;
}

int led_task(led_state_t state, uint16_t states)
{
    if(~states != mcp_0->get_output()) {
        mcp_0->set_all_output_bits(~states);
    }

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

void led_error(void)
{
    static uint32_t start_ms = 0;
    static bool led_state = false;

    // Blink every interval ms
    if ( board_millis() - start_ms < 100) return; // not enough time
    start_ms += 100;

    gpio_put(LED_R, led_state);
    led_state = 1 - led_state; // toggle
}
