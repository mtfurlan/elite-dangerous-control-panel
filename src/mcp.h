#pragma once

#include <pico/stdlib.h>
#include <stdbool.h>
#include <stddef.h>

#include <mcp23017.h>

class MCPInput : virtual public Mcp23017
{
protected:
    uint interrupt_pin;
    bool interrupt_mcp;
    static MCPInput* irqMap[8];

    public:
        MCPInput(i2c_inst_t *i2c,  uint8_t i2c_address, uint interrupt_pin);

        int init();

        bool changed();
        uint16_t read();

        static void irq(uint gpio, uint32_t event_mask);
};

class MCPOutput : virtual public Mcp23017
{
protected:
    public:
        MCPOutput(i2c_inst_t *i2c,  uint8_t i2c_address);

        int init();

        void write(uint16_t state);
};
