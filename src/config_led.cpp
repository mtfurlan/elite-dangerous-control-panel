#include "config_led.h"

bool ConfigLED::checkConfig(void)
{
    return !(this->LEDPin == 0);
}

void ConfigLED::setLED(uint16_t* leds, hid_incoming_data_t* hid)
{
    if (this->LEDPin) {
        bool input_state = this->GetState(hid);
        CFG_SET_BIT_VAL(*leds, this->LEDPin, input_state);
    }
}
