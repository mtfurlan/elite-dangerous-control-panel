#ifndef BUTTONS_H
#define BUTTONS_H

#include <pico/stdlib.h>
#include <stdbool.h>
#include <stddef.h>

#include <mcp23017.h>

/*
 * initialze buttons module
 */
int buttons_init(uint interrupt_pin, i2c_inst* i2c, uint8_t addr);

/*
 * inputs[out] output var for data from buttons, only updated when changed
 * @return if button data is changed
 */
bool buttons_task(uint16_t* inputs);

/*
 * get a pin, must call
 */
bool buttons_read(int pin);

#endif // BUTTONS_H
