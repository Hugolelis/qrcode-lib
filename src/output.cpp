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

    int total = matrix.size() + 2 * QUIET_ZONE;
    std::ofstream file(filename);

    file << "P1\n";
    file << total << " " << total << "\n";

    for (int r = 0; r < total; ++r) {
        for (int c = 0; c < total; ++c) {
            file << (isDark(matrix, r, c) ? "1" : "0") << " ";
        }
        file << "\n";
    }
}