cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(hwlt_pico2 C CXX ASM)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project

add_executable(hwlt_pico2
        hwlt_pico2.c
)

# pull in common dependencies
target_link_libraries(hwlt_pico2 pico_stdlib)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(hwlt_pico2)