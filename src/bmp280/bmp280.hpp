#pragma once

#include "pico/unique_id.h"
#include <array>
#include <chrono>
#include <cstdint>
#include <deque>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

uint32_t getFreeHeap(void);
struct bmp280 {

  struct measurement {
    uint64_t time;
    float    temperature;
    float    pressure;
    float    humidity;
  } current_measurement;

  bmp280();
  void update();

  std::string as_string() {
    std::stringstream ss;
    ss << std::setprecision(4);
    static char id[8];
    pico_get_unique_board_id_string(id, 8);

    for (const auto& meas : meas_container) {
      std::cout << id << "," << meas.time << "," << meas.temperature << "," << meas.pressure << "," << meas.humidity << std::endl;
      ss << id << "," << meas.time << "," << meas.temperature << "," << meas.pressure << "," << meas.humidity << "\n";
    }

    return ss.str();
  }

  static constexpr inline float temperature_diff = 0.25F;
  static constexpr inline float pressure_diff    = 10.0F;
  static constexpr inline float humidity_diff    = 1.0F;

private:
  struct calib_param {
    // temperature params
    uint16_t dig_t1;
    int16_t  dig_t2;
    int16_t  dig_t3;

    // pressure params
    uint16_t dig_p1;
    int16_t  dig_p2;
    int16_t  dig_p3;
    int16_t  dig_p4;
    int16_t  dig_p5;
    int16_t  dig_p6;
    int16_t  dig_p7;
    int16_t  dig_p8;
    int16_t  dig_p9;

    uint8_t dig_h1;
    int16_t dig_h2;
    uint8_t dig_h3;
    int16_t dig_h4;
    int16_t dig_h5;
    uint8_t dig_h6;
  };

  struct calib_param params;

  int32_t convert(int32_t temp);
  float   convert_temp(int32_t temp);
  float   convert_pressure(int32_t pressure, int32_t temp);
  float   convert_humidity(int32_t temp, int32_t adc_H);
  void    get_calib_params();
  void    init();
  void    read_raw(int32_t* temp, int32_t* pressure, int32_t* humidity);
  void    reset();

  std::deque<measurement> meas_container{};
};

struct ctrl_meas {

  enum Mode : uint8_t {
    sleep  = 0,
    forced = 1, // single measurement
    normal = 3,
  };

  enum Oversampling : uint8_t {
    skip = 0,
    x1   = 0b001,
    x2   = 0b010,
    x4   = 0b011,
    x8   = 0b100,
    x16  = 0b100,
  };

  uint8_t* as_uint8_t() {
    static_assert(sizeof(*this) == 2, "must be 2 bytes long");
    return reinterpret_cast<uint8_t*>(this);
  }

  const uint8_t address = 0xF4;
  Mode          mode   : 2;
  Oversampling  osrs_p : 3;
  Oversampling  osrs_t : 3;
};

struct Config {

  enum Standby : uint8_t {
    standby_0_5_ms  = 0b000,
    standby_62_5_ms = 0b001,
    standby_125_ms  = 0b010,
    standby_250_ms  = 0b011,
    standby_500_ms  = 0b100,
    standby_1000_ms = 0b101,
    standby_10_ms   = 0b110,
    standby_20_ms   = 0b111
  };

  enum Filter : uint8_t {
    off = 0,
    x2  = 0b001,
    x4  = 0b010,
    x8  = 0b011,
    x16 = 0b100,
  };

  const uint8_t address = 0xF5;
  uint8_t       spi3w_en : 1;
  uint8_t       unused   : 1;
  Filter        filter   : 3;
  Standby       t_sb     : 3;

  uint8_t* as_uint8_t() {
    static_assert(sizeof(*this) == 2, "must be 2 bytes long");
    return reinterpret_cast<uint8_t*>(this);
  }
};
struct ctrl_hum {
  ctrl_hum() : osrs_h(ctrl_meas::Oversampling::skip), reserved(0) {
  }
  const uint8_t address = 0xF2;

  ctrl_meas::Oversampling osrs_h   : 3;
  uint8_t                 reserved : 5;

  uint8_t* as_uint8_t() {
    static_assert(sizeof(*this) == 2, "must be 2 bytes long");
    return reinterpret_cast<uint8_t*>(this);
  }
};
