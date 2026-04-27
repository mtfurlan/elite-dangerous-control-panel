#include "config_button.h"

#define CFG_CHECK_BIT(x, pos)        (x & (1UL << (pos)))
#define CFG_CLEAR_BIT(x, pos)        (x & (~(1U << (pos))))
#define CFG_SET_BIT(x, pos)          (x | (1U << (pos)))
#define CFG_SET_BIT_VAL(x, pos, val) ((val) ? CFG_SET_BIT(x, pos) : CFG_CLEAR_BIT(x, pos))

bool ConfigButton::process(hid_button_report_t* output, hid_incoming_data_t* hid)
{
    MCPInput* mcp = (MCPInput*)hardware_data_source;
    if(mcp->getPin(ButtonPin) != last) {
        last = !last;
        output->buttons = CFG_SET_BIT_VAL(output->buttons, JoystickButton, last);
        return true;
    }
    return false;
}
