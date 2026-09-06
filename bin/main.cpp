#include "matrix.h"
#include "encoder.h"
#include "galoisfield.h"
#include "reedsolomon.h"
#include "formatinfo.h"

int main() {
    Matrix matrix(21);
    matrix.drawFunctionPatterns();
    matrix.reserveFormatInfoArea();

    GaloisField gf;
    BitBuffer buffer = encodeByteMode("HELLO");
    std::vector<uint8_t> data = buffer.toBytes();
    std::vector<uint8_t> ecCodewords = generateECCodewords(data, 7, gf);

    std::vector<uint8_t> allCodewords = data;
    allCodewords.insert(allCodewords.end(), ecCodewords.begin(), ecCodewords.end());

    matrix.placeData(allCodewords);
    matrix.applyMask0();

    uint16_t formatBits = computeFormatBits(0b01, 0);  // EC level L, mask 0
    matrix.placeFormatBits(formatBits);

    matrix.print();

    return 0;
}