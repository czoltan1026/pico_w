#include "lwip/err.h"
#include "lwip/init.h"
#include "lwip/ip_addr.h"
#include "lwip/netif.h"
#include "lwip/priv/tcp_priv.h"
#include "lwip/tcp.h"
#include "lwip/tcpbase.h"
#include "lwipopts.h"
#include "pico/cyw43_arch.h"
#include <string.h>
#include <string>

#define SERVER_IP "192.168.0.2"
#define SERVER_PORT 4241

static struct tcp_pcb* tcp_client_pcb = NULL;
static ip_addr_t       server_ip;
static u16_t           server_port = SERVER_PORT;

static std::string payload;

static void tcp_client_abort(tcp_pcb* connection) {
  tcp_close(connection);
  tcp_abort(connection);
  tcp_client_pcb = NULL;
}

static err_t tcp_client_sent(void* arg, struct tcp_pcb* tpcb, u16_t len) {
  puts(__PRETTY_FUNCTION__);
  tcp_close(tpcb);
  tcp_output(tpcb);
  tcp_abort(tpcb);
  tcp_client_pcb = NULL;
  printf("asdasd %s\n",((std::string*)(arg))->c_str());
  return ERR_OK;
}

static err_t tcp_client_connected(void* arg, struct tcp_pcb* tpcb, err_t err) {
  if (err != ERR_OK) {
    tcp_client_abort(tpcb);
    return err;
  }
  err = tcp_write(tpcb, payload.c_str(), payload.length(), TCP_WRITE_FLAG_COPY);
  if (err != ERR_OK) {
    tcp_client_abort(tpcb);
    return err;
  }
  tcp_output(tpcb);
  tcp_sent(tpcb, tcp_client_sent);
  return ERR_OK;
}

void send_data_to_server(const std::string& data) {
  payload = data;
  if (tcp_client_pcb == NULL) {
    tcp_client_pcb = tcp_new();
    if (tcp_client_pcb == nullptr) {
      puts("tcp_client_pcb remains null");
      return;
    }
  }

  tcp_err(tcp_client_pcb, [](void* arg, err_t err) {
    {
      tcp_client_pcb = nullptr;
      puts("error fcn");
    }
  });

  IP4_ADDR(&server_ip, 192, 168, 0, 2);

  if (tcp_client_pcb->state == TIME_WAIT) {
    tcp_client_abort(tcp_client_pcb);
  }

  tcp_arg(tcp_client_pcb, &payload);
  err_t err = ERR_OK;
  if (tcp_client_pcb->state == CLOSED) {
    err_t err = tcp_connect(tcp_client_pcb, &server_ip, server_port, tcp_client_connected);
  } else {
    printf("was connected %d", tcp_client_pcb->state);
  }

  if (err != ERR_OK) {
    tcp_client_abort(tcp_client_pcb);
  }
}
