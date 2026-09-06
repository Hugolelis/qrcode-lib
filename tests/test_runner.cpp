#include <cassert>
#include <iostream>
#include "bitbuffer.h"
#include "encoder.h"
#include "galoisfield.h"
#include "reedsolomon.h"
#include "formatinfo.h"

void testBitBuffer() {
    BitBuffer buffer;
    buffer.appendBits(0b0100, 4);  // mode indicator: Byte mode
    assert(buffer.size() == 4);
    assert(buffer.at(0) == false);
    assert(buffer.at(1) == true);
    assert(buffer.at(2) == false);
    assert(buffer.at(3) == false);
    std::cout << "testBitBuffer OK\n";
}

void testEncodeByteMode() {
    BitBuffer buffer = encodeByteMode("HELLO");
    assert(buffer.size() == 152);  // V1-L capacity
    std::cout << "testEncodeByteMode OK\n";
}

void testGaloisField() {
    GaloisField gf;
    assert(gf.multiply(1, 1) == 1);
    assert(gf.multiply(0, 5) == 0);
    assert(gf.multiply(29, 2) == gf.expTable(9));  // self-consistency with the table
    std::cout << "testGaloisField OK\n";
}

void testReedSolomon() {
    GaloisField gf;
    BitBuffer buffer = encodeByteMode("HELLO");
    std::vector<uint8_t> data = buffer.toBytes();
    std::vector<uint8_t> ec = generateECCodewords(data, 7, gf);

    // A valid codeword, divided by the generator, must leave remainder 0
    std::vector<uint8_t> fullCodeword = data;
    fullCodeword.insert(fullCodeword.end(), ec.begin(), ec.end());
    std::vector<uint8_t> remainder = generateECCodewords(fullCodeword, 7, gf);

    for (uint8_t byte : remainder) {
        assert(byte == 0);
    }
    std::cout << "testReedSolomon OK\n";
}

void testFormatInfo() {
    uint16_t formatBits = computeFormatBits(0b01, 0);  // EC level L, mask 0
    assert(formatBits == 0b111011111000100);
    std::cout << "testFormatInfo OK\n";
}

int main() {
    testBitBuffer();
    testEncodeByteMode();
    testGaloisField();
    testReedSolomon();
    testFormatInfo();
    std::cout << "All tests passed!\n";
    return 0;
}