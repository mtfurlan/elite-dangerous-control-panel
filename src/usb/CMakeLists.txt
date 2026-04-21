
# Make sure TinyUSB can find tusb_config.h
target_include_directories(${BINARY} PUBLIC
    .
)

target_sources(${BINARY} PUBLIC
    usb_descriptors.c
)
