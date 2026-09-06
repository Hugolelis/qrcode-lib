#include "galoisfield.h"

namespace {
    const int PRIMITIVE_POLYNOMIAL = 0x11D;
}

GaloisField::GaloisField() 
{
    int x = 1;
    for (int i = 0; i < 255; ++i) 
    {
        expTable_[i] = static_cast<uint8_t>(x);
        logTable_[x] = static_cast<uint8_t>(i);

        x <<= 1;
        if (x & 0x100) // overflowed past 8 bits
        {          
            x ^= PRIMITIVE_POLYNOMIAL;
        }
    }
}

uint8_t GaloisField::expTable(int index) const 
{
    return expTable_[index];
}

uint8_t GaloisField::logTable(int value) const 
{
    return logTable_[value];
}

uint8_t GaloisField::multiply(uint8_t a, uint8_t b) const 
{
    if (a == 0 || b == 0) 
    {
        return 0;
    }

    int logSum = logTable_[a] + logTable_[b];
    return expTable_[logSum % 255];
}