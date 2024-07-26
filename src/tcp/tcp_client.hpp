#pragma once

#include "lwip/err.h"
#include <cstdint>
#include <string>
int   tcp_client_init(const std::string&);
void  tcp_client_run(const std::string&);
void  tcp_client_connect(const std::string& ip);
err_t tcp_client_close();
void send_data_to_server(const std::string &data);