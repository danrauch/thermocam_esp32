#pragma once

#include "color.h"
#include "fixed_matrix.h"

namespace thermocam::draw_utils {

template <typename T, size_t ROWS, size_t COLS>
void draw_cross_into_image(int row, int col, color::RGB8Color color, FixedSizeMatrix<T, ROWS, COLS> &image)
{
    if (row < 0 || row >= ROWS || col < 0 || col >= COLS) {
        return;
    }

    if (row >= 1) {
        image(row - 1, col) = color;
    }
    if (row >= 2) {
        image(row - 2, col) = color;
    }
    if (row < ROWS - 1) {
        image(row + 1, col) = color;
    }
    if (row < ROWS - 2) {
        image(row + 2, col) = color;
    }
    image(row, col) = color;
    if (col >= 1) {
        image(row, col - 1) = color;
    }
    if (col >= 2) {
        image(row, col - 2) = color;
    }
    if (col < COLS - 1) {
        image(row, col + 1) = color;
    }
    if (col < COLS - 2) {
        image(row, col + 2) = color;
    }
}

} // namespace thermocam