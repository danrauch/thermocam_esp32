#pragma once

#include <stdint.h>

#include "color.h"
#include "types/mlx_types.h"

namespace thermocam {

constexpr bool DEBUG_OUTPUT = false;

constexpr uint32_t SERIAL_BAUDRATE = 115200;

constexpr uint32_t I2C_FREQUENCY_IN_HZ = 1'000'000;
constexpr uint8_t I2C_SDA_PIN = 6;
constexpr uint8_t I2C_SCL_PIN = 7;
constexpr uint8_t UI_BTN_PIN = 2;

constexpr uint8_t MLX_SENSOR_WIDTH = 32;
constexpr uint8_t MLX_SENSOR_HEIGHT = 24;
constexpr auto DEFAULT_MLX_REFRESH_RATE = Mlx90640RefreshRate::MLX90640_8_HZ;

constexpr uint8_t BILINEAR_INTERPOLATION_FACTOR = 2;
constexpr uint8_t DRAW_INTERPOLATION_FACTOR = 4;
constexpr uint16_t UPSCALED_IMAGE_WIDTH = MLX_SENSOR_WIDTH * BILINEAR_INTERPOLATION_FACTOR;
constexpr uint16_t UPSCALED_IMAGE_HEIGHT = MLX_SENSOR_HEIGHT * BILINEAR_INTERPOLATION_FACTOR;

constexpr uint8_t COLOR_BLEND_STEPS = 40;
constexpr auto MIN_TEMP_COLOR = color::common_colors::BLUE;
const auto MAX_TEMP_COLOR = color::common_colors::RED;

constexpr float DEFAULT_MANUAL_MIN_TEMP = 5.0;
constexpr float DEFAULT_MANUAL_MAX_TEMP = 40.0;
constexpr auto MIN_TFT_TEMP_COLOR = color::convert_rgb888_to_rgb565(
    MIN_TEMP_COLOR.r(), MIN_TEMP_COLOR.g(), MIN_TEMP_COLOR.b());
constexpr auto MAX_TFT_TEMP_COLOR = color::convert_rgb888_to_rgb565(
    MAX_TEMP_COLOR.r(), MAX_TEMP_COLOR.g(), MAX_TEMP_COLOR.b());

} // namespace thermocam