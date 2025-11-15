#include "config_smart.h"

bool ConfigSmart::checkConfig(void)
{
    return ConfigLED::checkConfig() && ConfigButton::checkConfig();
}

bool ConfigSmart::generateOutput(uint16_t* output, uint16_t button, hid_incoming_data_t* hid)
{
    bool last_state = CFG_CHECK_BIT(*output, this->JoystickButton);
    bool button_state = CFG_CHECK_BIT(button, this->ButtonPin);
    bool input_state = this->GetState(hid);

    if (button_state == input_state) {
        this->vbutton.reset();
        CFG_CLEAR_BIT(*output, this->JoystickButton);
    } else {
        bool pressed = this->vbutton.process();
        CFG_SET_BIT_VAL(*output, this->JoystickButton, pressed);
    }

    return button_state == last_state;
}
