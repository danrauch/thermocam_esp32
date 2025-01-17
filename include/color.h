#pragma once

#include <array>
#include <cassert>
#include <ostream>
#include <stdint.h>
#include <tuple>
#include <vector>

// test some "fun" stuff :-)
#define FUN [[nodiscard]] constexpr auto
#define STATIC_FUN [[nodiscard]] constexpr static auto

namespace thermocam::color {

using RGBArray = std::array<uint8_t, 3>;

FUN convert_rgb888_to_rgb565(uint8_t r, uint8_t g, uint8_t b) noexcept -> uint16_t
{
    uint16_t r5 = (r >> 3) & 0x1F; // 5 bit for red
    uint16_t g6 = (g >> 2) & 0x3F; // 6 bit for green
    uint16_t b5 = (b >> 3) & 0x1F; // 5 bit for blue

    return (r5 << 11) | (g6 << 5) | b5;
}

FUN encode_rgb_to_int(uint8_t r, uint8_t g, uint8_t b) noexcept -> uint32_t
{
    return r << 16 | g << 8 | b;
}

FUN decode_int_to_rgb(uint32_t color) noexcept -> RGBArray
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
    STATIC_FUN create_from_enum(CommonColor color) noexcept -> RGB8Color
    {
        const auto [r, g, b] = decode_int_to_rgb(static_cast<uint32_t>(color));
        return RGB8Color::create_from_rgb(r, g, b);
    }

    STATIC_FUN create_from_rgb(uint8_t r, uint8_t g, uint8_t b) noexcept -> RGB8Color
    {
        assert(r < 256 && g < 256 && b < 256);
        return RGB8Color(r, g, b);
    }

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

    FUN r() const noexcept -> uint8_t { return _r; }
    FUN g() const noexcept -> uint8_t { return _g; }
    FUN b() const noexcept -> uint8_t { return _b; }
    FUN rgb_array() const noexcept -> RGBArray { return {_r, _g, _b}; }

    RGB8Color lerp(const RGB8Color &other, float fraction) { return RGB8Color::lerp(*this, other, fraction); }

    friend RGB8Color operator*(double factor, const RGB8Color &color)
    {
        return RGB8Color(color.r() * factor, color.g() * factor, color.b() * factor);
    }

    // Need default constructor so it can be in std::array
    constexpr RGB8Color() = default;

private:
    constexpr RGB8Color(int r, int g, int b) noexcept : _r(std::max(std::min(r, 255), 0)),
                                                        _g(std::max(std::min(g, 255), 0)),
                                                        _b(std::max(std::min(b, 255), 0))
    {
    }
    uint8_t _r = 0, _g = 0, _b = 0;
};

namespace common_colors {
constexpr auto BLACK = RGB8Color::create_from_enum(CommonColor::BLACK);
constexpr auto WHITE = RGB8Color::create_from_enum(CommonColor::WHITE);
constexpr auto RED = RGB8Color::create_from_enum(CommonColor::RED);
constexpr auto GREEN = RGB8Color::create_from_enum(CommonColor::GREEN);
constexpr auto BLUE = RGB8Color::create_from_enum(CommonColor::BLUE);
constexpr auto YELLOW = RGB8Color::create_from_enum(CommonColor::YELLOW);
constexpr auto CYAN = RGB8Color::create_from_enum(CommonColor::CYAN);
constexpr auto MAGENTA = RGB8Color::create_from_enum(CommonColor::MAGENTA);
} // namespace common_colors

} // namespace thermocam::color

#undef FUN
#undef STATIC_FUN