#ifndef CONFIG_SMART_H
#define CONFIG_SMART_H
#include "config.h"
#include "config_button.h"
#include "config_led.h"
#include "vbutton.h"

class ConfigSmart : virtual public Config, public ConfigLED, public ConfigButton
{
protected:
    VButton vbutton;

public:
    // TODO: I'm not real happy with the readability here mixing super constructors and code in the header
    ConfigSmart(int joystick_button, int button_pin, int led_pin, get_state_f get_state)
        : ConfigLED(led_pin, get_state), ConfigButton(joystick_button, button_pin),
          vbutton(3, 100, 1000)
    {
    }

    virtual bool checkConfig(void);
    virtual bool generateOutput(uint16_t* output, uint16_t button, hid_incoming_data_t* hid);
};

#endif // CONFIG_SMART_H
