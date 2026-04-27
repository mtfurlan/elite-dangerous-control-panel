#pragma once

#include <pico/stdlib.h>
#include <stdbool.h>
#include <stddef.h>

#include <mcp23017.h>

class MCPInput : virtual public Mcp23017
{
protected:
    uint interrupt_pin;
    volatile bool dirty;
    /*
     * isr requires static function, so we have to map back to class pointers
     * this is just an array of the 8 possible addressed MCPs and we check based
     * on the isr gpio
     * I guess we could have more than 8 with more i²c busses, but we can burn
     * that bridge when we get to it
     */
    static MCPInput* irqMap[8];

    public:
        MCPInput(i2c_inst_t *i2c,  uint8_t i2c_address, uint interrupt_pin);

        int init();

        bool changed();
        uint16_t read();
        bool getPin(int pin);

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
