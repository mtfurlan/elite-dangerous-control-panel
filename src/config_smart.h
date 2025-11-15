#ifndef CONFIG_SMART_H
#define CONFIG_SMART_H
#include "config.h"

class ConfigSmart : public Config
{
protected:
    int JoystickButton;
    int ButtonPin;
    int LedPin;
    get_state_f GetState;

    uint32_t PushMillis;
    uint32_t UnpushMillis;
    int retry;

public:
    // TODO: I'm not real happy with the readability here
    ConfigSmart(int joystick_button, int button_pin, int led_pin, get_state_f get_state)
        : JoystickButton(joystick_button), ButtonPin(button_pin), LedPin(led_pin),
          GetState(get_state)
    {
        this->PushMillis = 0;
        this->UnpushMillis = 0;
    }

    bool checkConfig(void);
    bool generateOutput(uint16_t* output, uint16_t button, hid_incoming_data_t* hid);

    void setLED(uint16_t* leds, hid_incoming_data_t* hid);
};

#endif // CONFIG_SMART_H
