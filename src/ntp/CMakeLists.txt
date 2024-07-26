add_library(picow_ntp_client_background
    STATIC
        ntp.cpp
)

target_include_directories(picow_ntp_client_background PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)
target_link_libraries(picow_ntp_client_background
    pico_cyw43_arch_lwip_threadsafe_background
    pico_lwip_mbedtls
    pico_mbedtls
    pico_stdlib
)