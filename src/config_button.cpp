#include "config_button.h"
#include <stdio.h>

bool ConfigButton::checkConfig(void)
{
    return !(this->ButtonPin == 0 || this->JoystickButton == 0);
}

bool ConfigButton::generateOutput(uint16_t* output, uint16_t button, hid_incoming_data_t* hid)
{
    bool lastSet = CFG_CHECK_BIT(*output, this->JoystickButton);
    bool btn = CFG_CHECK_BIT(button, this->ButtonPin);

    if (btn != this->last) {
        this->last = btn;
        if (lastSet != btn) {
            CFG_SET_BIT_VAL(*output, this->JoystickButton, btn);
            return true;
        }
    }
    return false;
}
