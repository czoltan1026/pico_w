
#include "cyw43_ll.h"
#include "hardware/structs/watchdog.h"
#include "hardware/timer.h"
#include "hardware/watchdog.h"
#include "lwip/tcp.h"
#include "pico/printf.h"
#include "sys/unistd.h"
extern "C" {
#include "cyw43.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "pico/cyw43_arch.h"
#include "pico/error.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include "pico/unique_id.h"
}
#include <algorithm>
#include <iostream>
#include <random>
#include <string>

#include <iomanip>
#include <iostream>

#include "src/bmp280/bmp280.hpp"
#include "src/ntp/ntp.hpp"
#include "src/tcp/tcp_client.hpp"

#include <malloc.h>
#include <map>
#include <sstream>
#include <string>

#include "sys/time.h"
#include <filesystem>

uint32_t getTotalHeap(void) {
  extern char __StackLimit, __bss_end__;

  return &__StackLimit - &__bss_end__;
}

uint32_t getFreeHeap(void) {
  struct mallinfo m = mallinfo();

  return getTotalHeap() - m.uordblks;
}

extern "C" int tls_test();

void connect_to_wifi() {
  int err = PICO_ERROR_NONE;
  do {
    printf("wifi connecting: %d\n", err);
    fflush(0);
    err = cyw43_arch_wifi_connect_timeout_ms("ZeltonPelase", "dusjelszo", CYW43_AUTH_WPA2_AES_PSK, 30000);
  } while (!(err == PICO_ERROR_NONE));
  printf("wifi connected.\n");
}

std::string random_string() {
  const int         LENGTH     = 1024;
  const std::string CHARACTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

  static std::random_device       rd;
  static std::mt19937             generator(rd());
  std::uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);

  // Generate random string
  std::string random_string;
  random_string.reserve(LENGTH);
  for (int i = 0; i < LENGTH; ++i) {
    random_string += CHARACTERS[distribution(generator)];
  }

  return random_string;
}

bool worker(repeating_timer_t* timer) {

  printf("cyw43_tcpip_link_status %d\n", cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA));
  if (cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA) != CYW43_LINK_UP) {
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    connect_to_wifi();
  }

  static bmp280 bmp;
  bmp.update();
  static uint32_t count = 0;
  if (cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA) == CYW43_LINK_UP) {
    send_data_to_server(bmp.as_string());
  }

  return true;
}

int main() {

  stdio_init_all();
  if (cyw43_arch_init()) {
    run_ntp();
    printf("Wi-Fi init failed");
    return -1;
  }

  cyw43_arch_enable_sta_mode();
  sleep_ms(500);
  printf("asd");
  fflush(0);

  repeating_timer_t out;
  add_repeating_timer_ms(-5000, worker, nullptr, &out);
  while (true) {
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    puts("anya");
    sleep_ms(1000);
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
  }
}