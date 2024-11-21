#pragma once

#include "fixed_matrix.h"

template <typename T, size_t IN_ROWS, size_t IN_COLS, size_t OUT_ROWS, size_t OUT_COLS>
constexpr void bilinear_upscale(
    const FixedSizeMatrix<T, IN_ROWS, IN_COLS> &in,
    FixedSizeMatrix<T, OUT_ROWS, OUT_COLS> &out)
{
    constexpr int SCALE_FACTOR = (int)(OUT_ROWS / IN_ROWS);
    static_assert(SCALE_FACTOR >= 1);

    for (int i = 0; i < out.rows() - SCALE_FACTOR; i++) {
        int ii = (int)floor(i / SCALE_FACTOR);

        for (int j = 0; j < out.cols() - SCALE_FACTOR; j++) {
            int jj = (int)floor(j / SCALE_FACTOR);

            T v00 = in(ii, jj), v01 = in(ii, jj + 1),
              v10 = in(ii + 1, jj), v11 = in(ii + 1, jj + 1);
            double fi = i / SCALE_FACTOR - ii, fj = j / SCALE_FACTOR - jj;
            out(i, j) = (1 - fi) * (1 - fj) * v00 + (1 - fi) * fj * v01 +
                        fi * (1 - fj) * v10 + fi * fj * v11;
        }

    }
}