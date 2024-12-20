#pragma once

#include "fixed_matrix.h"

namespace thermocam {

template <typename T, size_t IN_ROWS, size_t IN_COLS, size_t OUT_ROWS, size_t OUT_COLS>
constexpr void bilinear_upscale(
    const FixedSizeMatrix<T, IN_ROWS, IN_COLS> &in,
    FixedSizeMatrix<T, OUT_ROWS, OUT_COLS> &out) noexcept
{
    constexpr int SCALE_FACTOR = (int)(OUT_ROWS / IN_ROWS);
    static_assert(SCALE_FACTOR >= 1);

    if constexpr (SCALE_FACTOR == 1) {
        out = in;
        return;
    }

    for (int row_out = 0; row_out < out.rows() - SCALE_FACTOR; row_out++) {
        int row_in = (int)floor(row_out / SCALE_FACTOR);

        for (int col_out = 0; col_out < out.cols() - SCALE_FACTOR; col_out++) {
            int col_in = (int)floor(col_out / SCALE_FACTOR);

            T v00 = in(row_in, col_in);
            T v01 = in(row_in, col_in + 1);
            T v10 = in(row_in + 1, col_in);
            T v11 = in(row_in + 1, col_in + 1);

            double frac_row = row_out / SCALE_FACTOR - row_in;
            double frac_col = col_out / SCALE_FACTOR - col_in;

            out(row_out, col_out) = (1 - frac_row) * (1 - frac_col) * v00 + (1 - frac_row) * frac_col * v01 +
                                    frac_row * (1 - frac_col) * v10 + frac_row * frac_col * v11;
        }
    }
}

}