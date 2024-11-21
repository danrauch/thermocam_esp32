#include "color.h"

namespace thermocam::color {

// private

RGB8Color::RGB8Color(int r, int g, int b) : _r(std::max(std::min(r, 255), 0)),
                                            _g(std::max(std::min(g, 255), 0)),
                                            _b(std::max(std::min(b, 255), 0))
{
}

// public

RGB8Color RGB8Color::create_from_enum(CommonColor color)
{
    const auto [r, g, b] = decode_int_to_rgb(static_cast<uint32_t>(color));
    return RGB8Color::create_from_rgb(r, g, b);
}

RGB8Color RGB8Color::create_from_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    assert(r < 256 && g < 256 && b < 256);
    return RGB8Color(r, g, b);
}

RGB8Color RGB8Color::operator+(const RGB8Color &rhs) const
{
    return RGB8Color(_r + rhs.r(), _g + rhs.g(), _b + rhs.b());
}

RGB8Color RGB8Color::operator-(const RGB8Color &rhs) const
{
    return RGB8Color(_r - rhs.r(), _g - rhs.g(), _b - rhs.b());
}

RGB8Color RGB8Color::operator*(float rhs) const
{
    return RGB8Color(_r * rhs, _g * rhs, _b * rhs);
}

RGB8Color RGB8Color::operator*(double rhs) const
{
    return RGB8Color(_r * rhs, _g * rhs, _b * rhs);
}

RGB8Color RGB8Color::lerp(const RGB8Color &from_color, const RGB8Color &to_color, float fraction)
{
    if (fraction <= 0.0) {
        return from_color;
    }
    if (fraction >= 1.0) {
        return to_color;
    }

    return (to_color - from_color) * fraction + from_color;
}

std::vector<RGB8Color> RGB8Color::discrete_blend(const RGB8Color &from_color, const RGB8Color &to_color, uint32_t steps)
{
    std::vector<RGB8Color> color_vector;
    color_vector.reserve(steps);

    for (int current_step; current_step < steps; current_step++) {
        float fraction = current_step / steps;
        color_vector.emplace_back(RGB8Color::lerp(from_color, to_color, fraction));
    }

    return color_vector;
}

} // namespace thermocam::color