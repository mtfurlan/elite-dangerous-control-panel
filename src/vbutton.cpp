#include "vbutton.h"
#include <bsp/board_api.h>

void VButton::reset(void)
{
    this->retryCurrent = 0;
    this->lastMillis = 0;
    this->state = VBTN_STOPPED;
}
bool VButton::process(void)
{
    switch (this->state) {
        case VBTN_STOPPED:
            this->lastMillis = board_millis();
            //printf("starting press %ld\n", this->lastMillis);
            this->state = VBTN_PUSHED;
            return true;
            break;
        case VBTN_PUSHED:
            // if we are done pressing it
            if (board_millis() - this->lastMillis > this->pressLength) {
                //printf("done press press %ld\n", board_millis());
                // if we have retry, go into unpushed else we're done, reset
                //printf("retryCount: %d, retryCurrent: %d\n", this->retryCount, this->retryCurrent);
                if (this->retryCount && ++(this->retryCurrent) <= this->retryCount) {
                    this->state = VBTN_COOLDOWN;
                    this->lastMillis = board_millis();
                    //printf("scheduled retry #%d at %ld\n", this->retryCurrent, this->lastMillis);
                } else {
                    //printf("no more retry, did %d\n", this->retryCurrent-1);
                    this->state = VBTN_DONE;
                }
                //printf("button timed out at %ld\n", this->unpushMillis);
                return false;
            } else {
                //printf("button continuing at %ld\n", board_millis());
                return true;
            }
            break;
        case VBTN_COOLDOWN:
            // if cooldown timeout start pressing, else wait
            if (board_millis() - this->lastMillis > this->cooldownLength) {
                this->lastMillis = board_millis();
                //printf("ending cooldown press %ld after %ld\n", this->lastMillis, this->cooldownLength);
                this->state = VBTN_PUSHED;
                return true;
            } else {
                return false;
            }
            break;
        case VBTN_DONE:
            return false;
            break;
    }
    // TODO: why does compiler think we can get here, I don't think it's possible?
    return false;
}
