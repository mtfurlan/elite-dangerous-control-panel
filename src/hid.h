#ifndef HID_H
#define HID_H

#include <stdint.h>


#define MY_REPORT_DESC_GAMEPAD_BUTTONS(...) \
  HID_USAGE_PAGE ( HID_USAGE_PAGE_DESKTOP     )                 ,\
  HID_USAGE      ( HID_USAGE_DESKTOP_JOYSTICK )                 ,\
  HID_COLLECTION ( HID_COLLECTION_APPLICATION )                 ,\
    /* Report ID if any */\
    __VA_ARGS__ \
    HID_USAGE      ( HID_USAGE_DESKTOP_POINTER )                   ,\
    /* 16 bit Button Map */ \
    HID_USAGE_PAGE     ( HID_USAGE_PAGE_BUTTON                  ) ,\
    HID_USAGE_MIN      ( 1                                      ) ,\
    HID_USAGE_MAX      ( 16                                     ) ,\
    HID_LOGICAL_MIN    ( 0                                      ) ,\
    HID_LOGICAL_MAX    ( 1                                      ) ,\
    HID_REPORT_COUNT   ( 16                                     ) ,\
    HID_REPORT_SIZE    ( 1                                      ) ,\
    HID_INPUT          ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ) ,\
    /* 16 bit output */ \
    HID_USAGE_PAGE_N  ( 0xFF42, 2                   )       ,\
    /* HID_USAGE is reported as Warning: Undocumented usage by hidrdd */ \
    /* but without it, windows says (sic)*/ \
    /* This device cannot start. (Code 10) */ \
    /* A non constant main item was declaired without a corresponding usage. */ \
    HID_USAGE_MIN      ( 1                                      ) ,\
    HID_USAGE_MAX      ( 16                                     ) ,\
    HID_LOGICAL_MIN    ( 0                                      ) ,\
    HID_LOGICAL_MAX    ( 1                                      ) ,\
    HID_REPORT_COUNT   ( 16                                     ) ,\
    HID_REPORT_SIZE    ( 1                                      ) ,\
    HID_OUTPUT       ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE  ) ,\
  HID_COLLECTION_END \


typedef struct __attribute__ ((packed))
{
    uint16_t buttons;
} my_hid_report_gamepad_buttons_t;

typedef struct __attribute__ ((packed))
{
    uint16_t leds;
} my_hid_report_output_data_t;



#endif // HIDREPORT_H
