cmake_minimum_required(VERSION 3.13)

# NOTE: currently using https://github.com/raspberrypi/pico-sdk/pull/2629
include(vendor/pico-sdk/pico_sdk_init.cmake)
# remember: make sure pico sdk exists before project(…)

project(ed_control_panel)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

pico_sdk_init()


add_subdirectory(vendor/adamboardman-pico-interfaces)
add_subdirectory(vendor/adamboardman-pico-mcp23017)
add_subdirectory(vendor/QuadratureDecoder)

add_subdirectory(src)
