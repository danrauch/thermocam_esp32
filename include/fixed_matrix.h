#pragma once

#include <array>
#include <assert.h>

namespace thermocam {

template <typename T, size_t ROWS, size_t COLS>
class FixedSizeMatrix : public std::array<T, ROWS * COLS>
{
public:
    /*
    rows
    | x x x x
    | x x x x
    | x x x x
    v  ----> cols
    */
    /// Number of columns (x direction)
    [[nodiscard]]
    constexpr size_t cols() const noexcept
    {
        return COLS;
    }

    /// Number of rows (y direction)
    [[nodiscard]]
    constexpr size_t rows() const noexcept
    {
        return ROWS;
    }

    /// Access element at row and column
    [[nodiscard]]
    constexpr T &operator()(int row, int col)
    {
        assert(col < cols());
        assert(row < rows());

        return this->at(col + cols() * row);  // row major order
    }

    /// Access element at row and column (as const)
    [[nodiscard]]
    constexpr const T &operator()(int row, int col) const
    {
        assert(col < cols());
        assert(row < rows());

        return this->at(col + cols() * row);  // row major order
    }
};

} // namespace thermocam