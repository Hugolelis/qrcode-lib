#include <cassert>
#include <iostream>
#include "bitbuffer.h"
#include "encoder.h"
#include "galoisfield.h"
#include "reedsolomon.h"
#include "formatinfo.h"
#include "matrix.h"

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
    BitBuffer buffer = encodeByteMode("HELLO", 19 * 8);  // V1-L capacity
    assert(buffer.size() == 152);
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
    BitBuffer buffer = encodeByteMode("HELLO", 19 * 8);  // V1-L capacity
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

void testPlaceFormatBits() {
    // Regression test for the bit-order bug found while debugging real-world scans:
    // bit i must go to specific fixed positions, verified against a spec-compliant
    // reference implementation (not just re-derived from our own formula).
    Matrix matrix(1);  // Version 1, size 21
    uint16_t formatBits = 0b111011111000100;  // EC level L, mask 0
    matrix.placeFormatBits(formatBits);

    assert(matrix.get(0, 8) == Module::LIGHT);   // bit0 = 0
    assert(matrix.get(7, 8) == Module::DARK);    // bit6 = 1
    assert(matrix.get(8, 8) == Module::DARK);    // bit7 = 1
    assert(matrix.get(14, 8) == Module::DARK);   // bit8 = 1
    assert(matrix.get(20, 8) == Module::DARK);   // bit14 = 1
    assert(matrix.get(8, 20) == Module::LIGHT);  // bit0 = 0
    assert(matrix.get(8, 7) == Module::DARK);    // bit8 = 1
    assert(matrix.get(8, 0) == Module::DARK);    // bit14 = 1

    std::cout << "testPlaceFormatBits OK\n";
}

int main() {
    testBitBuffer();
    testEncodeByteMode();
    testGaloisField();
    testReedSolomon();
    testFormatInfo();
    testPlaceFormatBits();
    std::cout << "All tests passed!\n";
    return 0;
}