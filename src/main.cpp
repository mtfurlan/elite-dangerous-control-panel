#include <stdlib.h>

#include <bsp/board_api.h>
#include <hardware/i2c.h>
#include <pico/stdio.h>
#include <tusb.h>

#include <mcp23017.h>

#include "buttons.h"
#include "config_button.h"
#include "config_quadrature.h"
#include "config_smart.h"
#include "hid.h"
#include "led.h"
#include "usb_descriptors.h"


#define MCP_INPUT         0x20
#define MCP_OUTPUT        0x27
#define MCP_INPUT_IRQ_PIN 6


#define I2C_GPIO_PIN_SDA 4
#define I2C_GPIO_PIN_SCL 5


void hid_task(bool dirty, uint16_t* inputs);
static void send_hid_report(uint8_t report_id, uint16_t btn);


static Config* config[] = {
    new ConfigSmart(1,
                    1,
                    1,
                    [](hid_incoming_data_t* data) -> bool {
                        return data->Flags.fields.Landing_Gear_Down;
                    }),
    new ConfigSmart(2,
                    2,
                    2,
                    [](hid_incoming_data_t* data) -> bool {
                        return data->Flags.fields.LightsOn;
                    }),
    new ConfigSmart(3,
                    3,
                    3,
                    [](hid_incoming_data_t* data) -> bool {
                        return data->Flags.fields.Night_Vision;
                    }),
    new ConfigSmart(4,
                    4,
                    4,
                    [](hid_incoming_data_t* data) -> bool {
                        return data->Flags.fields.Hardpoints_Deployed;
                    }),
    new ConfigSmart(5,
                    5,
                    5,
                    [](hid_incoming_data_t* data) -> bool {
                        return data->Flags.fields.Cargo_Scoop_Deployed;
                    }),
    new ConfigButton(6, 6),
    new ConfigButton(7, 7),
    new ConfigButton(8, 8),
    new ConfigButton(9, 9),
    new ConfigButton(10, 10),
    new ConfigButton(11, 11),
    new ConfigButton(12, 12),
    new ConfigButton(13, 13),
    new ConfigQuadrature(14, 15, 14, 15),
    new ConfigButton(16, 16),
};

static led_state_t led_state = BLINK_NOT_MOUNTED;
static hid_incoming_data_t hid_incoming_data;


int main(void)
{
    // Initialize TinyUSB stack
    board_init();
    tusb_init();


    i2c_init(i2c0, 400000);
    gpio_set_function(I2C_GPIO_PIN_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_GPIO_PIN_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_GPIO_PIN_SDA);
    gpio_pull_up(I2C_GPIO_PIN_SCL);

    int err = 0;
    err |= buttons_init(MCP_INPUT_IRQ_PIN, i2c0, MCP_INPUT);
    err |= led_init(i2c0, MCP_OUTPUT);

    // TinyUSB board init callback after init
    if (board_init_after_tusb) {
        board_init_after_tusb();
    }

    // let pico sdk use the first cdc interface for std io
    stdio_init_all();

    tud_task();

    for (size_t i = 0; i < TU_ARRAY_SIZE(config); ++i) {
        Config* c = config[i];
        if (!c->checkConfig()) {
            sleep_ms(1000);
            printf("config %d bad\n", i);
            err |= 1;
        }
    }

    if (err) {
        printf("failed to init: %d\n", err);
        while (true) {
            tud_task();
            led_error();
        }
    }

    printf("hello running");
    uint16_t button_data = 0;
    uint16_t output;
    uint16_t led_data;

    // main run loop
    while (1) {
        // TinyUSB device task | must be called regurlarly
        tud_task();

        bool dirty = buttons_task(&button_data);

        // input data: hid_incoming_data.leds
        // switch state: button_data
        for (size_t i = 0; i < TU_ARRAY_SIZE(config); ++i) {
            Config* c = config[i];
            dirty |= c->generateOutput(&output, button_data, &hid_incoming_data);
        }


        hid_task(dirty, &output);

        led_data = 0;
        for (size_t i = 0; i < TU_ARRAY_SIZE(config); ++i) {
            Config* c = config[i];
            c->setLED(&led_data, &hid_incoming_data);
        }

        led_task(led_state, led_data);
    }

    // indicate no error
    return 0;
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
    (void)remote_wakeup_en;
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
// Every 10ms, we will sent 1 report for each HID profile (keyboard, mouse etc ..)
// tud_hid_report_complete_cb() is used to send the next report after previous one is complete
static uint16_t btn;
void hid_task(bool dirty, uint16_t* inputs)
{
    btn = *inputs;
    // Poll every 10ms
    const uint32_t interval_ms = 10;
    static uint32_t start_ms = 0;

    if (!dirty && (board_millis() - start_ms < interval_ms)) {
        return; // not enough time
    }
    start_ms += interval_ms;

    // Remote wakeup
    if (tud_suspended() && btn) {
        // Wake up host if we are in suspend mode
        // and REMOTE_WAKEUP feature is enabled by host
        tud_remote_wakeup();
    } else {
        // Send the 1st of report chain, the rest will be sent by tud_hid_report_complete_cb()
        send_hid_report(REPORT_ID_GAMEPAD, btn);
    }
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len)
{
    (void)instance;
    (void)len;

    uint8_t next_report_id = report[0] + 1u;

    if (next_report_id < REPORT_ID_COUNT) {
        send_hid_report(next_report_id, btn);
    }
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance,
                               uint8_t report_id,
                               hid_report_type_t report_type,
                               uint8_t* buffer,
                               uint16_t reqlen)
{
    // TODO not Implemented
    (void)instance;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)reqlen;

    return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance,
                           uint8_t report_id,
                           hid_report_type_t report_type,
                           uint8_t const* buffer,
                           uint16_t bufsize)
{
    (void)instance;

    if (report_type == HID_REPORT_TYPE_OUTPUT) {
        // Set keyboard LED e.g Capslock, Numlock etc...
        if (report_id == REPORT_ID_GAMEPAD) {
            // bufsize should be (at least) 1
            if (bufsize != sizeof(hid_incoming_data_t)) {
                printf("got a weird size data from hid, reporrt id %d, expected %d bytes, got %d\n",
                       report_id,
                       sizeof(hid_incoming_data_t),
                       bufsize);
                return;
            }

            hid_incoming_data = *(hid_incoming_data_t*)buffer;

            printf("got data\n");
        } else {
            printf("got unexpected output repoort for id %d", report_id);
        }
    }
}

static void send_hid_report(uint8_t report_id, uint16_t btn)
{
    // skip if hid is not ready yet
    if (!tud_hid_ready())
        return;

    switch (report_id) {
        case REPORT_ID_GAMEPAD: {
            // use to avoid send multiple consecutive zero report for keyboard
            static bool has_gamepad_key = false;

            hid_button_report_t report = {
                .buttons = btn,
            };

            if (btn) {
                tud_hid_report(REPORT_ID_GAMEPAD, &report, sizeof(report));

                has_gamepad_key = true;
            } else {
                if (has_gamepad_key)
                    tud_hid_report(REPORT_ID_GAMEPAD, &report, sizeof(report));
                has_gamepad_key = false;
            }
        } break;

        default:
            break;
    }
}

//--------------------------------------------------------------------+
// USB CDC
//--------------------------------------------------------------------+

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
        tud_cdc_n_write(itf, (uint8_t const*)"OK\r\n", 4);
        tud_cdc_n_write_flush(itf);
    }
}
