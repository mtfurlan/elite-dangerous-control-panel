/**
 * Copyright (c) 2024 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdlib.h>
#include <bsp/board_api.h>
#include <tusb.h>

#include <pico/stdio.h>

#include "usb_descriptors.h"

void custom_cdc_task(void);
void hid_task(void);
static void send_hid_report(uint8_t report_id, uint32_t btn);

int main(void)
{
    // Initialize TinyUSB stack
    board_init();
    tusb_init();

    // TinyUSB board init callback after init
    if (board_init_after_tusb) {
        board_init_after_tusb();
    }

    // let pico sdk use the first cdc interface for std io
    stdio_init_all();

    // main run loop
    while (1) {
        // TinyUSB device task | must be called regurlarly
        tud_task();

        // custom tasks
        custom_cdc_task();
        //hid_task();
    }

    // indicate no error
    return 0;
}
//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+
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
                //int8_t const delta = 5;

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

//--------------------------------------------------------------------+
// USB CDC
//--------------------------------------------------------------------+
void custom_cdc_task(void)
{
    // polling CDC interfaces if wanted

    // Check if CDC interface 0 (for pico sdk stdio) is connected and ready

    if (tud_cdc_n_connected(0)) {
        // print on CDC 0 some debug message
        printf("Connected to CDC 0\n");
        sleep_ms(5000); // wait for 5 seconds
    }
}

// callback when data is received on a CDC interface
void tud_cdc_rx_cb(uint8_t itf)
{
    // allocate buffer for the data in the stack
    uint8_t buf[CFG_TUD_CDC_RX_BUFSIZE];

    printf("RX CDC %d\n", itf);

    // read the available data
    // | IMPORTANT: also do this for CDC0 because otherwise
    // | you won't be able to print anymore to CDC0
    // | next time this function is called
    uint32_t count = tud_cdc_n_read(itf, buf, sizeof(buf));

    // check if the data was received on the second cdc interface
    if (itf == 1) {
        // process the received data
        buf[count] = 0; // null-terminate the string
        // now echo data back to the console on CDC 0
        printf("Received on CDC 1: %s\n", buf);

        // and echo back OK on CDC 1
        tud_cdc_n_write(itf, (uint8_t const *) "OK\r\n", 4);
        tud_cdc_n_write_flush(itf);
    }
}

