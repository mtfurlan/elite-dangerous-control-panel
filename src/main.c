/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <bsp/board_api.h>
#include <tusb.h>

#include "usb_descriptors.h"
#include "led.h"

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+

static led_state_t led_state = BLINK_NOT_MOUNTED;

bool inputs_task(void);
void cdc_task(void);
void hid_task(void);
/**
 * Macro to call the provided function and return if error
 * @param f The function to call
 */
#define INIT_MACRO(f, ...)                  \
    do {                                    \
        int err = f(__VA_ARGS__);           \
        if (err) {                          \
            Serial1.printf(#f " failed: %d", err); \
            return err    ;                 \
        }                                   \
    } while (0)


int inputs_init()
{
    //TODO: input
    return 0;
}
/*------------- MAIN -------------*/
int main(void)
{
    board_init();
    led_init();
    inputs_init();

    // init device stack on configured roothub port
    tud_init(BOARD_TUD_RHPORT);

    if (board_init_after_tusb) {
        board_init_after_tusb();
    }

    while (1)
    {
        tud_task(); // tinyusb device task
        led_task(led_state);
        cdc_task();
        bool dirty = inputs_task();

        hid_task();
    }
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
    led_state = BLINK_MOUNTED;
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
    led_state = BLINK_NOT_MOUNTED;
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
    (void) remote_wakeup_en;
    led_state = BLINK_SUSPENDED;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
    led_state = tud_mounted() ? BLINK_MOUNTED : BLINK_NOT_MOUNTED;
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

static void send_hid_report(uint8_t report_id, uint32_t btn)
{
    // skip if hid is not ready yet
    if ( !tud_hid_ready() ) return;

    switch(report_id)
    {
        case REPORT_ID_KEYBOARD:
            {
                // use to avoid send multiple consecutive zero report for keyboard
                static bool has_keyboard_key = false;

                if ( btn )
                {
                    uint8_t keycode[6] = { 0 };
                    keycode[0] = HID_KEY_A;

                    tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keycode);
                    has_keyboard_key = true;
                }else
                {
                    // send empty key report if previously has key pressed
                    if (has_keyboard_key) tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
                    has_keyboard_key = false;
                }
            }
            break;

        case REPORT_ID_MOUSE:
            {
                int8_t const delta = 5;

                // no button, right + down, no scroll, no pan
                // annoying
                // tud_hid_mouse_report(REPORT_ID_MOUSE, 0x00, delta, delta, 0, 0);
            }
            break;

        case REPORT_ID_CONSUMER_CONTROL:
            {
                // use to avoid send multiple consecutive zero report
                static bool has_consumer_key = false;

                if ( btn )
                {
                    // volume down
                    uint16_t volume_down = HID_USAGE_CONSUMER_VOLUME_DECREMENT;
                    tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &volume_down, 2);
                    has_consumer_key = true;
                }else
                {
                    // send empty key report (release key) if previously has key pressed
                    uint16_t empty_key = 0;
                    if (has_consumer_key) tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &empty_key, 2);
                    has_consumer_key = false;
                }
            }
            break;

        case REPORT_ID_GAMEPAD:
            {
                // use to avoid send multiple consecutive zero report for keyboard
                static bool has_gamepad_key = false;

                hid_gamepad_report_t report =
                {
                    .x   = 0, .y = 0, .z = 0, .rz = 0, .rx = 0, .ry = 0,
                    .hat = 0, .buttons = 0
                };

                if ( btn )
                {
                    report.hat = GAMEPAD_HAT_UP;
                    report.buttons = GAMEPAD_BUTTON_A;
                    tud_hid_report(REPORT_ID_GAMEPAD, &report, sizeof(report));

                    has_gamepad_key = true;
                }else
                {
                    report.hat = GAMEPAD_HAT_CENTERED;
                    report.buttons = 0;
                    if (has_gamepad_key) tud_hid_report(REPORT_ID_GAMEPAD, &report, sizeof(report));
                    has_gamepad_key = false;
                }
            }
            break;

        default: break;
    }
}

uint8_t inputSwitches = 0x0;
bool inputs_task()
{
    // TODO: read switch?
    //if (readMCP != 0 && millis() - readMCP > debounce_time) {
    //    readMCP = 0;
    //    inputSwitches = mcp.readGPIOAB();
    //    Serial1.printf("input switches changed: %02X\n", inputSwitches);
    //    return true;
    //}
    return false;
}
// Every 10ms, we will sent 1 report for each HID profile (keyboard, mouse etc ..)
// tud_hid_report_complete_cb() is used to send the next report after previous one is complete
void hid_task(void)
{
    // Poll every 10ms
    const uint32_t interval_ms = 10;
    static uint32_t start_ms = 0;

    if ( board_millis() - start_ms < interval_ms) return; // not enough time
    start_ms += interval_ms;

    uint32_t const btn = board_button_read();

    // Remote wakeup
    if ( tud_suspended() && btn )
    {
        // Wake up host if we are in suspend mode
        // and REMOTE_WAKEUP feature is enabled by host
        tud_remote_wakeup();
    }else
    {
        // Send the 1st of report chain, the rest will be sent by tud_hid_report_complete_cb()
        send_hid_report(REPORT_ID_KEYBOARD, btn);
    }
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len)
{
    (void) instance;
    (void) len;

    uint8_t next_report_id = report[0] + 1u;

    if (next_report_id < REPORT_ID_COUNT)
    {
        send_hid_report(next_report_id, board_button_read());
    }
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
    // TODO not Implemented
    (void) instance;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;

    return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
    (void) instance;

    if (report_type == HID_REPORT_TYPE_OUTPUT)
    {
        // Set keyboard LED e.g Capslock, Numlock etc...
        if (report_id == REPORT_ID_KEYBOARD)
        {
            // bufsize should be (at least) 1
            if ( bufsize < 1 ) return;

            uint8_t const kbd_leds = buffer[0];

            if (kbd_leds & KEYBOARD_LED_CAPSLOCK)
            {
                // Capslock On: disable blink, turn led on
                // TODO
                // led_state = 0;
                // board_led_write(true);
            }else
            {
                // Caplocks Off: back to normal blink
                // TODO
                // board_led_write(false);
                // led_state = BLINK_MOUNTED;
            }
        }
    }
}


//--------------------------------------------------------------------+
// USB CDC
//--------------------------------------------------------------------+
void cdc_task(void)
{
  if ( tud_cdc_connected() )
  {
    // connected and there are data available
    if ( tud_cdc_available() )
    {
      uint8_t buf[64];

      uint32_t count = tud_cdc_read(buf, sizeof(buf));

      // echo back to both web serial and cdc
      // TODO
      // echo_all(buf, count);
    }
  }
}

// Invoked when cdc when line state changed e.g connected/disconnected
void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts)
{
  (void) itf;

  // connected
  if ( dtr && rts )
  {
    // print initial message when connected
    tud_cdc_write_str("\r\nTinyUSB WebUSB device example\r\n");
  }
}

// Invoked when CDC interface received data from host
void tud_cdc_rx_cb(uint8_t itf)
{
  (void) itf;
}
