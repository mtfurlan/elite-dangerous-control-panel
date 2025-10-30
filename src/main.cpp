#include <stdlib.h>

#include <bsp/board_api.h>
#include <tusb.h>
#include <pico/stdio.h>
#include <hardware/i2c.h>

#include <mcp23017.h>

#include "usb_descriptors.h"
#include "led.h"
#include "buttons.h"
#include "hid.h"


#define MCP_INPUT 0x20
#define MCP_OUTPUT 0x27
#define MCP_INPUT_IRQ_PIN 6


#define I2C_GPIO_PIN_SDA 4
#define I2C_GPIO_PIN_SCL 5



void hid_task(bool dirty, uint16_t* inputs);
static void send_hid_report(uint8_t report_id, uint32_t btn);

typedef enum {
    DIRECT, // output inputPin directly
    SMART_BTN, // do maths on input and output to decide what to do to pretend to be a toggle button
    //RISING, // send events on rising edge
    //FALLING, // send events on falling edge
    //OPTIMISTIC_SW, act as a switch and set LED based on that
} input_mode_t;

typedef struct {
    uint8_t input_bit; // 1-32, 0 is no input
    uint8_t output_bit; // joy btn 1-32, 0 no output
    int button_pin; // -1 is disabled // TODO: inputPin is tied to output number, seems confusing
    int led_pin; // -1 is disabled
                    // TODO: direction?
    input_mode_t mode;
} input_config_t;

static input_config_t config[] = {
  {
      .input_bit = 1,
      .output_bit = 1,
      .button_pin = 0,
      .led_pin = 15,
      .mode = DIRECT,
  }
};

static led_state_t led_state = BLINK_NOT_MOUNTED;
static my_hid_report_output_data_t hid_incoming_data;


#define GET_BUTTON_STATE(c, i) (i & (1 << c->button_pin))
#define SET_OUTPUT_BIT(c, val)  ((val & 0x1) << (c->output_bit - 1))
#define GET_LED_STATE(c, i) (i & (1 << c->led_pin))
#define GET_LED_INPUT(c, i) (i & (1 << (c->input_bit - 1)))
#define SET_LED_BIT(c, val)  ((val & 0x1) << (c->led_pin))
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

    buttons_init(MCP_INPUT_IRQ_PIN, i2c0, MCP_INPUT);

    led_init(i2c0, MCP_OUTPUT);

    // TinyUSB board init callback after init
    if (board_init_after_tusb) {
        board_init_after_tusb();
    }

    // let pico sdk use the first cdc interface for std io
    stdio_init_all();

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
        if(dirty) {
            output = 0;
            for(size_t i = 0; i < TU_ARRAY_SIZE(config); ++i) {
                input_config_t* c = &config[i];
                if(c->output_bit != 0 && c->button_pin >= 0) {
                    switch(c->mode) {
                        case DIRECT:
                            // just set output to button state
                            output |= SET_OUTPUT_BIT(c, GET_BUTTON_STATE(c, button_data));
                            break;
                        case SMART_BTN:
                            // if button xor led, trigger a little
                            if(GET_BUTTON_STATE(c, button_data) ^ GET_LED_STATE(c, hid_incoming_data.leds)) {
                                output |= SET_OUTPUT_BIT(c, 1); //TODO: trigger a duratin then turn off
                            }
                            break;
                    }
                }
            }
        }
        hid_task(dirty, &output);

        led_data = 0;
        for(size_t i = 0; i < TU_ARRAY_SIZE(config); ++i) {
            input_config_t* c = &config[i];
            if(c->input_bit != 0 && c->led_pin >= 0) {
                led_data |= SET_LED_BIT(c, GET_LED_INPUT(c, hid_incoming_data.leds));
                //printf("data %04X, config %d uses bit %d, got %d, set bit %d of led as %d, leds is %d\n",
                //        hid_incoming_data.leds,
                //        i,
                //        c->input_bit,
                //        GET_LED_INPUT(c, hid_incoming_data.leds),
                //        c->led_pin,
                //        SET_LED_BIT(c, GET_LED_INPUT(c, hid_incoming_data.leds)),
                //        led_data);
            }
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
// Every 10ms, we will sent 1 report for each HID profile (keyboard, mouse etc ..)
// tud_hid_report_complete_cb() is used to send the next report after previous one is complete
static uint8_t btn;
void hid_task(bool dirty, uint16_t* inputs)
{
    btn = *inputs;
    // Poll every 10ms
    const uint32_t interval_ms = 10;
    static uint32_t start_ms = 0;

    if (!dirty && ( board_millis() - start_ms < interval_ms)) {
        return; // not enough time
    }
    start_ms += interval_ms;

    // Remote wakeup
    if ( tud_suspended() && btn )
    {
        // Wake up host if we are in suspend mode
        // and REMOTE_WAKEUP feature is enabled by host
        tud_remote_wakeup();
    }else
    {
        // Send the 1st of report chain, the rest will be sent by tud_hid_report_complete_cb()
        send_hid_report(REPORT_ID_GAMEPAD, btn);
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
        send_hid_report(next_report_id, btn);
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

    if (report_type == HID_REPORT_TYPE_OUTPUT) {
        // Set keyboard LED e.g Capslock, Numlock etc...
        if (report_id == REPORT_ID_GAMEPAD) {
            // bufsize should be (at least) 1
            if ( bufsize != sizeof(my_hid_report_output_data_t)) {
                printf("got a weird size data from hid, reporrt id %d, size %d\n", report_id, bufsize);
                return;
            }

            hid_incoming_data = *(my_hid_report_output_data_t*)buffer;

            printf("got data %02X\n", (uint8_t)(hid_incoming_data.leds & 0xff));
        } else {
            printf("got unexpected output repoort for id %d", report_id);
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

                if ( false && btn )
                {
                    uint8_t keycode[6] = { 0 };
                    keycode[0] = HID_KEY_A;

                    tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keycode);
                    has_keyboard_key = true;
                }else {
                    // send empty key report if previously has key pressed
                    if (has_keyboard_key) tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
                    has_keyboard_key = false;
                }
            }
            break;

        case REPORT_ID_GAMEPAD:
            {
                // use to avoid send multiple consecutive zero report for keyboard
                static bool has_gamepad_key = false;

                my_hid_report_gamepad_buttons_t report = {
                //hid_gamepad_report_t report = {
                    .buttons = 0
                };

                if ( btn ) {
                    report.buttons = 0x1;
                    tud_hid_report(REPORT_ID_GAMEPAD, &report, sizeof(report));

                    has_gamepad_key = true;
                } else {
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
