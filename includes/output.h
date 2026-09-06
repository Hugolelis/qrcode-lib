#pragma once
#include <string>
#include "matrix.h"

void printToTerminal(const Matrix& matrix);
void writePBM(const Matrix& matrix, const std::string& filename);