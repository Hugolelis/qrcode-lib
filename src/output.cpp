#include "output.h"
#include <iostream>
#include <fstream>
#include <filesystem>

namespace {
    const int QUIET_ZONE = 4;

    bool isDark(const Matrix& matrix, int row, int col) 
    {
        int size = matrix.size();
        bool insideMatrix = row >= QUIET_ZONE && row < QUIET_ZONE + size &&
                            col >= QUIET_ZONE && col < QUIET_ZONE + size;
        if (!insideMatrix) 
        {
            return false;  // quiet zone is always light
        }
        return matrix.get(row - QUIET_ZONE, col - QUIET_ZONE) == Module::DARK;
    }
}

void printToTerminal(const Matrix& matrix) 
{
    int total = matrix.size() + 2 * QUIET_ZONE;
    for (int r = 0; r < total; ++r) 
    {
        for (int c = 0; c < total; ++c) 
        {
            std::cout << (isDark(matrix, r, c) ? "██" : "  ");
        }
        std::cout << '\n';
    }
}

void writePBM(const Matrix& matrix, const std::string& filename) {
    std::filesystem::path filePath(filename);
    if (filePath.has_parent_path()) {
        std::filesystem::create_directories(filePath.parent_path());
    }

    const int MODULE_SCALE = 10;  // each module becomes a 10x10 pixel block
    int totalModules = matrix.size() + 2 * QUIET_ZONE;
    int totalPixels = totalModules * MODULE_SCALE;

    std::ofstream file(filename);
    file << "P1\n";
    file << totalPixels << " " << totalPixels << "\n";

    for (int r = 0; r < totalPixels; ++r) {
        for (int c = 0; c < totalPixels; ++c) {
            int moduleRow = r / MODULE_SCALE;
            int moduleCol = c / MODULE_SCALE;
            file << (isDark(matrix, moduleRow, moduleCol) ? "1" : "0") << " ";
        }
        file << "\n";
    }
}