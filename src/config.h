#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

typedef enum {
    DIRECT, // output inputPin directly
    SMART_BTN, // do maths on input and output to decide what to do to pretend to be a toggle button
    //RISING, // send events on rising edge
    //FALLING, // send events on falling edge
    //OPTIMISTIC_SW, act as a switch and set LED based on that
} input_mode_t;

typedef struct {
    uint8_t input; // 1-32, 0 is no input
    uint8_t output; // joy btn 1-32, 0 no output
    int input_pin; // -1 is disabled // TODO: inputPin is tied to output number, seems confusing
    int output_pin; // -1 is disabled
                    // TODO: direction?
    input_mode_t mode;
} input_config_t;

#endif // CONFIG_H
