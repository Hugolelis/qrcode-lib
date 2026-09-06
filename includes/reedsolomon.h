#pragma once
#include <vector>
#include <cstdint>
#include "galoisfield.h"

std::vector<uint8_t> buildGeneratorPolynomial(int ecCount, const GaloisField& gf);

std::vector<uint8_t> generateECCodewords(const std::vector<uint8_t>& data,
                                           int ecCount,
                                           const GaloisField& gf);