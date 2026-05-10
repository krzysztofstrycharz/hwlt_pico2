cmake_minimum_required(VERSION 3.13)

# set pico-sdk path
set(PICO_SDK_PATH ${CMAKE_CURRENT_SOURCE_DIR}/pico-sdk)

# set mcu
set(PICO_BOARD pico2)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(${PICO_SDK_PATH}/pico_sdk_init.cmake)

project(hwlt_pico2 C CXX ASM)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project

add_executable(hwlt_pico2
        src/hwlt_pico2.c
)

# pull in common dependencies
target_link_libraries(hwlt_pico2 pico_stdlib)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(hwlt_pico2)