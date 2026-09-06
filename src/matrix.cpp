#include "matrix.h"
#include <iostream>

Matrix::Matrix(int version)
    : version_(version),
      size_(4 * version + 17),
      grid_(size_, std::vector<Module>(size_, Module::UNSET)),
      isDataModule_(size_, std::vector<bool>(size_, false)) {}

void Matrix::set(int row, int col, Module value) 
{
    grid_[row][col] = value;
}

Module Matrix::get(int row, int col) const 
{
    return grid_[row][col];
}

int Matrix::size() const 
{
    return size_;
}

void Matrix::drawFinderPattern(int topRow, int topCol) 
{
    for (int i = 0; i < 7; ++i) {
        for (int j = 0; j < 7; ++j) {
            bool isOuterRing = (i == 0 || i == 6 || j == 0 || j == 6);
            bool isCenter    = (i >= 2 && i <= 4 && j >= 2 && j <= 4);
            Module m = (isOuterRing || isCenter) ? Module::DARK : Module::LIGHT;
            set(topRow + i, topCol + j, m);
        }
    }
}

void Matrix::drawSeparators() 
{
    for (int i = 0; i < 8; ++i) {
        // around the top-left finder
        set(7, i, Module::LIGHT);
        set(i, 7, Module::LIGHT);

        // around the top-right finder
        set(7, size_ - 1 - i, Module::LIGHT);
        set(i, size_ - 8, Module::LIGHT);

        // around the bottom-left finder
        set(size_ - 8, i, Module::LIGHT);
        set(size_ - 1 - i, 7, Module::LIGHT);
    }
}

void Matrix::drawTimingPatterns() 
{
    for (int i = 8; i <= size_ - 9; ++i) {
        Module m = (i % 2 == 0) ? Module::DARK : Module::LIGHT;
        set(6, i, m);   // horizontal timing pattern (row 6)
        set(i, 6, m);   // vertical timing pattern (column 6)
    }
}

void Matrix::drawDarkModule() 
{
    // Formula: (4 * version + 9, 8)
    set(4 * version_ + 9, 8, Module::DARK);
}

void Matrix::drawAlignmentPatterns() 
{
    if (version_ == 1) return;  // Version 1 has no alignment patterns

    int center = size_ - 7;  // valid for versions 2-6 (single alignment pattern)
    for (int i = -2; i <= 2; ++i) {
        for (int j = -2; j <= 2; ++j) 
        {
            bool isOuterRing = (i == -2 || i == 2 || j == -2 || j == 2);
            bool isCenter = (i == 0 && j == 0);
            Module m = (isOuterRing || isCenter) ? Module::DARK : Module::LIGHT;
            set(center + i, center + j, m);
        }
    }
}

void Matrix::drawFunctionPatterns() {
    drawFinderPattern(0, 0);
    drawFinderPattern(0, size_ - 7);
    drawFinderPattern(size_ - 7, 0);
    drawSeparators();
    drawTimingPatterns();
    drawDarkModule();
    drawAlignmentPatterns();
}

void Matrix::reserveFormatInfoArea() 
{
    for (int i : {0, 1, 2, 3, 4, 5, 7, 8}) 
    {
        set(i, 8, Module::RESERVED);
        set(8, i, Module::RESERVED);
    }
    for (int i = size_ - 7; i <= size_ - 1; ++i) 
    {
        set(i, 8, Module::RESERVED);
    }
    for (int i = size_ - 8; i <= size_ - 1; ++i) 
    {
        set(8, i, Module::RESERVED);
    }
}

void Matrix::placeData(const std::vector<uint8_t>& codewords) 
{
    size_t bitIndex = 0;
    size_t totalBits = codewords.size() * 8;

    int col = size_ - 1;
    int direction = -1;  // -1 = moving upward, +1 = moving downward

    while (col > 0) 
    {
        if (col == 6) col--;  // skip the vertical timing pattern column entirely

        for (int i = 0; i < size_; ++i) 
        {
            int row = (direction == -1) ? (size_ - 1 - i) : i;

            for (int c = col; c >= col - 1; --c) 
            {
                if (get(row, c) == Module::UNSET) 
                {
                    bool bit = false;
                    if (bitIndex < totalBits) 
                    {
                        uint8_t byte = codewords[bitIndex / 8];
                        int bitInByte = 7 - (bitIndex % 8);
                        bit = (byte >> bitInByte) & 1;
                        ++bitIndex;
                    }
                    set(row, c, bit ? Module::DARK : Module::LIGHT);
                    isDataModule_[row][c] = true;
                }
            }
        }

        direction = -direction;
        col -= 2;
    }
}

void Matrix::applyMask0() 
{
    for (int r = 0; r < size_; ++r) 
    {
        for (int c = 0; c < size_; ++c) 
        {
            bool shouldFlip = isDataModule_[r][c] && (r + c) % 2 == 0;
            if (shouldFlip) 
            {
                Module current = get(r, c);
                Module flipped = (current == Module::DARK) ? Module::LIGHT : Module::DARK;
                set(r, c, flipped);
            }
        }
    }
}

void Matrix::placeFormatBits(uint16_t formatBits) 
{
    auto bitAt = [formatBits](int i) 
    {
        return (formatBits >> i) & 1;
    };

    // column 8: bit i goes to a row that depends on i (spec-defined ranges)
    for (int i = 0; i < 15; ++i) 
    {
        int row;
        if (i < 6) row = i;
        else if (i < 8) row = i + 1;
        else row = size_ - 15 + i;
        set(row, 8, bitAt(i) ? Module::DARK : Module::LIGHT);
    }

    // row 8: bit i goes to a column that depends on i (spec-defined ranges)
    for (int i = 0; i < 15; ++i) 
    {
        int col;
        if (i < 8) col = size_ - 1 - i;
        else if (i == 8) col = 7;
        else col = 14 - i;
        set(8, col, bitAt(i) ? Module::DARK : Module::LIGHT);
    }
}