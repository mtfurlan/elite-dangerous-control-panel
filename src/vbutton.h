#ifndef VBUTTON_H
#define VBUTTON_H
#include "config.h"

typedef enum {
    VBTN_STOPPED,
    VBTN_PUSHED,
    VBTN_COOLDOWN,
    VBTN_DONE,
} VButton_state_e;

class VButton
{
protected:
    uint32_t lastMillis;
    int retryCurrent;
    VButton_state_e state;

public:
    VButton(int retryCount, int pressLength, int cooldownLength)
        : retryCount(retryCount), pressLength(pressLength), cooldownLength(cooldownLength)
    {
        this->reset();
    }
    /**
     * reset state to stopped
     */
    void reset(void);

    /**
     * schedule *additional* push somehow
     */
    void push(void);

    /*
     * restart if stopped
     */
    void ensure_pushing(void);

    /*
     * run button
     * will start pressing if not running currently
     *
     * @return pressed
     */
    bool process(void);

    int retryCount;
    int pressLength;
    int cooldownLength;
};

#endif // VBUTTON_H
