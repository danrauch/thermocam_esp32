#pragma once

#include <assert.h>
#include <cmath>
#include <initializer_list>
#include <memory>

template <typename T, size_t ROWS, size_t COLS>
class FixedSizeMatrix : public std::array<T, ROWS * COLS>
{
public:
    [[nodiscard]]
    constexpr size_t cols() const noexcept
    {
        return COLS;
    }

    [[nodiscard]]
    constexpr size_t rows() const noexcept
    {
        return ROWS;
    }

    constexpr T &operator()(int i, int j)
    {
        assert(j < COLS && i < ROWS);
        return this->at(i + ROWS * j);
    }

    constexpr const T &operator()(int i, int j) const
    {
        assert(j < COLS && i < ROWS);
        return this->at(i + ROWS * j);
    }

private:
    // matrix multiplication
    constexpr friend FixedSizeMatrix operator*(const FixedSizeMatrix &a, const FixedSizeMatrix &b)
    {
        assert(a.cols() == b.rows());
        FixedSizeMatrix<T, ROWS, COLS> result(a.rows(), b.cols());
        for (int i = 0; i < a.rows(); i++)
            for (int j = 0; j < b.cols(); j++) {
                T v = 0;
                for (int k = 0; k < a.cols(); k++)
                    v += a(i, k) * b(k, j);
                result(i, j) = v;
            }
        return result;
    }
};
