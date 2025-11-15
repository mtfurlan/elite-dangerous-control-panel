#ifndef CONFIG_LED_H
#define CONFIG_LED_H
#include "config.h"

class ConfigLED : virtual public Config
{
protected:
    int LEDPin;
    get_state_f GetState;

public:
    ConfigLED(int led_pin, get_state_f get_state) : LEDPin(led_pin), GetState(get_state) {}

    virtual bool checkConfig(void);

    virtual void setLED(uint16_t* leds, hid_incoming_data_t* hid);
};

#endif // CONFIG_LED_H
