#pragma once

#include <cmath>
#include <cstdio>

#include <TFT_eSPI.h>

#include "algorithms.h"
#include "color.h"
#include "config.h"
#include "fixed_matrix.h"
#include "types/common_types.h"
#include "types/container_types.h"

namespace thermocam::draw_utils {

inline void draw_arrow(TFT_eSPI &tft, int16_t x, int16_t y, int16_t size, int16_t color)
{
    tft.fillTriangle(x, y, x + size, y, x + size / 2, y + size + 2, color);
}

inline void draw_live_ui(TFT_eSPI &tft, const ThermoDisplaySettings &tds, const ThermoImageStats &tis)
{
    tft.fillRect(0, 185, 240, 53, TFT_BLACK);

    tft.setTextColor(TFT_WHITE, TFT_TRANSPARENT);
    tft.drawNumber(tis.frame_index, 3, 185, 2);

    char min_temp_buf[16];
    char max_temp_buf[16];
    std::snprintf(min_temp_buf, sizeof(min_temp_buf), "%.1f", static_cast<double>(tis.min_temp));
    std::snprintf(max_temp_buf, sizeof(max_temp_buf), "%.1f", static_cast<double>(tis.max_temp));

    if (tds.autoscale_active) {
        tft.setTextColor(TFT_GREEN, TFT_TRANSPARENT);
        tft.drawString("A", 230, 185, 2);
        tft.setTextColor(MIN_TFT_TEMP_COLOR, TFT_TRANSPARENT);
        tft.drawString(min_temp_buf, 3, 222, 2);
        tft.setTextColor(MAX_TFT_TEMP_COLOR, TFT_TRANSPARENT);
        tft.drawString(max_temp_buf, 210, 222, 2);
        return;
    }

    tft.setTextColor(TFT_LIGHTGREY, TFT_TRANSPARENT);
    tft.drawString("A", 230, 185, 2);

    int min_temp_x_pos = static_cast<int>(240.0f * algorithms::normalize(
                                                       tds.min_scale_temp, tds.max_scale_temp, tis.min_temp));
    tft.setTextColor(MIN_TFT_TEMP_COLOR, TFT_TRANSPARENT);
    draw_arrow(tft, min_temp_x_pos, 228, 6, MIN_TFT_TEMP_COLOR);
    tft.drawCentreString(min_temp_buf, min_temp_x_pos, 215, 2);
    if (min_temp_x_pos > 20) {
        tft.drawNumber(tds.min_scale_temp, 3, 222, 2);
    }

    int max_temp_x_pos = static_cast<int>(240.0f * algorithms::normalize(
                                                       tds.min_scale_temp, tds.max_scale_temp, tis.max_temp));
    tft.setTextColor(MAX_TFT_TEMP_COLOR, TFT_TRANSPARENT);
    draw_arrow(tft, max_temp_x_pos, 228, 6, MAX_TFT_TEMP_COLOR);
    auto drawn_width = tft.drawCentreString(max_temp_buf, max_temp_x_pos, 215, 2);
    if (max_temp_x_pos + drawn_width / 2 < 218) {
        tft.drawNumber(tds.max_scale_temp, 220, 222, 2);
    }
}

inline void draw_thermo_image(TFT_eSPI &tft, const UpscaledRGBThermoImage &upscaled_frame,
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

    const int strip_width = static_cast<int>(upscaled_frame.cols()) * draw_interpolation_factor;
    const int strip_height = draw_interpolation_factor;
    uint16_t strip[256 * 4];  // max 64*4 x 4
    const int num_cols = static_cast<int>(upscaled_frame.cols());
    const int num_rows = static_cast<int>(upscaled_frame.rows());

    for (int row = 0; row < num_rows; row++) {
        for (int line = 0; line < strip_height; line++) {
            for (int col = 0; col < num_cols; col++) {
                const auto [r, g, b] = upscaled_frame(row, col).rgb_array();
                tft.color565(r, g, b);
                const uint16_t color = color::convert_rgb888_to_rgb565(r, g, b);
                const uint16_t color_swapped = (color >> 8) | (color << 8);
                const int base = line * strip_width + col * draw_interpolation_factor;
                for (int k = 0; k < draw_interpolation_factor; k++) {
                    strip[base + k] = color_swapped;
                }
            }
            if (flag_invert_x == -1) {
                for (int i = 0; i < strip_width / 2; i++) {
                    const int j = strip_width - 1 - i;
                    const uint16_t tmp = strip[line * strip_width + i];
                    strip[line * strip_width + i] = strip[line * strip_width + j];
                    strip[line * strip_width + j] = tmp;
                }
            }
        }
        const int start_x = draw_offset_x + (flag_invert_x < 0 ? flag_invert_x * strip_width : 0);
        const int y = draw_offset_y + flag_invert_y * (row * draw_interpolation_factor);
        tft.pushImage(start_x, y, strip_width, strip_height, strip);
    }
}

inline void draw_cross_into_image(int row, int col, color::RGB8Color color, UpscaledRGBThermoImage &image)
{
    if (row < 0 || row >= image.rows() || col < 0 || col >= image.cols()) {
        return;
    }

    if (row >= 1) {
        image(row - 1, col) = color;
    }
    if (row >= 2) {
        image(row - 2, col) = color;
    }
    if (row < image.rows() - 1) {
        image(row + 1, col) = color;
    }
    if (row < image.rows() - 2) {
        image(row + 2, col) = color;
    }
    image(row, col) = color;
    if (col >= 1) {
        image(row, col - 1) = color;
    }
    if (col >= 2) {
        image(row, col - 2) = color;
    }
    if (col < image.cols() - 1) {
        image(row, col + 1) = color;
    }
    if (col < image.cols() - 2) {
        image(row, col + 2) = color;
    }
}

inline void insert_min_max_temp_crosses_into_image(UpscaledRGBThermoImage &image, const ThermoImageStats &tis,
                                            int bilinear_interpolation_factor,
                                            color::RGB8Color min_cross_color, color::RGB8Color max_cross_color)
{
    const auto upscaled_min_temp_index = tis.min_temp_index * bilinear_interpolation_factor;
    const auto upscaled_max_temp_index = tis.max_temp_index * bilinear_interpolation_factor;
    draw_cross_into_image(static_cast<int>(upscaled_min_temp_index / image.cols()),
                          static_cast<int>(upscaled_min_temp_index % image.cols()),
                          min_cross_color, image);
    draw_cross_into_image(static_cast<int>(upscaled_max_temp_index / image.cols()),
                          static_cast<int>(upscaled_max_temp_index % image.cols()),
                          max_cross_color, image);
}

inline void draw_streaming_ui(TFT_eSPI &tft, const char *ssid, const char *ip_addr, 
                              const ThermoImageStats &tis, bool force_black_bg = false)
{
    if (force_black_bg) {
        tft.fillScreen(TFT_BLACK);
    }
    
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    
    // Display WiFi SSID
    tft.drawString("SSID:", 5, 10, 2);
    tft.drawString(ssid, 45, 10, 2);
    
    // Display IP address
    tft.drawString("IP:", 5, 30, 2);
    tft.drawString(ip_addr, 45, 30, 2);
    
    // Display "STREAMING" in large text
    tft.setTextSize(3);
    tft.drawCentreString("STREAMING", 120, 80, 2);
    tft.setTextSize(1);
    
    // Display temperature values
    char avg_temp_buf[16];
    char min_temp_buf[16];
    char max_temp_buf[16];
    
    std::snprintf(avg_temp_buf, sizeof(avg_temp_buf), "%.1f C", static_cast<double>(tis.average_temp));
    std::snprintf(min_temp_buf, sizeof(min_temp_buf), "%.1f C", static_cast<double>(tis.min_temp));
    std::snprintf(max_temp_buf, sizeof(max_temp_buf), "%.1f C", static_cast<double>(tis.max_temp));
    
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("Avg:", 10, 175, 2);
    tft.drawString(avg_temp_buf, 70, 175, 2);
    
    tft.setTextColor(MIN_TFT_TEMP_COLOR, TFT_BLACK);
    tft.drawString("Min:", 10, 195, 2);
    tft.drawString(min_temp_buf, 70, 195, 2);
    
    tft.setTextColor(MAX_TFT_TEMP_COLOR, TFT_BLACK);
    tft.drawString("Max:", 10, 215, 2);
    tft.drawString(max_temp_buf, 70, 215, 2);
}

} // namespace thermocam::draw_utils