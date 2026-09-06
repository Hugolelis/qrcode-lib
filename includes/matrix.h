#pragma once
#include <vector>
#include <cstdint>

enum class Module : uint8_t {
    UNSET = 0,
    LIGHT = 1,
    DARK  = 2,
    RESERVED = 3   // position is fixed, but the value is written later (Etapa 6)
};

class Matrix
{
    public:
        explicit Matrix(int size);

        void drawFunctionPatterns();
        void drawFinderPattern(int topRow, int topCol);
        void drawSeparators();
        void drawTimingPatterns();
        void drawDarkModule();

        void reserveFormatInfoArea();

        void placeData(const std::vector<uint8_t>& codewords);

        void applyMask0();

        void set(int row, int col, Module value);
        Module get(int row, int col) const;
        
        int size() const;
        void print() const;

    private:
        int size_;
        std::vector<std::vector<Module>> grid_;
        std::vector<std::vector<bool>> isDataModule_;  // true only for modules written by placeData
};