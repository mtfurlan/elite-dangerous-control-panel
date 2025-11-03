#!/usr/bin/env -S pipx run
# /// script
# dependencies = [
#   "hid",
#   "watchfiles",
# ]
# ///


# watch an elite dangerous Status.json for changes
# when there is a chnage, parse it, transform it
# then send as a HID report to a specific vid/pid

import hid
import json
import os
import ctypes
import datetime
from enum import IntEnum
from watchfiles import watch, Change



statusFile="~/projects/elite-dangerous/savegame/Status.json"
#statusFile="/tmp/edtest/Status.json"

vid = 0xcafe
pid = 0x4003
report_id = 1

# https://elite-journal.readthedocs.io/en/latest/Status%20File.html
u8 = ctypes.c_uint8
class Flags_Bitfield(ctypes.LittleEndianStructure):
    _pack_ = 1  # prevent alignment padding
    _fields_ = [
        ("Docked", u8, 1),
        ("Landed", u8, 1),
        ("Landing_Gear_Down", u8, 1),
        ("Shields_Up", u8, 1),
        ("Supercruise", u8, 1),
        ("FlightAssist_Off", u8, 1),
        ("Hardpoints_Deployed", u8, 1),
        ("In_Wing", u8, 1),
        ("LightsOn", u8, 1),
        ("Cargo_Scoop_Deployed", u8, 1),
        ("Silent_Running", u8, 1),
        ("Scooping_Fuel", u8, 1),
        ("Srv_Handbrake", u8, 1),
        ("Srv_using_Turret_view", u8, 1),
        ("Srv_Turret_retracted", u8, 1),
        ("Srv_DriveAssist", u8, 1),
        ("Fsd_MassLocked", u8, 1),
        ("Fsd_Charging", u8, 1),
        ("Fsd_Cooldown", u8, 1),
        ("Low_Fuel", u8, 1),
        ("Over_Heating", u8, 1),
        ("Has_Lat_Long", u8, 1),
        ("IsInDanger", u8, 1),
        ("Being_Interdicted", u8, 1),
        ("In_MainShip", u8, 1),
        ("In_Fighter", u8, 1),
        ("In_SRV", u8, 1),
        ("Hud_in_Analysis_mode", u8, 1),
        ("Night_Vision", u8, 1),
        ("Altitude_from_Average_radius", u8, 1),
        ("fsdJump", u8, 1),
        ("srvHighBeam", u8, 1),
    ]
class Flags2_Bitfield(ctypes.LittleEndianStructure):
    _pack_ = 1  # prevent alignment padding
    _fields_ = [
        ("OnFoot", u8, 1),
        ("InTaxi", u8, 1),
        ("InMulticrew", u8, 1),
        ("OnFootInStation", u8, 1),
        ("OnFootOnPlanet", u8, 1),
        ("AimDownSight", u8, 1),
        ("LowOxygen", u8, 1),
        ("LowHealth", u8, 1),
        ("Cold", u8, 1),
        ("Hot", u8, 1),
        ("VeryCold", u8, 1),
        ("VeryHot", u8, 1),
        ("Glide_Mode", u8, 1),
        ("OnFootInHangar", u8, 1),
        ("OnFootSocialSpace", u8, 1),
        ("OnFootExterior", u8, 1),
        ("BreathableAtmosphere", u8, 1),
        ("Telepresence_Multicrew", u8, 1),
        ("Physical_Multicrew", u8, 1),
        ("Fsd_hyperdrive_charging", u8, 1),
    ]

class Flags(ctypes.LittleEndianUnion):
    _pack_ = 1  # prevent alignment padding
    _fields_ = [
        ("fields", Flags_Bitfield),
        ("raw", ctypes.c_uint32)
    ]
class Flags2(ctypes.LittleEndianUnion):
    _pack_ = 1  # prevent alignment padding
    _fields_ = [
        ("fields", Flags2_Bitfield),
        ("raw", ctypes.c_uint32, 20)
    ]

