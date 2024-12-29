#pragma once

#include <stdint.h>

namespace thermocam {

enum class MirrorMode
{
    NORMAL,
    MIRRORED_X,
    MIRRORED_Y,
    MIRRORED_XY
};

struct ThermoDisplaySettings
{
    float min_scale_temp;
    float max_scale_temp;
    MirrorMode mirror_mode;
    bool autoscale_active;
};

struct ThermoImageStats
{
    float average_temp;
    float min_temp;
    float max_temp;
    uint32_t min_temp_index;
    uint32_t max_temp_index;
    uint32_t frame_index;
};

} // namespace thermocam
