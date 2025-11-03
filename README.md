# RP2040 TinyUSB HID elite dangerous controller
A control panel for Elite Dangerous.

It takes input from the game's
[Status.json](https://elite-journal.readthedocs.io/en/latest/Status%20File.html)
and sends it over HID to the control panel.

That way we can have switches that act like buttons, as well as LEDs.


## HID notes
I had some Issues getting tinyusb working, I tried adafruit's port under platformio and too many things there were annoying and broken.
So eventaully I just ended up using the rp2040 pico-sdk and tinyusb directly.

`.lvimrc` to configure [`vim-ale`](https://github.com/dense-analysis/ale) cause it can't fully get stuff from `compile_commands.json` [yet](https://github.com/dense-analysis/ale/issues/3328)

### USB HID
* [Introduction to HID report descriptors](https://docs.kernel.org/hid/hidintro.html)
* spec
  * [HID 1.11 spec](https://www.usb.org/sites/default/files/hid1_11.pdf)
  * [HID Usage Tables](https://usb.org/sites/default/files/hut1_6.pdf): has usage pages and usage tags and other stuff

* input: data from device -> host like keyboard or joystick
* output: data from host -> device, like keyboard LEDs
* feature report: other stuff
  > "Feature"s are not meant to be consumed by the end user and define configuration options for the device.


#### linux support of HID
Unfortunately, linux doesn't support stuff like weird simulation buttons, so
the only real options are gamepad buttons or keyboard buttons.

[Look at how little of sim is covered](https://github.com/torvalds/linux/blob/master/drivers/hid/hid-input.c#L797-L806).


#### decoding descriptor reports
* [hidrdd](https://github.com/abend0c1/hidrdd)
  * `alias hidrdd="rexx ~/src/hidrdd/rd.rex -i ~/src/hidrdd/rd.conf --no-struct"`
  * `hidrdd -bf /sys/bus/hid/devices/0003\:CAFE\:4003.0060/report_descriptor`
* [online USB Descriptor and Request Parser](http://eleccelerator.com/usbdescreqparser/)
  * `xxd -p /sys/bus/hid/devices/0003\:CAFE\:4003.0060/report_descriptor | tr -d '\n' | copyToClipboard`



#### host side hid tools
```
hidapitester --vidpid CAFE:4003 --open --length 3 --send-output 1,0x00,0x0
```

python [hid](https://pypi.org/project/hid/) wraps [libusb hidapi](https://libusb.info/hidapi/group__API.html)
```
import hid
with hid.Device(vid, pid) as h:
    h.write(bytes([0x01, 0x42, 0, 0, 0]))
```

udev:
```
SUBSYSTEMS=="usb", ATTRS{idVendor}=="cafe", MODE:="0666"
```

### Fun facts about windows
windows will just disregard hid devices with vendor pages without usage min/max, even though it's a vendor page

In actual windows, you can see a (misspelled) error in the device manager.

In wine, I'm not sure how to get any error message.
