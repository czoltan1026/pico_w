/**
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

#include <random>
#include <set>

int main() {


  stdio_init_all();
  if (cyw43_arch_init()) {
    printf("Wi-Fi init failed");
    return -1;
  }
  std::random_device rd;
  std::set<float> hist;
  std::uniform_real_distribution<float> dist(0, 1.0F);
  while (true) {
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    const auto value = dist(rd);
    hist.emplace(value);
    printf("Hello, world! %f %d \n", value, hist.size());
    sleep_ms(50);
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    sleep_ms(50);
  }
}
