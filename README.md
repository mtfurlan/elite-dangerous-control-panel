# RP2040 USB TinyUSB example project

I had some Issues getting tinyusb working, I tried adafruit's port under platformio and too many things there were annoying and broken.
So this is a demonstrator for just using rp2040 pico-sdk and tinyusb directly

`.lvimrc` to configure [`vim-ale`](https://github.com/dense-analysis/ale) cause it can't fully get stuff from `compile_commands.json` yet
`debug.sh` needs a bit better interface, but I don't remember how to cmake good right now

## USB HID
* [Introduction to HID report descriptors](https://docs.kernel.org/hid/hidintro.html)
* spec
  * [HID 1.11 spec](https://www.usb.org/sites/default/files/hid1_11.pdf)
  * [HID Usage Tables](https://usb.org/sites/default/files/hut1_6.pdf): has usage pages and usage tags and other stuff

input: data from device -> host like keyboard or joystick
output: data from host -> device, like keyboard LEDs
feature report: other stuff
> "Feature"s are not meant to be consumed by the end user and define configuration options for the device.


* linux doesn't support stuff like weird simulation buttons, so the only real options are gamepad buttons or keyboard buttons
  * [look at how little of sim is covered](https://github.com/torvalds/linux/blob/master/drivers/hid/hid-input.c#L797-L806)
  * joystick buttons are probbaly the best in terms of elite dangerous mapping menu?


#### decoding
links from linux kernel
* [hidrdd](https://github.com/abend0c1/hidrdd)
  * `alias hidrdd="rexx ~/src/hidrdd/rd.rex -i ~/src/hidrdd/rd.conf --no-struct"`
  * `hidrdd -bf /sys/bus/hid/devices/0003\:CAFE\:4003.0060/report_descriptor`
* [online USB Descriptor and Request Parser](http://eleccelerator.com/usbdescreqparser/)
  * `xxd -p /sys/bus/hid/devices/0003\:CAFE\:4003.0060/report_descriptor | tr -d '\n' | copyToClipboard`



#### host side hid tools
* python lib: https://pypi.org/project/hid/
  * wraps https://libusb.info/hidapi/group__API.html
```
import hid
with hid.Device(vid, pid) as h:
    h.write(bytes([0x01, 0x42, 0, 0, 0]))
```


udev:
```
SUBSYSTEMS=="usb", ATTRS{idVendor}=="cafe", MODE:="0666"

```
hidapitester --vidpid CAFE:4003   --open --length 5 --send-output 1,0x15,0,0,0
