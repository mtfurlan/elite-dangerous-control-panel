#include "config_quadrature.h"

#include "buttons.h"

#include <map>
#include <stdio.h>

#define PIN_A 2
#define PIN_B 3

int ConfigQuadrature::init(void)
{
    this->last_a = buttons_read(this->ButtonPinA);
    this->last_b = buttons_read(this->ButtonPinB);

    gpio_init(PIN_A);
    gpio_set_dir(PIN_A, GPIO_OUT);
    gpio_init(PIN_B);
    gpio_set_dir(PIN_B, GPIO_OUT);

    return 0;
}

bool ConfigQuadrature::generateOutput(uint16_t* output, uint16_t button, hid_incoming_data_t* hid)
{
    bool last_joystick_cw = CFG_CHECK_BIT(*output, this->JoystickButtonCW);
    bool last_joystick_ccw = CFG_CHECK_BIT(*output, this->JoystickButtonCCW);

    bool a = CFG_CHECK_BIT(button, this->ButtonPinA);
    bool b = CFG_CHECK_BIT(button, this->ButtonPinB);


    // if there was a change, process quadrature update
    if (this->last_a != a || this->last_b != b) {
        int8_t last = ((last_a & 0x1) << 1) | (last_b & 0x1);
        int8_t cur = ((a & 0x1) << 1) | (b & 0x1);

        // cw 00, 01, 11, 10, 00
        // ccw 00, 10, 11, 01, 00
        std::map<int8_t, std::map<int8_t, int8_t> > map = {
            { 0b00, { { 0b01, 1 }, { 0b10, -1 } } },
            { 0b01, { { 0b11, 1 }, { 0b00, -1 } } },
            { 0b11, { { 0b10, 1 }, { 0b01, -1 } } },
            { 0b10, { { 0b00, 1 }, { 0b11, -1 } } },
        };
        // we only set last for valid states, so we know it's in the map
        // we can have invalid transitions though, so we have to find the second lookup
        printf("quad transition from %d,%d to %d,%d\n", last_a, last_b, a, b);
        if (map[last].find(cur) != map[last].end()) {
            printf("state going from %d to ", this->quad_state);
            if (a == 1 && b == 1) {
                switch (this->quad_state) {
                    case 3:
                        printf("(pushing cw) ");
                        this->vbuttonCW.push();
                        break;
                    case -3:
                        printf("(pushing ccw) ");
                        this->vbuttonCCW.push();
                        break;
                    default:
                        break;
                }
                this->quad_state = 0;
            } else {
                this->quad_state += map[last].find(cur)->second;
            }
            printf("%d\n", this->quad_state);
            this->last_a = a;
            this->last_b = b;
            gpio_put(PIN_A, !b);
            gpio_put(PIN_B, !a);
        } else {
            // do we just ignore bad transitions?
            printf("unknown quad state transition from %d,%d to %d,%d\n", last_a, last_b, a, b);
        }
    }


    // process buttons
    bool new_cw = this->vbuttonCW.process();
    bool new_ccw = this->vbuttonCCW.process();
    bool dirty = false;
    if (new_cw != last_joystick_cw) {
        CFG_SET_BIT_VAL(*output, this->JoystickButtonCW, new_cw);
        dirty = true;
    }
    if (new_ccw != last_joystick_ccw) {
        CFG_SET_BIT_VAL(*output, this->JoystickButtonCCW, new_ccw);
        dirty = true;
    }

    return dirty;
}
