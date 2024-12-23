#include <array>
#include <iomanip>
#include <numeric>
#include <sstream>
#include <string>

#include <Adafruit_MLX90640.h>
#include <TFT_eSPI.h>

#include "algorithms.h"
#include "color.h"
#include "draw_utils.h"
#include "fixed_matrix.h"
#include "mlx_utils.h"
#include "upscaling.h"

using namespace thermocam;
using namespace thermocam::color;

const uint8_t I2C_SDA = 6;
const uint8_t I2C_SCL = 7;

const bool DEBUG_OUTPUT = false;
const uint8_t MLX_SENSOR_WIDTH = 32;
const uint8_t MLX_SENSOR_HEIGHT = 24;
const uint8_t BILINEAR_INTERPOLATION_FACTOR = 2;
const uint8_t DRAW_INTERPOLATION_FACTOR = 4;
constexpr uint16_t DISPLAYED_IMAGE_WIDTH = MLX_SENSOR_WIDTH * BILINEAR_INTERPOLATION_FACTOR;
constexpr uint16_t DISPLAYED_IMAGE_HEIGHT = MLX_SENSOR_HEIGHT * BILINEAR_INTERPOLATION_FACTOR;
const uint8_t COLOR_BLEND_STEPS = 40;

const auto CURRENT_REFRESH_RATE = Mlx90640RefreshRate::MLX90640_8_HZ;
const auto MIN_TEMP_COLOR = common_colors::BLUE;
const auto MAX_TEMP_COLOR = common_colors::RED;

const auto MIN_TFT_TEMP_COLOR = convert_rgb888_to_rgb565(MIN_TEMP_COLOR.r(), MIN_TEMP_COLOR.g(), MIN_TEMP_COLOR.b());
const auto MAX_TFT_TEMP_COLOR = convert_rgb888_to_rgb565(MAX_TEMP_COLOR.r(), MAX_TEMP_COLOR.g(), MAX_TEMP_COLOR.b());

const float DEFAULT_MANUAL_MIN_TEMP = 5.0;
const float DEFAULT_MANUAL_MAX_TEMP = 40.0;

// sensor handle
Adafruit_MLX90640 mlx;
// buffer for full frame of temperatures
FixedSizeMatrix<float, MLX_SENSOR_HEIGHT, MLX_SENSOR_WIDTH> raw_frame;
FixedSizeMatrix<RGB8Color, MLX_SENSOR_HEIGHT, MLX_SENSOR_WIDTH> rgb_frame;
FixedSizeMatrix<RGB8Color, DISPLAYED_IMAGE_HEIGHT, DISPLAYED_IMAGE_WIDTH> displayed_frame;
uint32_t frame_index = 0;
bool autoscale_active = false;
float min_scale_temp = DEFAULT_MANUAL_MIN_TEMP;
float max_scale_temp = DEFAULT_MANUAL_MAX_TEMP;

TFT_eSPI tft = TFT_eSPI();
TwoWire mlx_i2c = TwoWire(0);

void setup()
{
    static_assert(TFT_WIDTH % COLOR_BLEND_STEPS == 0);

    Serial.begin(115200);
    while (!Serial)
        delay(10);
    delay(100);

    tft.init();

    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_TRANSPARENT);
    tft.setTextSize(1);

    auto color_legend = RGB8Color::discrete_blend(MIN_TEMP_COLOR, MAX_TEMP_COLOR, COLOR_BLEND_STEPS);
    auto size_step = tft.width() / color_legend.size();
    for (size_t i = 0; i < color_legend.size(); i++) {
        const auto [r, g, b] = color_legend[i].rgb_array();
        auto color = convert_rgb888_to_rgb565(r, g, b);
        tft.fillRect(i * size_step, 238, size_step, 2, color);
    }

    Serial.println("Search for MLX90640");
    mlx_i2c.begin(I2C_SDA, I2C_SCL, 1'000'000);
    if (!mlx.begin(MLX90640_I2CADDR_DEFAULT, &mlx_i2c)) {
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
}

void draw_arrow(TFT_eSPI &tft, int16_t x, int16_t y, int16_t size, int16_t color)
{
    tft.fillTriangle(x, y, x + size, y, x + size / 2, y + size + 2, color);
}

