#include "color.h"

namespace thermocam::color {

// private

Color::Color(int r, int g, int b) : _r(std::max(std::min(r, 255), 0)),
                                    _g(std::max(std::min(g, 255), 0)),
                                    _b(std::max(std::min(b, 255), 0))
{
}

// public

Color Color::create_from_enum(CommonColor color)
{
    const auto [r, g, b] = decode_int_to_rgb(static_cast<uint32_t>(color));
    return Color::create_from_rgb(r, g, b);
}

Color Color::create_from_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    assert(r < 256 && g < 256 && b < 256);
    return Color(r, g, b);
}

Color Color::operator+(const Color &rhs) const
{
    return Color(_r + rhs.r(), _g + rhs.g(), _b + rhs.b());
}

Color Color::operator-(const Color &rhs) const
{
    return Color(_r - rhs.r(), _g - rhs.g(), _b - rhs.b());
}

Color Color::operator*(float rhs) const
{
    return Color(_r * rhs, _g * rhs, _b * rhs);
}

Color Color::operator*(double rhs) const
{
    return Color(_r * rhs, _g * rhs, _b * rhs);
}

Color Color::lerp(const Color &color1, const Color &color2, float fraction)
{
    if (fraction <= 0.0) {
        return color1;
    }
    if (fraction >= 1.0) {
        return color2;
    }

    return (color2 - color1) * fraction + color1;

    int r = (int)((float)(std::max(color2.r() - color1.r(), 0)) * fraction + (float)color1.r());
    int g = (int)((float)(std::max(color2.g() - color1.g(), 0)) * fraction + (float)color1.g());
    int b = (int)((float)(std::max(color2.b() - color1.b(), 0)) * fraction + (float)color1.b());

    return Color(r, g, b);
}

std::vector<Color> Color::discrete_blend(const Color &color1, const Color &color2, uint32_t steps)
{
    std::vector<Color> color_vector;
    color_vector.reserve(steps);

    for (int current_step; current_step < steps; current_step++) {
        float fraction = current_step / steps;
        color_vector.emplace_back(Color::lerp(color1, color2, fraction));
    }

    return color_vector;
}

} // namespace thermocam::color