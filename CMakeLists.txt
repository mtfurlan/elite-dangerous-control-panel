cmake_minimum_required(VERSION 3.13)

# initialize the SDK directly
include(../pico-sdk/pico_sdk_init.cmake)

project(ed_control_panel)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

pico_sdk_init()

add_executable(ed_controller)

target_sources(ed_controller PUBLIC
        src/main.cpp
        src/led.cpp
        src/buttons.cpp
        src/usb_descriptors.c
        )

# Make sure TinyUSB can find tusb_config.h
target_include_directories(ed_controller PUBLIC
        src
)

# In addition to pico_stdlib required for common PicoSDK functionality, add dependency on tinyusb_device
# for TinyUSB device support and tinyusb_board for the additional board support library used by the example
target_link_libraries(ed_controller PUBLIC pico_stdlib pico_unique_id tinyusb_device tinyusb_board)

# Uncomment this line to enable fix for Errata RP2040-E5 (the fix requires use of GPIO 15)
target_compile_definitions(ed_controller PUBLIC PICO_RP2040_USB_DEVICE_ENUMERATION_FIX=1)

pico_enable_stdio_usb(ed_controller 1)
pico_add_extra_outputs(ed_controller)

# Enable the reset interface
target_compile_definitions(ed_controller PUBLIC
        PICO_STDIO_USB_ENABLE_RESET_VIA_VENDOR_INTERFACE=1
        # use SDK MS_OS_20 descriptor, as this doesn't have a custom one
        PICO_STDIO_USB_RESET_INTERFACE_SUPPORT_MS_OS_20_DESCRIPTOR=1
        # pass the interface number to the SDK MS_OS_20 descriptor
        PICO_STDIO_USB_RESET_INTERFACE_MS_OS_20_DESCRIPTOR_ITF=3
        )



add_subdirectory(vendor/adamboardman-pico-interfaces)
add_subdirectory(vendor/adamboardman-pico-mcp23017)

target_link_libraries(ed_controller PRIVATE pico_mcp23017)
