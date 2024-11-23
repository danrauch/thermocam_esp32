#pragma once

namespace thermocam::algorithms {

template <typename T>
[[nodiscard]] constexpr T lerp(T from, T to, float fraction) noexcept
{
    if (fraction <= 0.0) {
        return from;
    }
    if (fraction >= 1.0) {
        return to;
    }
    return (to - from) * fraction + from;
}

template <typename T>
[[nodiscard]] constexpr T normalize(T min, T max, T value_to_normalize) noexcept
{
    return (value_to_normalize - min) / (max - min);
}

}