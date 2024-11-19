#include <Adafruit_MLX90640.h>

namespace thermocam
{

using Mlx90640RefreshRate = mlx90640_refreshrate;
using Mlx90640PixelReadoutMode = mlx90640_mode;
using Mlx90640BitResolution = mlx90640_res;

constexpr uint32_t convert_refresh_rate_to_ms(Mlx90640RefreshRate refresh_rate) 
{
  switch (refresh_rate)
  {
  case Mlx90640RefreshRate::MLX90640_0_5_HZ: return 2000;
  case Mlx90640RefreshRate::MLX90640_1_HZ: return 1000;
  case Mlx90640RefreshRate::MLX90640_2_HZ: return 500;
  case Mlx90640RefreshRate::MLX90640_4_HZ: return 250;
  case Mlx90640RefreshRate::MLX90640_8_HZ: return 125;
  case Mlx90640RefreshRate::MLX90640_16_HZ: return 62;
  case Mlx90640RefreshRate::MLX90640_32_HZ: return 31;
  case Mlx90640RefreshRate::MLX90640_64_HZ: return 15;
  default: return 2000;
  }
}

float normalize(float min, float max, float value_to_normalize)
{
    return (value_to_normalize - min) / (max - min);
}

} // namespace thermocam