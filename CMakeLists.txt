cmake_minimum_required(VERSION 3.13)

# initialize the SDK directly

include(/workspaces/pico-sdk/pico_sdk_init.cmake)
project(my_project)
pico_sdk_init()


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_COMPILER_LAUNCHER ccache)
set(CMAKE_CXX_COMPILER_LAUNCHER ccache)
# initialize the Raspberry Pi Pico SDK

set(PICO_MBEDTLS_CONFIG_FILE src/mbedtls_config.h)

include_directories(src/)

add_compile_definitions(picow_tcpip_client_background
    WIFI_SSID=\"ZeltonPelase\"
    WIFI_PASSWORD=\"dusjelszo\"
)

add_compile_definitions(LWIP_TIMEVAL_PRIVATE=0)
add_compile_options(-I${PROJECT_SOURCE_DIR}/include/ -I/usr/include/newlib/c++/10.3.1/ -I/usr/include/newlib/ -I/usr/include/newlib/c++/10.3.1/arm-none-eabi/thumb/v6-m/nofp/ -g)

add_library(bmp280 STATIC
    "src/bmp280/bmp280.cpp"
    "src/bmp280/bmp280.hpp"
)


add_subdirectory(src/tls/)
#add_subdirectory(src/csv_to_googledrive/)
add_subdirectory(src/ntp/)
add_subdirectory(src/tcp/)

target_link_libraries(bmp280 
PUBLIC
    pico_stdlib
    pico_cyw43_arch_lwip_threadsafe_background
    pico_lwip_mbedtls
    pico_mbedtls
    hardware_i2c 
)

add_executable(hello_world
    "main.cpp"
)

include_directories(src/bmp_280/ src/)

pico_enable_stdio_usb(hello_world 1)
pico_enable_stdio_uart(hello_world 0)

target_link_libraries(hello_world
    pico_stdlib
    picow_tcpip_client_background
    picow_ntp_client_background
    bmp280
    hardware_i2c
    hardware_rtc
    pico_lwip_mbedtls
    pico_mbedtls
    picow_tls_verify_background
    #csv_to_googledrive
)

pico_add_extra_outputs(hello_world)
