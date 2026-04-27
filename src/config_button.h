#pragma once
#include "config.h"
#include "mcp.h"

class ConfigButton : public Config
{
protected:
    int JoystickButton;
    int ButtonPin;
    bool last;

public:
    ConfigButton(MCPInput* input, int joystick_button, int button_pin)
        : Config(input),
        JoystickButton(joystick_button),
        ButtonPin(button_pin)
    {
    }
    bool process(hid_button_report_t* output, hid_incoming_data_t* hid);
};
