#include <iostream>
#include "galoisfield.h"
#include "reedsolomon.h"
#include "encoder.h"

int main() {
    GaloisField gf;
    BitBuffer buffer = encodeByteMode("HELLO");
    std::vector<uint8_t> data = buffer.toBytes();

    std::vector<uint8_t> ecCodewords = generateECCodewords(data, 7, gf);

    std::cout << "EC codewords: ";
    for (uint8_t byte : ecCodewords) {
        std::cout << std::hex << (int)byte << " ";
    }
    std::cout << "\n";

    // Self-check: dividing (data + EC codewords) by the generator must leave remainder 0
    std::vector<uint8_t> fullCodeword = data;
    fullCodeword.insert(fullCodeword.end(), ecCodewords.begin(), ecCodewords.end());
    std::vector<uint8_t> remainder = generateECCodewords(fullCodeword, 7, gf);

    bool allZero = true;
    for (uint8_t byte : remainder) {
        if (byte != 0) allZero = false;
    }
    std::cout << "Self-check remainder: ";
    for (uint8_t byte : remainder) std::cout << (int)byte << " ";
    std::cout << (allZero ? "-> OK\n" : "-> FAILED\n");

    return 0;
}