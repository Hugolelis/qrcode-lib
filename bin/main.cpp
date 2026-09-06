#include "matrix.h"
#include "encoder.h"
#include "galoisfield.h"
#include "reedsolomon.h"

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
    matrix.print();

    return 0;
}