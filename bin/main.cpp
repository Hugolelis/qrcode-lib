#include "matrix.h"
#include "encoder.h"
#include "galoisfield.h"
#include "reedsolomon.h"
#include "formatinfo.h"
#include "output.h"

int main() {
    Matrix matrix(3);  // Version 3
    matrix.drawFunctionPatterns();
    matrix.reserveFormatInfoArea();

    GaloisField gf;
    BitBuffer buffer = encodeByteMode("https://hugolelis-dev.vercel.app/", 55 * 8);  // V3-L capacity
    std::vector<uint8_t> data = buffer.toBytes();
    std::vector<uint8_t> ecCodewords = generateECCodewords(data, 15, gf);  // V3-L: 15 EC codewords

    std::vector<uint8_t> allCodewords = data;
    allCodewords.insert(allCodewords.end(), ecCodewords.begin(), ecCodewords.end());

    matrix.placeData(allCodewords);
    matrix.applyMask0();

    uint16_t formatBits = computeFormatBits(0b01, 0);
    matrix.placeFormatBits(formatBits);

    printToTerminal(matrix);
    writePBM(matrix, "output/qrcode.pbm");

    return 0;
}