cmake_minimum_required(VERSION 3.13)

# initialize the SDK directly
include(/workspaces/pico-sdk/pico_sdk_init.cmake)

project(my_project)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()


add_compile_options(-I/usr/include/newlib/c++/10.3.1/ -I/usr/include/newlib/ -I/usr/include/newlib/c++/10.3.1/arm-none-eabi/thumb/v6-m/nofp/)

add_executable(hello_world
    main.cpp
)


pico_enable_stdio_usb(hello_world 1)
pico_enable_stdio_uart(hello_world 0)


target_link_libraries(hello_world pico_stdlib pico_cyw43_arch_none pico_stdlib hardware_adc )

pico_add_extra_outputs(hello_world)