#ifndef HID_H
#define HID_H

#include <stdint.h>

typedef struct __attribute__((packed)) {
    uint16_t buttons;
} hid_button_report_t;


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
    /* 16 bit Button Map */ \
    HID_USAGE_PAGE     ( HID_USAGE_PAGE_BUTTON                  ), \
    /* add 0x11 so we get the linux BTN_TRIGGER_HAPPY, caus there are 40 of */ \
    /* those, and only like 12 BTN_JOYSTICK */ \
    /* Skip BTN_TRIGGER_HAPPY13 cause it (or BTN_JOYSTICK + 13 which is ?) */ \
    /* to open the steam overlay which is annoying cause steam input */ \
    /* detects this as the home button because everything *must* be a */ \
    /* gamepad */ \
    HID_USAGE_MIN      ( 0x11                                      ), \
    HID_USAGE_MAX      ( 0x11 + 12                                     ), \
    HID_LOGICAL_MIN    ( 0                                      ), \
    HID_LOGICAL_MAX    ( 1                                      ), \
    HID_REPORT_COUNT   ( 12                                     ), \
    HID_REPORT_SIZE    ( 1                                      ), \
    HID_INPUT          ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ), \
    HID_USAGE_MIN      ( 0x11 + 13                                      ), \
    HID_USAGE_MAX      ( 0x11 + 16                                     ), \
    HID_REPORT_COUNT   ( 2                                     ), \
    HID_INPUT          ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ), \
    /* 16 bit output */ \
    HID_USAGE_PAGE_N   ( 0xFF42, 2                              ), \
    /*
     * HID_USAGE_(MIN|MAX) gets a warning from hidrdd:
     * > Warning: Undocumented usage
     * because this is a vendor page without definitions.
     * But without it, windows says (sic)
     * > This device cannot start. (Code 10)
     * > A non constant main item was declaired without a corresponding usage.
     */ \
    HID_USAGE_MIN      ( 1                                      ), \
    HID_USAGE_MAX      ( sizeof(hid_incoming_data_t)    ), \
    HID_LOGICAL_MIN    ( 0                                      ), \
    HID_LOGICAL_MAX    ( 255                                    ), \
    HID_REPORT_COUNT   ( sizeof(hid_incoming_data_t)    ), \
    HID_REPORT_SIZE    ( 8                                      ), \
    HID_OUTPUT         ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ), \
  HID_COLLECTION_END

// clang-format on


#endif // HIDREPORT_H
