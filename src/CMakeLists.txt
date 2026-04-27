set(BINARY ed_controller)
add_executable(${BINARY})

target_sources(${BINARY} PUBLIC
    main.cpp
    led.cpp
    mcp.cpp
    config.cpp
    config_button.cpp
)
# Make sure TinyUSB can find hid.h
target_include_directories(${BINARY} PUBLIC
    .
)
add_subdirectory(usb)

# In addition to pico_stdlib required for common PicoSDK functionality, add dependency on tinyusb_device
# for TinyUSB device support and tinyusb_board for the additional board support library used by the example
target_link_libraries(${BINARY} PUBLIC pico_stdlib pico_unique_id tinyusb_device tinyusb_board)

# Uncomment this line to enable fix for Errata RP2040-E5 (the fix requires use of GPIO 15)
target_compile_definitions(${BINARY} PUBLIC PICO_RP2040_USB_DEVICE_ENUMERATION_FIX=1)

pico_enable_stdio_usb(${BINARY} 1)
pico_add_extra_outputs(${BINARY})

# Enable the reset interface
target_compile_definitions(${BINARY} PUBLIC
    PICO_STDIO_USB_ENABLE_RESET_VIA_VENDOR_INTERFACE=1
    # use SDK MS_OS_20 descriptor, as this doesn't have a custom one
    PICO_STDIO_USB_RESET_INTERFACE_SUPPORT_MS_OS_20_DESCRIPTOR=1
    # pass the interface number to the SDK MS_OS_20 descriptor
    PICO_STDIO_USB_RESET_INTERFACE_MS_OS_20_DESCRIPTOR_ITF=3
)

target_link_libraries(${BINARY} PRIVATE pico_mcp23017 QuadratureDecoder)
