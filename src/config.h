#ifndef CONFIG_H
#define CONFIG_H

#include "hid.h"


// function type to get hid incomfing data state for a specific config
typedef bool (*get_state_f)(hid_incoming_data_t*);

// these are offset by one because of stupid stuff, TODO reconsider or at least rename
#define CFG_CHECK_BIT(x, pos)        (x & (1UL << (pos - 1)))
#define CFG_CLEAR_BIT(x, pos)        (x &= (~(1U << (pos - 1))))
#define CFG_SET_BIT(x, pos)          (x |= (1U << (pos - 1)))
#define CFG_SET_BIT_VAL(x, pos, val) (x = val ? x | (1U << (pos - 1)) : x & ~(1U << (pos - 1)))

class Config
{
public:
    // check if config is valid on boot
    virtual bool checkConfig(void) = 0;

    /**
     * generate output based on input or time or whatever
     *
     * ouptut[in,out] bits to set in output, incoming is a mix of last state and other config new states
     * button: current button states
     * hid: data from computer
     * return: if state changed
     */
    virtual bool generateOutput(uint16_t* output, uint16_t button, hid_incoming_data_t* hid);

    virtual void setLED(uint16_t* leds, hid_incoming_data_t* hid);
};


#endif // CONFIG_H
