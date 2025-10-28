#!/usr/bin/env -S pipx run
# /// script
# dependencies = [
#   "hid",
# ]
# ///

# watchexec -w ~/projects/elite-dangerous/savegame/ -f Status.json ./sendData.py
#   "watchfiles",
import hid
import json
import os
#from watchfiles import watch

statusFile="~/projects/elite-dangerous/savegame/Status.json"
#statusFile="/tmp/status.json"

with open(os.path.expanduser(statusFile), 'r') as file:
    status = json.load(file)

#print(json.dumps(status, indent=4))

lights = bool(status['Flags'] & 1 << 8)
print(f"lights: {lights}")


vid = 0xcafe
pid = 0x4003
report_id = 1

with hid.Device(vid, pid) as h:
    #print(f'Device manufacturer: {h.manufacturer}')
    #print(f'Product: {h.product}')
    #print(f'Serial Number: {h.serial}')
    h.write(bytes([report_id, lights, 0, 0, 0]))

    #h.send_feature_report(bytes([0x00, 0x42, 0x55, 0x32]))
