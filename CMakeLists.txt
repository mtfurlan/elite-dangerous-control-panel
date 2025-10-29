cmake_minimum_required(VERSION 3.13)

# initialize the SDK directly
include(../pico-sdk/pico_sdk_init.cmake)

project(thingy)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

pico_sdk_init()

add_executable(projectThing)

target_sources(projectThing PUBLIC
        src/main.cpp
        src/led.c
        src/inputs.c
        src/usb_descriptors.c
        )

# Make sure TinyUSB can find tusb_config.h
target_include_directories(projectThing PUBLIC
        src
)

# In addition to pico_stdlib required for common PicoSDK functionality, add dependency on tinyusb_device
# for TinyUSB device support and tinyusb_board for the additional board support library used by the example
target_link_libraries(projectThing PUBLIC pico_stdlib pico_unique_id tinyusb_device tinyusb_board)

# Uncomment this line to enable fix for Errata RP2040-E5 (the fix requires use of GPIO 15)
target_compile_definitions(projectThing PUBLIC PICO_RP2040_USB_DEVICE_ENUMERATION_FIX=1)

pico_enable_stdio_usb(projectThing 1)
pico_add_extra_outputs(projectThing)
