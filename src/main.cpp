#include <array>
#include <numeric>

#include <Adafruit_MLX90640.h>

#include "color.h"
#include "fixed_matrix.h"
#include "utils.h"

using namespace thermocam;
using namespace thermocam::color;

const uint8_t MLX_SENSOR_WIDTH = 32;
const uint8_t MLX_SENSOR_HEIGHT = 24;
const auto CURRENT_REFRESH_RATE = Mlx90640RefreshRate::MLX90640_4_HZ;
const auto MIN_TEMP_COLOR = Color::create_from_enum(CommonColor::BLACK);
const auto MAX_TEMP_COLOR =  Color::create_from_rgb(0, 0, 255);
const uint8_t COLOR_BLEND_STEPS = 64;
const float DEFAULT_MANUAL_MIN_TEMP = 0.0;
const float DEFAULT_MANUAL_MAX_TEMP = 40.0;
const uint8_t INTERPOLATION_FACTOR = 6;

// sensor handle
Adafruit_MLX90640 mlx;
// buffer for full frame of temperatures
FixedSizeMatrix<float, MLX_SENSOR_WIDTH, MLX_SENSOR_HEIGHT> raw_frame;
FixedSizeMatrix<Color, MLX_SENSOR_WIDTH, MLX_SENSOR_HEIGHT> rgb_frame;
FixedSizeMatrix<Color,
                MLX_SENSOR_WIDTH * INTERPOLATION_FACTOR,
                MLX_SENSOR_HEIGHT * INTERPOLATION_FACTOR>
    displayed_frame;
uint32_t frame_index = 0;
bool autoscale_active = true;
float min_temp = DEFAULT_MANUAL_MIN_TEMP;
float max_temp = DEFAULT_MANUAL_MAX_TEMP;

template <typename T, size_t IN_ROWS, size_t IN_COLS, size_t OUT_ROWS, size_t OUT_COLS>
constexpr void bilinear_upscale(
    const FixedSizeMatrix<T, IN_ROWS, IN_COLS> &in,
    FixedSizeMatrix<T, OUT_ROWS, OUT_COLS> &out)
{
    constexpr int SCALE_FACTOR = (int)(OUT_ROWS / IN_ROWS);
    static_assert(SCALE_FACTOR >= 1);

    for (int i = 0; i < out.rows() - SCALE_FACTOR; i++) {
        int ii = (int)floor(i / SCALE_FACTOR);

        for (int j = 0; j < out.cols() - SCALE_FACTOR; j++) {
            int jj = (int)floor(j / SCALE_FACTOR);

            T v00 = in(ii, jj), v01 = in(ii, jj + 1),
              v10 = in(ii + 1, jj), v11 = in(ii + 1, jj + 1);
            double fi = i / SCALE_FACTOR - ii, fj = j / SCALE_FACTOR - jj;
            out(i, j) = (1 - fi) * (1 - fj) * v00 + (1 - fi) * fj * v01 +
                        fi * (1 - fj) * v10 + fi * fj * v11;
        }

    }
}

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        delay(10);
    delay(100);

    Serial.println("Search for MLX90640");
    if (!mlx.begin(MLX90640_I2CADDR_DEFAULT, &Wire)) {
        Serial.println("MLX90640 not found!");
        while (1)
            delay(10);
    }

    Serial.println("Found MLX90640 with serial number: ");
    Serial.print(mlx.serialNumber[0], HEX);
    Serial.print(mlx.serialNumber[1], HEX);
    Serial.println(mlx.serialNumber[2], HEX);

    mlx.setMode(Mlx90640PixelReadoutMode::MLX90640_CHESS);
    mlx.setResolution(Mlx90640BitResolution::MLX90640_ADC_18BIT);
    mlx.setRefreshRate(CURRENT_REFRESH_RATE);

    auto color_legend = Color::discrete_blend(MIN_TEMP_COLOR, MAX_TEMP_COLOR, COLOR_BLEND_STEPS);
    // TODO: draw legend to screen
}

void loop()
{
    // delay(convert_refresh_rate_to_ms(CURRENT_REFRESH_RATE));
    if (mlx.getFrame(raw_frame.data()) != 0) {
        Serial.println("frame read failed");
        return;
    }
    frame_index++;

    float average = std::accumulate(raw_frame.begin(), raw_frame.end(), 0.0f) / raw_frame.size();
    std::string msg("#:  Avg  |  Min  |  Max  [°C]:\n");
    msg += std::to_string(frame_index) + ": " + std::to_string(average);

    if (autoscale_active) {
        auto [min_temp_frame, max_temp_frame] = std::minmax_element(raw_frame.begin(), raw_frame.end());
        min_temp = *min_temp_frame;
        max_temp = *max_temp_frame;
        msg += " " + std::to_string(*min_temp_frame) + " | " + std::to_string(*max_temp_frame);
    } else {
        msg += "   n/a   |   n/a  ";
    }
    Serial.println(msg.c_str());

    size_t rgb_array_index = 0;
    for (const auto &temp_at_pixel : raw_frame) {
        float normalized_temp = normalize(min_temp, max_temp, temp_at_pixel);
        auto color = Color::lerp(MIN_TEMP_COLOR, MAX_TEMP_COLOR, normalized_temp);
        Serial.println(std::string(MIN_TEMP_COLOR).c_str());
        Serial.println(std::string(MAX_TEMP_COLOR).c_str());
        Serial.println(std::string(color).c_str());
        Serial.println("");
        rgb_frame[rgb_array_index] = color;
        rgb_array_index += 1;
    }

    //bilinear_upscale(rgb_frame, displayed_frame);

    //Serial.println(std::string(rgb_frame(10, 10)).c_str());
    //Serial.println(std::string(displayed_frame(100, 100)).c_str());
}
