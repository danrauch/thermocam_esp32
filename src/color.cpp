#include "color.h"
#include "algorithms.h"

namespace thermocam::color {

// public

RGB8Color RGB8Color::operator+(const RGB8Color &rhs) const
{
    return RGB8Color(_r + rhs.r(), _g + rhs.g(), _b + rhs.b());
}

RGB8Color RGB8Color::operator-(const RGB8Color &rhs) const
{
    return RGB8Color(_r - rhs.r(), _g - rhs.g(), _b - rhs.b());
}

RGB8Color RGB8Color::operator*(const RGB8Color &rhs) const
{
    return RGB8Color(_r * rhs.r(), _g * rhs.g(), _b * rhs.b());
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
    if (fraction <= 0.0f) {
        return from_color;
    }
    if (fraction >= 1.0f) {
        return to_color;
    }

    const auto r = static_cast<int>(from_color.r() + (to_color.r() - from_color.r()) * fraction);
    const auto g = static_cast<int>(from_color.g() + (to_color.g() - from_color.g()) * fraction);
    const auto b = static_cast<int>(from_color.b() + (to_color.b() - from_color.b()) * fraction);

    return RGB8Color::create_from_rgb(r, g, b);
}

std::vector<RGB8Color> RGB8Color::discrete_blend(const RGB8Color &from_color, const RGB8Color &to_color, uint32_t steps)
{       
    std::vector<RGB8Color> color_vector;
    color_vector.reserve(steps);
    float step = 1.0 / (steps - 1);
    float fraction = 0.0;
    for (int current_step = 0; current_step < steps; current_step++) {
        color_vector.emplace_back(algorithms::lerp(from_color, to_color, fraction));
        fraction += step;
    }

    return color_vector;
}

} // namespace thermocam::color