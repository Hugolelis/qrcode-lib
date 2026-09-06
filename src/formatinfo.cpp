#include "formatinfo.h"

namespace {
    const uint32_t BCH_GENERATOR = 0x537;   // x^10 + x^8 + x^5 + x^4 + x^2 + x + 1
    const uint16_t FORMAT_MASK   = 0x5412;  // fixed XOR mask required by the spec
}

uint16_t computeFormatBits(int ecLevelBits, int maskPattern) 
{
    uint32_t data = (static_cast<uint32_t>(ecLevelBits) << 3) | maskPattern;  // 5 bits total

    // BCH(15,5) remainder via polynomial long division in GF(2)
    uint32_t value = data << 10;  // 15-bit value: 5 data bits followed by 10 zero bits
    for (int i = 14; i >= 10; --i) 
    {
        if (value & (1u << i)) 
        {
            value ^= (BCH_GENERATOR << (i - 10));
        }
    }

    uint16_t formatBits = static_cast<uint16_t>((data << 10) | value);
    formatBits ^= FORMAT_MASK;

    return formatBits;
}