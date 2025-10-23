#ifndef LED_H
#define LED_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdbool.h>
/* Blink pattern
 * - 250 ms  : device not mounted
 * - 1000 ms : device mounted
 * - 2500 ms : device is suspended
 */
typedef enum  {
    BLINK_NOT_MOUNTED = 1000,
    BLINK_MOUNTED = 2000,
    BLINK_SUSPENDED = 5500,
} led_state_t;

int led_init(void);
int led_task(led_state_t state);

#ifdef __cplusplus
 }
#endif
#endif // LED_H
