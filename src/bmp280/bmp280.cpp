/**
 * Copyright (c) 2021 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

#include <cstdint>
#include <iostream>
#include <stdio.h>

#include "bmp280.hpp"
#include "cyw43.h"
#include "hardware/i2c.h"
#include "ntp/ntp.hpp"
#include "pico/binary_info.h"
#include "pico/stdlib.h"

/* Example code to talk to a BMP280 temperature and pressure sensor

   NOTE: Ensure the device is capable of being driven at 3.3v NOT 5v. The Pico
   GPIO (and therefore I2C) cannot be used at 5v.

   You will need to use a level shifter on the I2C lines if you want to run the
   board at 5v.

   Connections on Raspberry Pi Pico board, other boards may vary.

   GPIO PICO_DEFAULT_I2C_SDA_PIN (on Pico this is GP4 (pin 6)) -> SDA on BMP280
   board
   GPIO PICO_DEFAULT_I2C_SCK_PIN (on Pico this is GP5 (pin 7)) -> SCL on
   BMP280 board
   3.3v (pin 36) -> VCC on BMP280 board
   GND (pin 38)  -> GND on BMP280 board
*/

// device has default bus address of 0x76
#define ADDR _u(0x76)

// hardware registers
#define REG_CONFIG _u(0xF5)
#define REG_CTRL_MEAS _u(0xF4)
#define REG_RESET _u(0xE0)

#define REG_TEMP_XLSB _u(0xFC)
#define REG_TEMP_LSB _u(0xFB)
#define REG_TEMP_MSB _u(0xFA)

#define REG_PRESSURE_XLSB _u(0xF9)
#define REG_PRESSURE_LSB _u(0xF8)
#define REG_PRESSURE_MSB _u(0xF7)

// calibration registers
#define REG_DIG_T1_LSB _u(0x88)
#define REG_DIG_T1_MSB _u(0x89)
#define REG_DIG_T2_LSB _u(0x8A)
#define REG_DIG_T2_MSB _u(0x8B)
#define REG_DIG_T3_LSB _u(0x8C)
#define REG_DIG_T3_MSB _u(0x8D)
#define REG_DIG_P1_LSB _u(0x8E)
#define REG_DIG_P1_MSB _u(0x8F)
#define REG_DIG_P2_LSB _u(0x90)
#define REG_DIG_P2_MSB _u(0x91)
#define REG_DIG_P3_LSB _u(0x92)
#define REG_DIG_P3_MSB _u(0x93)
#define REG_DIG_P4_LSB _u(0x94)
#define REG_DIG_P4_MSB _u(0x95)
#define REG_DIG_P5_LSB _u(0x96)
#define REG_DIG_P5_MSB _u(0x97)
#define REG_DIG_P6_LSB _u(0x98)
#define REG_DIG_P6_MSB _u(0x99)
#define REG_DIG_P7_LSB _u(0x9A)
#define REG_DIG_P7_MSB _u(0x9B)
#define REG_DIG_P8_LSB _u(0x9C)
#define REG_DIG_P8_MSB _u(0x9D)
#define REG_DIG_P9_LSB _u(0x9E)
#define REG_DIG_P9_MSB _u(0x9F)

// number of calibration registers to be read
#define NUM_CALIB_PARAMS 24

bmp280::bmp280() {

  // useful information for picotool
  // clang-format off
  bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));
  bi_decl(bi_program_description("BMP280 I2C example for the Raspberry Pi Pico"));
  // clang-format on

  // I2C is "open drain", pull ups to keep signal high when no data is being
  // sent
  i2c_init(i2c_default, 1000 * 1000);
  gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
  gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
  gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

  // configure BMP280
  init();

  get_calib_params();
}

void bmp280::init() {

  ctrl_hum humidity{};
  humidity.osrs_h = ctrl_meas::Oversampling::x4;
  i2c_write_blocking(i2c_default, ADDR, humidity.as_uint8_t(), 2, false);

  ctrl_meas measurement{};
  measurement.osrs_p = ctrl_meas::Oversampling::x4;
  measurement.osrs_t = ctrl_meas::Oversampling::x8;
  measurement.mode   = ctrl_meas::normal;
  i2c_write_blocking(i2c_default, ADDR, measurement.as_uint8_t(), 2, false);

  Config config{};
  config.filter = Config::Filter::x8;
  config.t_sb   = Config::standby_500_ms;
  i2c_write_blocking(i2c_default, ADDR, config.as_uint8_t(), 2, false);
}

