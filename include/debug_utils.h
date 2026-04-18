#pragma once

#include <string>

#include "Arduino.h"

#include "types/common_types.h"

namespace thermocam::debug_utils {

inline std::string generate_debug_string(const ThermoDisplaySettings &tds, const ThermoImageStats &stats)
{
    std::string msg("#:  Avg  |  Min  |  Max  [°C]:\n");
    msg += std::to_string(stats.frame_index) + ": " + std::to_string(stats.average_temp);
    if (tds.autoscale_active) {
        msg += " " + std::to_string(tds.min_scale_temp) + "@" + std::to_string(stats.min_temp_index) + " | " +
               std::to_string(tds.max_scale_temp) + "@" + std::to_string(stats.max_temp_index);
    } else {
        msg += "   n/a   |   n/a  ";
    }
    return msg;
}

} // namespace thermocam::debug_utils