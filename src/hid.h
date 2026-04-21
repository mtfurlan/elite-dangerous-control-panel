#ifndef HID_H
#define HID_H
/**
 * this defines the structs used to send/recieve data over the usb descriptor
 * it is included by usb/usb_descriptors.c
 */

#include <stdint.h>

#define USB_MANUFACTURER_STR "sczie"
#define USB_PRODUCT_STR "elite dangerous control panel"

// set some example Vendor and Product ID
// the board will use to identify at the host
#define _PID_MAP(itf, n) ((CFG_TUD_##itf) << (n))
#define VID              0xCafe
// use _PID_MAP to generate unique PID for each interface
#define PID (0x4000 | _PID_MAP(CDC, 0) | _PID_MAP(HID, 1))


#define USB_VENDOR_USAGE_PAGE_FOR_INPUT 0xFF42

/**
 * data sent to computer as gamepad buttons
 * see the USB definition below for HID_USAGE_PAGE_BUTTON
 */
typedef struct __attribute__((packed)) {
    uint16_t buttons;
} hid_button_report_t;
#define BUTTON_COUNT 16


typedef enum {
    Unknown = -1,
    Clean = 0,
    IllegalCargo = 1,
    Speeding = 2,
    Wanted = 3,
    Hostile = 4,
    PassengerWanted = 5,
    Warrant = 6,
    Allied = 7,
    Thargoid = 8,
} LegalState_e;

typedef struct __attribute__((packed)) {
    union {
        uint32_t raw;
        struct {
            bool Docked                       : 1;
            bool Landed                       : 1;
            bool Landing_Gear_Down            : 1;
            bool Shields_Up                   : 1;
            bool Supercruise                  : 1;
            bool FlightAssist_Off             : 1;
            bool Hardpoints_Deployed          : 1;
            bool In_Wing                      : 1;
            bool LightsOn                     : 1;
            bool Cargo_Scoop_Deployed         : 1;
            bool Silent_Running               : 1;
            bool Scooping_Fuel                : 1;
            bool Srv_Handbrake                : 1;
            bool Srv_using_Turret_view        : 1;
            bool Srv_Turret_retracted         : 1;
            bool Srv_DriveAssist              : 1;
            bool Fsd_MassLocked               : 1;
            bool Fsd_Charging                 : 1;
            bool Fsd_Cooldown                 : 1;
            bool Low_Fuel                     : 1;
            bool Over_Heating                 : 1;
            bool Has_Lat_Long                 : 1;
            bool IsInDanger                   : 1;
            bool Being_Interdicted            : 1;
            bool In_MainShip                  : 1;
            bool In_Fighter                   : 1;
            bool In_SRV                       : 1;
            bool Hud_in_Analysis_mode         : 1;
            bool Night_Vision                 : 1;
            bool Altitude_from_Average_radius : 1;
            bool fsdJump                      : 1;
            bool srvHighBeam                  : 1;
        } fields;
    } Flags;
    union {
        uint32_t raw : 20;
        struct {
            bool OnFoot                  : 1;
            bool InTaxi                  : 1;
            bool InMulticrew             : 1;
            bool OnFootInStation         : 1;
            bool OnFootOnPlanet          : 1;
            bool AimDownSight            : 1;
            bool LowOxygen               : 1;
            bool LowHealth               : 1;
            bool Cold                    : 1;
            bool Hot                     : 1;
            bool VeryCold                : 1;
            bool VeryHot                 : 1;
            bool Glide_Mode              : 1;
            bool OnFootInHangar          : 1;
            bool OnFootSocialSpace       : 1;
            bool OnFootExterior          : 1;
            bool BreathableAtmosphere    : 1;
            bool Telepresence_Multicrew  : 1;
            bool Physical_Multicrew      : 1;
            bool Fsd_hyperdrive_charging : 1;
        } fields;
    } Flags2;
    int8_t PipsSys;
    int8_t PipsEng;
    int8_t PipsWep;
    int8_t FireGroup;
    LegalState_e LegalState : 8;
} hid_incoming_data_t;

// clang-format off
#define MY_REPORT_DESC_GAMEPAD_BUTTONS(...) \
  HID_USAGE_PAGE ( HID_USAGE_PAGE_DESKTOP                       ), \
  HID_USAGE      ( HID_USAGE_DESKTOP_JOYSTICK                   ), \
  HID_COLLECTION ( HID_COLLECTION_APPLICATION                   ), \
    /* Report ID if any */\
    __VA_ARGS__ \
    \
    /* BUTTON_COUNT bit Button Map
     *
     * 0x0 to 0xf are BTN_JOYSTICK or BTN_GAMEPAD which have hardcoded meanings
     * we don't want to deal with
     * https://github.com/torvalds/linux/blob/d46dd0d88341e45f8e0226fdef5462f5270898fc/include/uapi/linux/input-event-codes.h#L366-L400
     * so start at 0x11 so we get the linux BTN_TRIGGER_HAPPY
     * TODO: why skip 0x10 -> BTN_TRIGGER_HAPPY for of 0x11 ->BTN_TRIGGER_HAPPY1
     * TODO: test that
     */ \
    HID_USAGE_PAGE     ( HID_USAGE_PAGE_BUTTON                  ), \
    HID_REPORT_SIZE    ( 1                                      ), \
    HID_LOGICAL_MIN    ( 0                                      ), \
    HID_LOGICAL_MAX    ( 1                                      ), \
    HID_USAGE_MIN      ( 0x11                                   ), \
    HID_USAGE_MAX      ( 0x11 + BUTTON_COUNT                    ), \
    HID_REPORT_COUNT   ( BUTTON_COUNT - 1                       ), \
    HID_INPUT          ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ), \
    \
    /*
     * HID_USAGE_(MIN|MAX) gets a warning from hidrdd:
     * > Warning: Undocumented usage
     * because this is a vendor page without definitions.
     * But without it, windows says (sic)
     * > This device cannot start. (Code 10)
     * > A non constant main item was declaired without a corresponding usage.
     */ \
    HID_USAGE_PAGE_N   ( USB_VENDOR_USAGE_PAGE_FOR_INPUT, 2     ), \
    HID_USAGE_MIN      ( 1                                      ), \
    HID_USAGE_MAX      ( sizeof(hid_incoming_data_t)            ), \
    HID_LOGICAL_MIN    ( 0                                      ), \
    HID_LOGICAL_MAX    ( 255                                    ), \
    HID_REPORT_COUNT   ( sizeof(hid_incoming_data_t)            ), \
    HID_REPORT_SIZE    ( 8                                      ), \
    HID_OUTPUT         ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ), \
  HID_COLLECTION_END

// clang-format on


#endif // HIDREPORT_H
