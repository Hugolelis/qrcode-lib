#include <iostream>
#include <iomanip>
#include "encoder.h"

int main() {
    BitBuffer buffer = encodeByteMode("HELLO");
    std::vector<uint8_t> codewords = buffer.toBytes();

    std::cout << "Total codewords: " << codewords.size() << "\n";
    for (uint8_t byte : codewords) {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                   << static_cast<int>(byte) << ' ';
    }
    std::cout << "\n";
    return 0;
}