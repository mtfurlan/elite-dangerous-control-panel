#pragma once

#include "hid.h"

class Config
{
public:
    void* hardware_data_source; // used so we can only run buttons that have updates


    Config(void* input)
        : hardware_data_source(input)
    {
    }

    /**
     * do any setup after system is running
     */
    virtual int init(void);

    /**
     * generate output based on input or time or whatever
     *
     * ouptut[in,out] data repoted back to computer, incoming is a mix of last state and other config new states
     * hid: data from computer
     * return: if output changed
     */
    virtual bool process(hid_button_report_t* output, hid_incoming_data_t* hid) = 0;
};
