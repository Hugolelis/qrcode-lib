#include "matrix.h"
#include <iostream>

Matrix::Matrix(int size)
    : size_(size),
      grid_(size, std::vector<Module>(size, Module::UNSET)),
      isDataModule_(size, std::vector<bool>(size, false)) {}

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

void Matrix::print() const 
{
    for (int r = 0; r < size_; ++r) {
        for (int c = 0; c < size_; ++c) {
            std::cout << (get(r, c) == Module::DARK ? "██" : "  ");
        }
        std::cout << '\n';
    }
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
    // Formula: (4 * version + 9, 8). Version 1 -> (13, 8)
    set(13, 8, Module::DARK);
}

void Matrix::drawFunctionPatterns() 
{
    drawFinderPattern(0, 0);
    drawFinderPattern(0, size_ - 7);
    drawFinderPattern(size_ - 7, 0);
    drawSeparators();
    drawTimingPatterns();
    drawDarkModule();
}

void Matrix::reserveFormatInfoArea() 
{
    // Copy A, around the top-left finder
    for (int i : {0, 1, 2, 3, 4, 5, 7, 8}) 
    {
        set(i, 8, Module::RESERVED);
        set(8, i, Module::RESERVED);
    }

    // Copy B, split between the top-right and bottom-left finders
    for (int i = 14; i <= 20; ++i) 
    {
        set(i, 8, Module::RESERVED);
    }
    for (int i = 13; i <= 20; ++i) 
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