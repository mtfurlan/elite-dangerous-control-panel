#include "mcp.h"

#include <stdio.h>

#include <pico/stdlib.h>

#include <mcp23017.h>


MCPInput* MCPInput::irqMap[8] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

MCPInput::MCPInput(i2c_inst_t *i2c,  uint8_t i2c_address, uint interrupt_pin)
    : Mcp23017(i2c, i2c_address), interrupt_pin(interrupt_pin)
{
    irqMap[i2c_address - 0x20] = this;
}

void MCPInput::irq(uint gpio, uint32_t event_mask)
{

    if (event_mask & GPIO_IRQ_EDGE_FALL) {
        for(size_t i = 0; i < 8; ++i) {
            if (MCPInput::irqMap[i] != NULL && MCPInput::irqMap[i]->interrupt_pin == gpio) {
                MCPInput::irqMap[i]->interrupt_mcp = true;
            }
        }
    }
}

int MCPInput::init()
{
    int result = 0;

    result |= setup(true, false);
    result |= set_io_direction(0xFFFF);
    result |= set_pullup(0xFFFF);
    result |= set_interrupt_type(0x0000); // will interrupt on both edges
    result |= enable_interrupt(0xFFFF);

    if (result) {
        return 1;
    }

    gpio_set_irq_enabled_with_callback(interrupt_pin,
            GPIO_IRQ_EDGE_FALL,
            true,
            MCPInput::irq
            );

    return 0;
}

bool MCPInput::changed()
{
    return interrupt_mcp;
}
uint16_t MCPInput::read()
{
    if(interrupt_mcp) {
        return ~update_and_get_input_values();
    } else {
        return ~get_last_input_pin_values();
    }
}



// ======================================================
MCPOutput::MCPOutput(i2c_inst_t *i2c,  uint8_t i2c_address)
    : Mcp23017(i2c, i2c_address)
{
}
int MCPOutput::init()
{
    int result = 0;
    result |= set_io_direction(0x0000);
    result |= set_pullup(0xFFFF);

    return result != 0;
}
void MCPOutput::write(uint16_t state)
{
    if (((uint16_t)~state) != get_output()) {
        set_all_output_bits(~state);
    }
}
