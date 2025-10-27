#!/usr/bin/env -S pipx run
# /// script
# dependencies = [
#   "hid",
# ]
# ///

import hid



vid = 0xcafe
pid = 0x4003

with hid.Device(vid, pid) as h:
    print(f'Device manufacturer: {h.manufacturer}')
    print(f'Product: {h.product}')
    print(f'Serial Number: {h.serial}')
    h.write(bytes([0x01, 0x42, 0, 0, 0]))

    #h.send_feature_report(bytes([0x00, 0x42, 0x55, 0x32]))
