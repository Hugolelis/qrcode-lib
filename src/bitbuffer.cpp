#include "bitbuffer.h"

void BitBuffer::appendBits(uint32_t value, int numBits)
{
    // Walk from the most significant bit down to the least significant one
    for (int i = numBits - 1; i >= 0; --i)
    {
        bool bit = (value >> i) & 1;
        bits_.push_back(bit);
    }
}

int BitBuffer::size() const 
{
    return static_cast<int>(bits_.size());
}

bool BitBuffer::at(int index) const 
{
    return bits_[index];
}

std::vector<uint8_t> BitBuffer::toBytes() const {
    std::vector<uint8_t> bytes;

    for (size_t i = 0; i < bits_.size(); i += 8) {
        uint8_t byte = 0;
        for (int j = 0; j < 8; ++j) {
            byte = (byte << 1) | bits_[i + j];  // shift left and insert the next bit
        }
        bytes.push_back(byte);
    }

    return bytes;
}