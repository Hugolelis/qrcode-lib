#pragma once
#include <cstdint>
#include <array>

class GaloisField {
public:
    GaloisField();

    uint8_t expTable(int index) const;
    uint8_t logTable(int value) const;
    uint8_t multiply(uint8_t a, uint8_t b) const;

private:
    std::array<uint8_t, 256> expTable_;
    std::array<uint8_t, 256> logTable_;
};