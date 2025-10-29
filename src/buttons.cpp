#include "buttons.h"

#include <stdio.h>

#include <pico/stdlib.h>

#include <mcp23017.h>


static bool interrupt_mcp = true;
static Mcp23017* mcp_0;
/*
 * gpio: pin number triggering
 * event_mask: enum gpio_irq_level
 */
void mcp_irq(uint gpio, uint32_t event_mask) {
    (void)gpio;

    if (event_mask & GPIO_IRQ_EDGE_FALL) {
        interrupt_mcp = true;
    }
}
int buttons_init(uint interrupt_pin, i2c_inst* i2c, uint8_t addr)
{
    // static cause I dunno how to separate declaration and initialization
    static Mcp23017 _mcp(i2c, addr);
    mcp_0 = &_mcp;
    int result = 0;

	result |= mcp_0->setup(true, false);
	result |= mcp_0->set_io_direction(0xFFFF);
	result |= mcp_0->set_pullup(0xFFFF);
	result |= mcp_0->set_interrupt_type(0x0000); // will interrupt on both edges
	result |= mcp_0->enable_interrupt(0xFFFF);

    if(result) {
        printf("failed to init buttons mcp\n");
        return 1;
    }

    gpio_set_irq_enabled_with_callback(interrupt_pin, GPIO_IRQ_EDGE_FALL, true, &mcp_irq);

    return 0;
}

bool buttons_task(uint32_t* inputs)
{
    if(interrupt_mcp) {
        interrupt_mcp = false;
        mcp_0->update_and_get_input_values();
        *inputs = 0;
        uint16_t data = ~mcp_0->get_last_input_pin_values();
        *inputs = data;
        return true;
    }
    return false;
}
