#include "config_smart.h"
#include <stdio.h>

bool ConfigSmart::checkConfig(void)
{
    return ConfigLED::checkConfig() && ConfigButton::checkConfig();
}

bool ConfigSmart::generateOutput(uint16_t* output, uint16_t button, hid_incoming_data_t* hid)
{
    bool last_joystick = CFG_CHECK_BIT(*output, this->JoystickButton);
    bool btn = CFG_CHECK_BIT(button, this->ButtonPin);
    bool input = this->GetState(hid);


    // if there was a change
    if (this->last != btn) {
        printf("button: %d, last: %d, input: %d\n", btn, this->last, input);
        this->last = btn;
        if (btn == input) {
            this->vbutton.reset();
            printf("reset\n");
        } else {
            printf("ensure push\n");
            this->vbutton.ensure_pushing();
        }
    }

    bool pressed = this->vbutton.process();
    CFG_SET_BIT_VAL(*output, this->JoystickButton, pressed);

    return pressed == last_joystick;
}
