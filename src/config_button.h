#ifndef CONFIG_DIRECT_H
#define CONFIG_DIRECT_H
#include "config.h"

class ConfigButton : virtual public Config
{
protected:
    int JoystickButton;
    int ButtonPin;

public:
    ConfigButton(int joystick_button, int button_pin)
        : JoystickButton(joystick_button), ButtonPin(button_pin)
    {
    }
    virtual bool checkConfig(void);
    virtual bool generateOutput(uint16_t* output, uint16_t button, hid_incoming_data_t* hid);
};

#endif // CONFIG_DIRECT_H
