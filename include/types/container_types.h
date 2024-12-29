#pragma once

#include "config.h"

#include "color.h"
#include "fixed_matrix.h"

namespace thermocam {

using ThermoImage = FixedSizeMatrix<float, MLX_SENSOR_HEIGHT, MLX_SENSOR_WIDTH>;
using RGBThermoImage = FixedSizeMatrix<color::RGB8Color, MLX_SENSOR_HEIGHT, MLX_SENSOR_WIDTH>;
using UpscaledRGBThermoImage = FixedSizeMatrix<color::RGB8Color, UPSCALED_IMAGE_HEIGHT, UPSCALED_IMAGE_WIDTH>;

} // namespace thermocam