#pragma once

#include <cassert>
#include <array>
#include <ostream>
#include <stdint.h>
#include <tuple>
#include <vector>

namespace thermocam::color {

using RGBArray = std::array<uint8_t, 3>;

constexpr uint16_t convert_rgb888_to_rgb565(uint8_t r, uint8_t g, uint8_t b) {
    uint16_t r5 = (r >> 3) & 0x1F; // 5 bit for red
    uint16_t g6 = (g >> 2) & 0x3F; // 6 bit for green
    uint16_t b5 = (b >> 3) & 0x1F; // 5 bit for blue

    return (r5 << 11) | (g6 << 5) | b5;
}

constexpr uint32_t encode_rgb_to_int(uint8_t r, uint8_t g, uint8_t b)
{
    return r << 16 | g << 8 | b;
}

constexpr RGBArray decode_int_to_rgb(uint32_t color)
{
    return {(uint8_t)((color & 0x00FF0000) >> 16), (uint8_t)((color & 0x0000FF00) >> 8), (uint8_t)(color & 0x000000FF)};
}

enum class CommonColor : uint32_t
{
    BLACK = encode_rgb_to_int(0, 0, 0),
    WHITE = encode_rgb_to_int(255, 255, 255),
    RED = encode_rgb_to_int(255, 0, 0),
    GREEN = encode_rgb_to_int(0, 255, 0),
    BLUE = encode_rgb_to_int(0, 0, 255),
    YELLOW = encode_rgb_to_int(255, 255, 0),
    CYAN = encode_rgb_to_int(0, 255, 255),
    MAGENTA = encode_rgb_to_int(255, 0, 255),
};

class RGB8Color final
{
public:
    static RGB8Color create_from_enum(CommonColor color);
    static RGB8Color create_from_rgb(uint8_t r, uint8_t g, uint8_t b);

    static RGB8Color lerp(const RGB8Color &from_color, const RGB8Color &to_color, float fraction);
    static std::vector<RGB8Color> discrete_blend(const RGB8Color &from_color, const RGB8Color &to_color, uint32_t steps);

    RGB8Color operator+(const RGB8Color &rhs) const;
    RGB8Color operator-(const RGB8Color &rhs) const;
    RGB8Color operator*(const RGB8Color &rhs) const;
    RGB8Color operator*(float rhs) const;
    RGB8Color operator*(double rhs) const;
    operator std::string() const
    {
        return "(" + std::to_string(_r) + " " + std::to_string(_g) + " " + std::to_string(_b) + ")";
    }
    friend std::ostream &operator<<(std::ostream &out, const RGB8Color &obj)
    {
        return out << static_cast<std::string>(obj);
    }

    uint8_t r() const { return _r; }
    uint8_t g() const { return _g; }
    uint8_t b() const { return _b; }
    RGBArray rgb_array() const { return {_r, _g, _b}; }

    RGB8Color lerp(const RGB8Color &other, float fraction) { return RGB8Color::lerp(*this, other, fraction); }

    friend RGB8Color operator*(double factor, const RGB8Color &color)
    {
        return RGB8Color(color.r() * factor, color.g() * factor, color.b() * factor);
    }

    RGB8Color() = default;  // Need default constructor so it can be in std::array
private:
    RGB8Color(int r, int g, int b);
    uint8_t _r, _g, _b;
};


namespace common_colors {
    inline const auto BLACK = RGB8Color::create_from_enum(CommonColor::BLACK);
    inline const auto WHITE = RGB8Color::create_from_enum(CommonColor::WHITE);
    inline const auto RED = RGB8Color::create_from_enum(CommonColor::RED);
    inline const auto GREEN = RGB8Color::create_from_enum(CommonColor::GREEN);
    inline const auto BLUE = RGB8Color::create_from_enum(CommonColor::BLUE);
    inline const auto YELLOW = RGB8Color::create_from_enum(CommonColor::YELLOW);
    inline const auto CYAN = RGB8Color::create_from_enum(CommonColor::CYAN);
    inline const auto MAGENTA = RGB8Color::create_from_enum(CommonColor::MAGENTA);
}

} // namespace thermocam::color