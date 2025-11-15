#include "config_smart.h"
#include <bsp/board_api.h>

bool ConfigSmart::checkConfig(void)
{
    return !(this->ButtonPin == 0 || this->JoystickButton == 0);
}

/*
 * for a smart button, work out what to tell usb based on state
 * ideal operation
 * flip switch, input off
 * set output
 * input on
 * stop setting output
 *
 * unflip switch
 * set output
 * input off
 * stop setting output
 *
 *
 *
 * set: do we set output
 * c: smart config and state data {uint32_t push_millis, uint32_t unpush_millis, int retry}
 * button_state: if button is pressed
 * input_state: if computer has asserted a state
 * return if we have made state dirty
 */
bool ConfigSmart::generateOutput(uint16_t* output, uint16_t button, hid_incoming_data_t* hid)
{
    //bool lastSet = CFG_CHECK_BIT(*output, this->JoystickButton);
    bool button_state = CFG_CHECK_BIT(button, this->ButtonPin);
    bool input_state = this->GetState(hid);

    CFG_CLEAR_BIT(*output, this->JoystickButton);
    if (button_state == input_state && this->PushMillis == 0) {
        // state agrees and we don't have a pushed button
        this->retry = 0;
        this->PushMillis = 0;
        this->UnpushMillis = 0;
        return false;
    }
    // if millis set and we haven't timed out
    //printf("push millis: %ld, unpush millis: %ld, button: %d, led: %d %04X; ",
    //       this->PushMillis,
    //       this->UnpushMillis,
    //       button_state,
    //       input_state,
    //       hid_incoming_data.leds);
    // if we currently have a button pressed
    if (this->PushMillis != 0) {
        // if we are done pressing it
        if (board_millis() - this->PushMillis > SMART_BUTTON_PRESS) {
            this->PushMillis = 0;
            this->UnpushMillis = board_millis();
            //printf("button timed out at %ld\n", this->UnpushMillis);
            return true;
        } else {
            //printf("button continuing at %ld\n", board_millis());
            CFG_SET_BIT(*output, this->JoystickButton);
        }
        // else we either are in retry or new button
    } else if (button_state ^ input_state) {
        if (this->retry >= SMART_BUTTON_RETRY) {
            // too many retries, give up
            //printf("too many retries gave up\n");
            // TODO: set an error state or something?
        } else {
            if (this->UnpushMillis == 0
                || (board_millis() - this->UnpushMillis > SMART_BUTTON_COOLDOWN)) {
                this->PushMillis = board_millis();
                this->UnpushMillis = 0;
                this->retry++;
                //printf("button xor led triggered at %ld\n", this->PushMillis);
                CFG_SET_BIT(*output, this->JoystickButton);
                return true;
            } else {
                //printf("waiting for unpush timeout %ld %ld %ld\n",
                //       board_millis(),
                //       this->UnpushMillis,
                //       board_millis() - this->UnpushMillis);
            }
        }
    } else if (button_state == input_state) {
        // they match, reset retry
    }
    return false;
}

void ConfigSmart::setLED(uint16_t* leds, hid_incoming_data_t* hid)
{
    if (this->LedPin) {
        bool input_state = this->GetState(hid);
        CFG_SET_BIT_VAL(*leds, this->LedPin, input_state);
    }
}
