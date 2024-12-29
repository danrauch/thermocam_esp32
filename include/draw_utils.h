#pragma once

#include <cmath>
#include <sstream>
#include <iomanip>

#include <TFT_eSPI.h>

#include "color.h"
#include "fixed_matrix.h"
#include "types/common_types.h"
#include "types/container_types.h"

namespace thermocam::draw_utils {

void draw_arrow(TFT_eSPI &tft, int16_t x, int16_t y, int16_t size, int16_t color)
{
    tft.fillTriangle(x, y, x + size, y, x + size / 2, y + size + 2, color);
}

void draw_live_ui(TFT_eSPI &tft, ThermoDisplaySettings &tds, ThermoImageStats &tis)
{
    tft.fillRect(0, 185, 240, 53, TFT_BLACK);

    tft.setTextColor(TFT_WHITE, TFT_TRANSPARENT);
    tft.drawNumber(tis.frame_index, 3, 185, 2);

    if (tds.autoscale_active) {
        return; // TODO
    }

    std::stringstream min_temp_ss, max_temp_ss;
    min_temp_ss << std::fixed << std::setprecision(1) << tis.min_temp;
    max_temp_ss << std::fixed << std::setprecision(1) << tis.max_temp;

    int min_temp_x_pos = static_cast<int>(240.0f * algorithms::normalize(
                                                       tds.min_scale_temp, tds.max_scale_temp, tis.min_temp));
    tft.setTextColor(MIN_TFT_TEMP_COLOR, TFT_TRANSPARENT);
    draw_arrow(tft, min_temp_x_pos, 228, 6, MIN_TFT_TEMP_COLOR);
    tft.drawCentreString(min_temp_ss.str().c_str(), min_temp_x_pos, 215, 2);
    if (min_temp_x_pos > 20) {
        tft.drawNumber(tds.min_scale_temp, 3, 222, 2);
    }

    int max_temp_x_pos = static_cast<int>(240.0f * algorithms::normalize(
                                                       tds.min_scale_temp, tds.max_scale_temp, tis.max_temp));
    tft.setTextColor(MAX_TFT_TEMP_COLOR, TFT_TRANSPARENT);
    draw_arrow(tft, max_temp_x_pos, 228, 6, MAX_TFT_TEMP_COLOR);
    auto drawn_width = tft.drawCentreString(max_temp_ss.str().c_str(), max_temp_x_pos, 215, 2);
    if (max_temp_x_pos + drawn_width / 2 < 218) {
        tft.drawNumber(tds.max_scale_temp, 220, 222, 2);
    }
}

void draw_thermo_image(TFT_eSPI &tft, UpscaledRGBThermoImage &upscaled_frame,
                       int draw_interpolation_factor, MirrorMode mirror_mode)
{
    int draw_offset_x = 0;
    int draw_offset_y = 0;
    int flag_invert_x = 1;
    int flag_invert_y = 1;

    switch (mirror_mode) {
    case MirrorMode::NORMAL:
        break; // do nothing
    case MirrorMode::MIRRORED_X:
        draw_offset_x = TFT_WIDTH;
        flag_invert_x = -1;
        break;
    case MirrorMode::MIRRORED_Y:
        draw_offset_y = TFT_HEIGHT;
        flag_invert_y = -1;
        break;
    case MirrorMode::MIRRORED_XY:
        draw_offset_x = TFT_WIDTH;
        draw_offset_y = TFT_HEIGHT;
        flag_invert_x = -1;
        flag_invert_y = -1;
        break;
    default:
        break; // equals to NORMAL
    }

    for (size_t row = 0; row < upscaled_frame.rows(); row++) {
        for (size_t col = 1; col < upscaled_frame.cols(); col++) {
            const auto [r, g, b] = upscaled_frame(row, col).rgb_array();
            auto color = color::convert_rgb888_to_rgb565(r, g, b);
            tft.fillRect(draw_offset_x + flag_invert_x * (col - 1) * draw_interpolation_factor,
                         draw_offset_y + flag_invert_y * row * draw_interpolation_factor,
                         draw_interpolation_factor, draw_interpolation_factor, color);
        }
    }
}

template <typename T, size_t ROWS, size_t COLS>
void draw_cross_into_image(int row, int col, color::RGB8Color color, FixedSizeMatrix<T, ROWS, COLS> &image)
{
    if (row < 0 || row >= ROWS || col < 0 || col >= COLS) {
        return;
    }

    if (row >= 1) {
        image(row - 1, col) = color;
    }
    if (row >= 2) {
        image(row - 2, col) = color;
    }
    if (row < ROWS - 1) {
        image(row + 1, col) = color;
    }
    if (row < ROWS - 2) {
        image(row + 2, col) = color;
    }
    image(row, col) = color;
    if (col >= 1) {
        image(row, col - 1) = color;
    }
    if (col >= 2) {
        image(row, col - 2) = color;
    }
    if (col < COLS - 1) {
        image(row, col + 1) = color;
    }
    if (col < COLS - 2) {
        image(row, col + 2) = color;
    }
}

template <typename T, size_t ROWS, size_t COLS>
void insert_min_max_temp_crosses_into_image(FixedSizeMatrix<T, ROWS, COLS> &image, ThermoImageStats &tis,
                                            int bilinear_interpolation_factor,
                                            color::RGB8Color min_cross_color, color::RGB8Color max_cross_color)
{
    auto upscaled_min_temp_index = tis.min_temp_index * bilinear_interpolation_factor;
    auto upscaled_max_temp_index = tis.max_temp_index * bilinear_interpolation_factor;
    draw_cross_into_image(std::floor(upscaled_min_temp_index / image.cols()),
                          upscaled_min_temp_index % image.cols(),
                          min_cross_color, image);
    draw_cross_into_image(std::floor(upscaled_max_temp_index / image.cols()),
                          upscaled_max_temp_index % image.cols(),
                          max_cross_color, image);
}

} // namespace thermocam::draw