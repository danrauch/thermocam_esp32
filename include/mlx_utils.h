#pragma once

#include <Adafruit_MLX90640.h>

#include "color.h"
#include "types/common_types.h"
#include "types/container_types.h"

namespace thermocam::mlx_utils {

void update_thermo_image_stats_from_frame(ThermoImage &raw_frame, ThermoImageStats &tis)
{
    tis.average_temp = std::accumulate(raw_frame.begin(), raw_frame.end(), 0.0f) / raw_frame.size();
    auto [min_temp_frame, max_temp_frame] = std::minmax_element(raw_frame.begin(), raw_frame.end());
    tis.min_temp_index = std::distance(raw_frame.begin(), min_temp_frame);
    tis.max_temp_index = std::distance(raw_frame.begin(), max_temp_frame);
    tis.min_temp = *min_temp_frame;
    tis.max_temp = *max_temp_frame;
}

void convert_raw_temp_to_color(ThermoImage &raw_frame, RGBThermoImage &rgb_frame, ThermoDisplaySettings &tds)
{
    size_t rgb_array_index = 0;
    for (const auto &temp_at_pixel : raw_frame) {
        float normalized_temp = algorithms::normalize(tds.min_scale_temp, tds.max_scale_temp, temp_at_pixel);
        rgb_frame[rgb_array_index] = color::RGB8Color::lerp(MIN_TEMP_COLOR, MAX_TEMP_COLOR, normalized_temp);
        rgb_array_index += 1;
    }
}

constexpr uint32_t convert_refresh_rate_to_ms(Mlx90640RefreshRate refresh_rate)
{
    switch (refresh_rate) {
    case Mlx90640RefreshRate::MLX90640_0_5_HZ:
        return 2000;
    case Mlx90640RefreshRate::MLX90640_1_HZ:
        return 1000;
    case Mlx90640RefreshRate::MLX90640_2_HZ:
        return 500;
    case Mlx90640RefreshRate::MLX90640_4_HZ:
        return 250;
    case Mlx90640RefreshRate::MLX90640_8_HZ:
        return 125;
    case Mlx90640RefreshRate::MLX90640_16_HZ:
        return 62;
    case Mlx90640RefreshRate::MLX90640_32_HZ:
        return 31;
    case Mlx90640RefreshRate::MLX90640_64_HZ:
        return 15;
    default:
        return 2000;
    }
}

} // namespace thermocam::mlx_utils