void loop()
{
    if (mlx.getFrame(raw_frame.data()) != 0) {
        Serial.println("frame read failed");
        return;
    }
    frame_index++;
    frame_index %= 1000;

    float average = std::accumulate(raw_frame.begin(), raw_frame.end(), 0.0f) / raw_frame.size();
    auto [min_temp_frame, max_temp_frame] = std::minmax_element(raw_frame.begin(), raw_frame.end());
    int min_temp_index = std::distance(raw_frame.begin(), min_temp_frame);
    int max_temp_index = std::distance(raw_frame.begin(), max_temp_frame);
    if (autoscale_active) {
        min_scale_temp = *min_temp_frame - 1.0;
        max_scale_temp = *max_temp_frame + 1.0;
    }
    if constexpr (DEBUG_OUTPUT) {
        std::string msg("#:  Avg  |  Min  |  Max  [°C]:\n");
        msg += std::to_string(frame_index) + ": " + std::to_string(average);
        if (autoscale_active) {
            msg += " " + std::to_string(min_scale_temp) + "@" + std::to_string(min_temp_index) + " | " +
                   std::to_string(max_scale_temp) + "@" + std::to_string(max_temp_index);
        } else {
            msg += "   n/a   |   n/a  ";
        }
        Serial.println(msg.c_str());
    }

    size_t rgb_array_index = 0;
    for (const auto &temp_at_pixel : raw_frame) {
        float normalized_temp = algorithms::normalize(min_scale_temp, max_scale_temp, temp_at_pixel);
        rgb_frame[rgb_array_index] = RGB8Color::lerp(MIN_TEMP_COLOR, MAX_TEMP_COLOR, normalized_temp);
        rgb_array_index += 1;
    }

    bilinear_upscale(rgb_frame, displayed_frame);

    auto upscaled_min_temp_index = min_temp_index * BILINEAR_INTERPOLATION_FACTOR;
    auto upscaled_max_temp_index = max_temp_index * BILINEAR_INTERPOLATION_FACTOR;
    draw_utils::draw_cross_into_image(floor(upscaled_min_temp_index / displayed_frame.cols()),
                                      upscaled_min_temp_index % displayed_frame.cols(),
                                      common_colors::CYAN, displayed_frame);
    draw_utils::draw_cross_into_image(floor(upscaled_max_temp_index / displayed_frame.cols()),
                                      upscaled_max_temp_index % displayed_frame.cols(),
                                      common_colors::RED, displayed_frame);

    for (size_t row = 0; row < displayed_frame.rows(); row++) {
        for (size_t col = 2; col < displayed_frame.cols(); col++) {
            const auto [r, g, b] = displayed_frame(row, col).rgb_array();
            auto color = convert_rgb888_to_rgb565(r, g, b);
            tft.fillRect((col - 2) * DRAW_INTERPOLATION_FACTOR,
                         row * DRAW_INTERPOLATION_FACTOR,
                         DRAW_INTERPOLATION_FACTOR, DRAW_INTERPOLATION_FACTOR, color);
        }
    }

    std::stringstream min_temp_ss, max_temp_ss;
    min_temp_ss << "min " << std::fixed << std::setprecision(2) << *min_temp_frame;
    max_temp_ss << "max " << std::fixed << std::setprecision(2) << *max_temp_frame;

    tft.fillRect(0, 185, 240, 53, TFT_BLACK);

    tft.setTextColor(TFT_WHITE, TFT_TRANSPARENT);
    tft.drawNumber(frame_index, 3, 185, 2);

    if (!autoscale_active) {
        int min_temp_x_pos = static_cast<int>(240.0f * algorithms::normalize(min_scale_temp, max_scale_temp, *min_temp_frame));
        draw_arrow(tft, min_temp_x_pos, 228, 6, MIN_TFT_TEMP_COLOR);

        int max_temp_x_pos = static_cast<int>(240.0f * algorithms::normalize(min_scale_temp, max_scale_temp, *max_temp_frame));
        draw_arrow(tft, max_temp_x_pos, 228, 6, MAX_TFT_TEMP_COLOR);
    }

    tft.setTextColor(MIN_TFT_TEMP_COLOR, TFT_TRANSPARENT);
    tft.drawString(min_temp_ss.str().c_str(), 105, 185, 2);
    tft.drawNumber(min_scale_temp, 3, 222, 2);

    tft.setTextColor(MAX_TFT_TEMP_COLOR, TFT_TRANSPARENT);
    tft.drawString(max_temp_ss.str().c_str(), 175, 185, 2);
    tft.drawNumber(max_scale_temp, 220, 222, 2);
}