void bmp280::read_raw(int32_t* temp, int32_t* pressure, int32_t* humidity) {
  // BMP280 data registers are auto-incrementing and we have 3 temperature and
  // pressure registers each, so we start at 0xF7 and read 6 bytes to 0xFC
  // note: normal mode does not require further ctrl_meas and config register
  // writes

  uint8_t buf[8] = {};
  uint8_t reg    = REG_PRESSURE_MSB;
  i2c_write_blocking(i2c_default, ADDR, &reg, 1, true); // true to keep master control of bus
  i2c_read_blocking(i2c_default, ADDR, buf, 8, false);  // false - finished with bus

  // store the 20 bit read in a 32 bit signed integer for conversion
  *pressure = (buf[0] << 12) | (buf[1] << 4) | (buf[2] >> 4);
  *temp     = (buf[3] << 12) | (buf[4] << 4) | (buf[5] >> 4);
  *humidity = (buf[6] << 8) | buf[7];
}

void bmp280::reset() {
  // reset the device with the power-on-reset procedure
  uint8_t buf[2] = { REG_RESET, 0xB6 };
  i2c_write_blocking(i2c_default, ADDR, buf, 2, false);
}

// intermediate function that calculates the fine resolution temperature
// used for both pressure and temperature conversions
int32_t bmp280::convert(int32_t temp) {
  // use the 32-bit fixed point compensation implementation given in the
  // datasheet

  int32_t var1, var2;
  var1 = ((((temp >> 3) - ((int32_t)params.dig_t1 << 1))) * ((int32_t)params.dig_t2)) >> 11;
  var2
      = (((((temp >> 4) - ((int32_t)params.dig_t1)) * ((temp >> 4) - ((int32_t)params.dig_t1))) >> 12) * ((int32_t)params.dig_t3))
     >> 14;
  return var1 + var2;
}

float bmp280::convert_temp(int32_t temp) {
  // uses the BMP280 calibration parameters to compensate the temperature value
  // read from its registers
  int32_t t_fine = convert(temp);
  return (t_fine * 5 + 128) >> 8;
}

