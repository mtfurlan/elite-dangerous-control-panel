#include "config_direct.h"
#include <stdio.h>

bool ConfigDirect::checkConfig(void)
{
    return !(this->ButtonPin == 0 || this->JoystickButton == 0);
}

bool ConfigDirect::generateOutput(uint16_t* output, uint16_t button, hid_incoming_data_t* hid)
{
    bool lastSet = CFG_CHECK_BIT(*output, this->JoystickButton);
    bool buttonPressed = CFG_CHECK_BIT(button, this->ButtonPin);

    if (lastSet != buttonPressed) {
        CFG_SET_BIT_VAL(*output, this->JoystickButton, buttonPressed);
        return true;
    }
    return false;
}
