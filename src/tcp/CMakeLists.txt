add_library(picow_tcpip_client_background
    tcp_client.cpp
)


target_include_directories(picow_tcpip_client_background PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
        /workspaces/asio-1.30.2/include
        )
target_link_libraries(picow_tcpip_client_background
        pico_cyw43_arch_lwip_threadsafe_background
        pico_lwip_mbedtls
        pico_mbedtls
        pico_stdlib
)