class GuiFocus(IntEnum):
    NoFocus = 0,
    InternalPanel = 1, # right hand side
    ExternalPanel = 2, # left hand side
    CommsPanel = 3, # top
    RolePanel = 4, # bottom
    StationServices = 5,
    GalaxyMap = 6,
    SystemMap = 7,
    Orrery = 8,
    FSS_mode = 9,
    SAA_mode = 10,
    Codex = 11,

# I made this enum up, Status.json has the text
class LegalState(IntEnum):
    Unknown = -1
    Clean = 0
    IllegalCargo = 1
    Speeding = 2
    Wanted = 3
    Hostile = 4
    PassengerWanted = 5
    Warrant = 6
    Allied = 7
    Thargoid = 8


# TODO validate against usb descriptor
class HIDReport(ctypes.LittleEndianStructure):
    _pack_ = 1  # prevent alignment padding
    _fields_ = [
        ("Flags", Flags),
        ("Flags2", Flags2),
        ("PipsSys", ctypes.c_int8), # -1 for null, 0-8
        ("PipsEng", ctypes.c_int8),
        ("PipsWep", ctypes.c_int8),
        ("FireGroup", ctypes.c_int8), # -1 for unknown, A-H are 0-8
        ("LegalState", ctypes.c_int8), # -1 for unknown, 0-8
    ]

# take in a data struct, very similar to the Status.json, but with additional fields
# Flags_fields: {  Docked: bool, Landed: bool ... }
# Flags2_fields: {  OnFoot: bool, InTaxi: bool ... }
# GuiFocus_desc: string
# return a byte array to send over HID
def transform(data: dict) -> HIDReport:
    report = HIDReport()
    report.Flags.raw = data['Flags'];
    report.Flags2.raw = data['Flags2'] if 'Flags2' in data else 0;
    report.PipSys = data['Pips'][0] if 'Pips' in data else -1;
    report.PipEng = data['Pips'][1] if 'Pips' in data else -1;
    report.PipWep = data['Pips'][2] if 'Pips' in data else -1;
    report.FireGroup = data['FireGroup'] if 'FireGroup' in data else -1;
    report.LegalState = int(LegalState[data['LegalState']] if 'LegalState' in data else LegalState["Unknown"]);
    return report


# https://stackoverflow.com/a/34301571
def getdict(struct):
    result = {}
    #print struct
    def get_value(value):
         if (type(value) not in [int, float, bool]) and not bool(value):
             # it's a null pointer
             value = None
         elif hasattr(value, "_length_") and hasattr(value, "_type_"):
             # Probably an array
             #print value
             value = get_array(value)
         elif hasattr(value, "_fields_"):
             # Probably another struct
             value = getdict(value)
         return value
    def get_array(array):
        ar = []
        for value in array:
            value = get_value(value)
            ar.append(value)
        return ar
    for f  in struct._fields_:
         field = f[0]
         value = getattr(struct, field)
         # if the type is not a primitive and it evaluates to False ...
         value = get_value(value)
         result[field] = value
    return result


def parseJSON(status: dict) -> dict:
    flags = Flags()
    flags.raw = status['Flags'];
    status['Flags_fields'] = getdict(flags.fields)
    try:
        flags2 = Flags2()
        flags2.raw = status['Flags2'];
        status['Flags2_fields'] = getdict(flags2.fields)
    except:
        pass;

    try:
        status['GuiFocus_desc'] = GuiFocus[status['GuiFocus']]
    except:
        pass;
    # datetime.datetime.fromisoformat(status['timestamp'])
    return status;

def parseFile(filename: str) -> dict:
        with open(os.path.expanduser(statusFile), 'r') as file:
            try:
                status = json.load(file)
            except:
                print("failed to parse json")
                print(file)
                pass
            return parseJSON(status)


def sendReport(h, statusFile):
    data = parseFile(statusFile)
    hidreport = transform(data);
    h.write(bytes([report_id]) + bytes(hidreport))

with hid.Device(vid, pid) as h:
    sendReport(h, statusFile);
    for changes in watch(os.path.dirname(os.path.expanduser(statusFile))):
        for change in changes:
            if change[0] not in [Change.added, Change.modified]:
                continue;
            filename=change[1]
            if os.path.basename(filename) != "Status.json":
                continue;

            sendReport(h, filename);
