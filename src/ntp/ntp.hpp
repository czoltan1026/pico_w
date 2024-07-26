#pragma once
#include "time.h"
#include <cstdint>
void            run_ntp();
extern uint64_t ntp_time;

inline uint64_t get_uptime() {
  static time_t   start_time = time(nullptr);
  time_t current_time = time(nullptr);
  return current_time - start_time;
}

inline uint64_t get_current_time() {
  return get_uptime() + ntp_time;
}