float bmp280::convert_pressure(int32_t pressure, int32_t temp) {
  // uses the BMP280 calibration parameters to compensate the pressure value
  // read from its registers

  int32_t t_fine = convert(temp);

  int32_t  var1, var2;
  uint32_t converted = 0.0;
  var1               = (((int32_t)t_fine) >> 1) - (int32_t)64000;
  var2               = (((var1 >> 2) * (var1 >> 2)) >> 11) * ((int32_t)params.dig_p6);
  var2 += ((var1 * ((int32_t)params.dig_p5)) << 1);
  var2 = (var2 >> 2) + (((int32_t)params.dig_p4) << 16);
  var1 = (((params.dig_p3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3) + ((((int32_t)params.dig_p2) * var1) >> 1)) >> 18;
  var1 = ((((32768 + var1)) * ((int32_t)params.dig_p1)) >> 15);
  if (var1 == 0) {
    return 0; // avoid exception caused by division by zero
  }
  converted = (((uint32_t)(((int32_t)1048576) - pressure) - (var2 >> 12))) * 3125;
  if (converted < 0x80000000) {
    converted = (converted << 1) / ((uint32_t)var1);
  } else {
    converted = (converted / (uint32_t)var1) * 2;
  }
  var1      = (((int32_t)params.dig_p9) * ((int32_t)(((converted >> 3) * (converted >> 3)) >> 13))) >> 12;
  var2      = (((int32_t)(converted >> 2)) * ((int32_t)params.dig_p8)) >> 13;
  converted = (uint32_t)((int32_t)converted + ((var1 + var2 + params.dig_p7) >> 4));
  return converted;
}

void bmp280::get_calib_params() {
  // raw temp and pressure values need to be calibrated according to
  // parameters generated during the manufacturing of the sensor
  // there are 3 temperature params, and 9 pressure params, each with a LSB
  // and MSB register, so we read from 24 registers

  uint8_t buf[NUM_CALIB_PARAMS] = { 0 };
  uint8_t reg                   = REG_DIG_T1_LSB;

  i2c_write_blocking(i2c_default, ADDR, &reg, 1, true);
  i2c_read_blocking(i2c_default, ADDR, buf, NUM_CALIB_PARAMS, false);

  // store these in a struct for later use
  params.dig_t1 = (uint16_t)(buf[1] << 8) | buf[0];
  params.dig_t2 = (int16_t)(buf[3] << 8) | buf[2];
  params.dig_t3 = (int16_t)(buf[5] << 8) | buf[4];

  params.dig_p1 = (uint16_t)(buf[7] << 8) | buf[6];
  params.dig_p2 = (int16_t)(buf[9] << 8) | buf[8];
  params.dig_p3 = (int16_t)(buf[11] << 8) | buf[10];
  params.dig_p4 = (int16_t)(buf[13] << 8) | buf[12];
  params.dig_p5 = (int16_t)(buf[15] << 8) | buf[14];
  params.dig_p6 = (int16_t)(buf[17] << 8) | buf[16];
  params.dig_p7 = (int16_t)(buf[19] << 8) | buf[18];
  params.dig_p8 = (int16_t)(buf[21] << 8) | buf[20];
  params.dig_p9 = (int16_t)(buf[23] << 8) | buf[22];

  reg = 0xA1;
  i2c_write_blocking(i2c_default, ADDR, &reg, 1, true);
  i2c_read_blocking(i2c_default, ADDR, buf, 1, false);

  params.dig_h1 = buf[0];

  reg = 0xE1;
  i2c_write_blocking(i2c_default, ADDR, &reg, 1, true);
  i2c_read_blocking(i2c_default, ADDR, buf, 6, false);

  int16_t dig_h4_lsb;
  int16_t dig_h4_msb;
  int16_t dig_h5_lsb;
  int16_t dig_h5_msb;
  params.dig_h2 = (int16_t)buf[1] << 8 | buf[0];
  params.dig_h3 = buf[2];
  dig_h4_msb    = (int16_t)(int8_t)buf[3] * 16;
  dig_h4_lsb    = (int16_t)(buf[4] & 0x0F);
  params.dig_h4 = dig_h4_msb | dig_h4_lsb;
  dig_h5_msb    = (int16_t)(int8_t)buf[5] * 16;
  dig_h5_lsb    = (int16_t)(buf[4] >> 4);
  params.dig_h5 = dig_h5_msb | dig_h5_lsb;
  params.dig_h6 = (int8_t)buf[6];
}

void bmp280::update() {

  int32_t raw_temperature = 0;
  int32_t raw_pressure    = 0;
  int32_t raw_humidity    = 0;

  read_raw(&raw_temperature, &raw_pressure, &raw_humidity);
  current_measurement.temperature = convert_temp(raw_temperature) / 100.0F;
  current_measurement.pressure    = convert_pressure(raw_pressure, raw_temperature);
  current_measurement.humidity    = convert_humidity(raw_temperature, raw_humidity);

  if (meas_container.size() > 10000) {
    meas_container.pop_front();
  }
  // if (cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA) == CYW43_LINK_UP) {
  //   send_data_to_server(bmp.as_string());
  // }
  meas_container.push_back({ 0, current_measurement.temperature, current_measurement.pressure, current_measurement.humidity });
}

#define dump(x) printf("%d: %s %llu\n", __LINE__, #x, x)
float bmp280::convert_humidity(int32_t temp, int32_t adc_H) {

  int32_t  var1;
  int32_t  var2;
  int32_t  var3;
  int32_t  var4;
  int32_t  var5;
  uint32_t humidity;
  uint32_t humidity_max = 102400;

  var1     = convert(temp) - ((int32_t)76800);
  var2     = (int32_t)(adc_H * 16384);
  var3     = (int32_t)(((int32_t)params.dig_h4) * 1048576);
  var4     = ((int32_t)params.dig_h5) * var1;
  var5     = (((var2 - var3) - var4) + (int32_t)16384) / 32768;
  var2     = (var1 * ((int32_t)params.dig_h6)) / 1024;
  var3     = (var1 * ((int32_t)params.dig_h3)) / 2048;
  var4     = ((var2 * (var3 + (int32_t)32768)) / 1024) + (int32_t)2097152;
  var2     = ((var4 * ((int32_t)params.dig_h2)) + 8192) / 16384;
  var3     = var5 * var2;
  var4     = ((var3 / 32768) * (var3 / 32768)) / 128;
  var5     = var3 - ((var4 * ((int32_t)params.dig_h1)) / 16);
  var5     = (var5 < 0 ? 0 : var5);
  var5     = (var5 > 419430400 ? 419430400 : var5);
  humidity = (uint32_t)(var5 / 4096);

  if (humidity > humidity_max) {
    humidity = humidity_max;
  }

  return humidity / 1000.0F;
}
