#include <array>
#include <numeric>
#include <string>

#include <Adafruit_MLX90640.h>
#include <TFT_eSPI.h>

#include "config.h"

#include "algorithms.h"
#include "color.h"
#include "debounced_pin.h"
#include "debug_utils.h"
#include "draw_utils.h"
#include "fixed_matrix.h"
#include "mlx_utils.h"
#include "types/common_types.h"
#include "types/container_types.h"

using namespace thermocam;
using namespace thermocam::color;

// buffer for full frame of temperatures
ThermoImage raw_frame;
RGBThermoImage rgb_frame;
UpscaledRGBThermoImage upscaled_frame;

ThermoDisplaySettings tds{.min_scale_temp = DEFAULT_MANUAL_MIN_TEMP,
                          .max_scale_temp = DEFAULT_MANUAL_MAX_TEMP,
                          .mirror_mode = MirrorMode::MIRRORED_X,
                          .autoscale_active = false};

ThermoImageStats tis{.average_temp = 0.0,
                     .min_temp = 0.0,
                     .max_temp = 0.0,
                     .min_temp_index = 0,
                     .max_temp_index = 0,
                     .frame_index = 0};

Adafruit_MLX90640 mlx;
TFT_eSPI tft;
TwoWire mlx_i2c(0);
DebouncedPin button1(UI_BTN_PIN, PinMode::IN_PULLDOWN);

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
    mlx.setRefreshRate(DEFAULT_MLX_REFRESH_RATE);
}

void loop()
{
    if (button1.readout() == PinState::HIGH_LEVEL) {
        tds.autoscale_active = !tds.autoscale_active;
        Serial.println("switch autoscale");
    }

    if (mlx.getFrame(raw_frame.data()) != 0) {
        Serial.println("frame read failed");
        return;
    }
    tis.frame_index++;
    tis.frame_index %= 1000;
    mlx_utils::update_thermo_image_stats_from_frame(raw_frame, tis);

    if (tds.autoscale_active) {
        tds.min_scale_temp = tis.min_temp - 1.0;
        tds.max_scale_temp = tis.max_temp + 1.0;
    } else {
        tds.min_scale_temp = DEFAULT_MANUAL_MIN_TEMP;
        tds.max_scale_temp = DEFAULT_MANUAL_MAX_TEMP;
    }
    if constexpr (DEBUG_OUTPUT) {
        Serial.println(debug_utils::generate_debug_string(tds, tis).c_str());
    }

    mlx_utils::convert_raw_temp_to_color(raw_frame, rgb_frame, tds);

    algorithms::bilinear_upscale(rgb_frame, upscaled_frame);

    draw_utils::insert_min_max_temp_crosses_into_image(upscaled_frame, tis,
                                                       BILINEAR_INTERPOLATION_FACTOR,
                                                       common_colors::CYAN, common_colors::RED);
    draw_utils::draw_thermo_image(tft, upscaled_frame, DRAW_INTERPOLATION_FACTOR, tds.mirror_mode);
    draw_utils::draw_live_ui(tft, tds, tis);
}
