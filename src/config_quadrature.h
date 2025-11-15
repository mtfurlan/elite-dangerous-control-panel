#ifndef CONFIG_QUADRATURE_H
#define CONFIG_QUADRATURE_H
#include "config.h"
#include "config_button.h"
#include "config_led.h"
#include "vbutton.h"

class ConfigQuadrature : virtual public Config
{
protected:
    int JoystickButtonCW;
    int JoystickButtonCCW;
    int ButtonPinA;
    int ButtonPinB;
    VButton vbuttonCW;
    VButton vbuttonCCW;

    bool last_a;
    bool last_b;

    int quad_state;

public:
    ConfigQuadrature(int joystick_button_cw, int joystick_button_ccw, int button_a, int button_b)
        : JoystickButtonCW(joystick_button_cw), JoystickButtonCCW(joystick_button_ccw),
          ButtonPinA(button_a), ButtonPinB(button_b), vbuttonCW(0, 50, 50),
          vbuttonCCW(0, 50, 50), last_a(false), last_b(false), quad_state(0)
    {
    }

    virtual bool checkConfig(void);
    virtual bool generateOutput(uint16_t* output, uint16_t button, hid_incoming_data_t* hid);
};

#endif // CONFIG_QUADRATURE_H
