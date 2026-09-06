#pragma once
#include <vector>
#include <cstdint>

class BitBuffer 
{
    public:
        void appendBits(uint32_t value, int numBIts);
        int size() const;
        bool at(int index) const;
        std::vector<uint8_t> toBytes() const;

        private:
            std::vector<bool> bits_;
};