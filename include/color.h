#include <array>
#include <stdint.h>
#include <tuple>
#include <vector>

namespace thermocam::color {

using RGBArray = std::array<uint8_t, 3>;

constexpr uint32_t encode_rgb_to_int(uint8_t r, uint8_t g, uint8_t b)
{
    return r << 16 | g << 8 | b;
}

constexpr RGBArray decode_int_to_rgb(uint32_t color)
{
    return { (uint8_t)(color & 0x00FF0000 >> 16), (uint8_t)(color & 0x0000FF00 >> 8), (uint8_t)(color & 0x000000FF)};
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

class Color final
{
public:
    static Color create_from_enum(CommonColor color);
    static Color create_from_rgb(uint8_t r, uint8_t g, uint8_t b);

    static Color lerp(const Color &color1, const Color &color2, float fraction);
    static std::vector<Color> discrete_blend(const Color &color1, const Color &color2, uint32_t steps);

    Color operator+(const Color &rhs) const;
    Color operator-(const Color &rhs) const;
    Color operator*(float rhs) const;
    Color operator*(double rhs) const;
    operator std::string() const
    {
        return "(" + std::to_string(_r) + " " + std::to_string(_g) + " " + std::to_string(_b) + ")";
    }
    friend std::ostream &operator<<(std::ostream &out, const Color &obj)
    {
        return out << static_cast<std::string>(obj);
    }

    uint8_t r() const { return _r; }
    uint8_t g() const { return _g; }
    uint8_t b() const { return _b; }
    RGBArray rgb_array() const { return {_r, _g, _b}; }

    Color lerp(const Color &other, float fraction) { return Color::lerp(*this, other, fraction); }

    Color() = default;
    Color(int r, int g, int b);
    friend Color operator*(double factor, const Color &color)
    {
        return Color(color.r() * factor, color.g() * factor, color.b() * factor);
    }

private:
    uint8_t _r, _g, _b;
};

} // namespace thermocam::color