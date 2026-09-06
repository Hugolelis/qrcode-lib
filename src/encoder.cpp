#include "encoder.h"
#include "bitbuffer.h"
#include <algorithm>

namespace {
    const int V1_L_CAPACITY_BITS = 19 * 8;  // 19 data codewords for Version 1, EC level L

    void addTerminatorAndPadding(BitBuffer& buffer, int capacityBits) {
        // Terminator: up to 4 zero bits, but never exceed the remaining capacity
        int terminatorBits = std::min(4, capacityBits - buffer.size());
        buffer.appendBits(0, terminatorBits);

        // Pad with zero bits until the length is a multiple of 8
        while (buffer.size() % 8 != 0) {
            buffer.appendBits(0, 1);
        }

        // Fill the rest with alternating pad bytes 0xEC and 0x11
        bool useECPad = true;
        while (buffer.size() < capacityBits) {
            buffer.appendBits(useECPad ? 0xEC : 0x11, 8);
            useECPad = !useECPad;
        }
    }
}

BitBuffer encodeByteMode(const std::string& text, int capacityBits) {
    BitBuffer buffer;

    buffer.appendBits(0b0100, 4);
    buffer.appendBits(text.size(), 8);

    for (char c : text) {
        buffer.appendBits(static_cast<uint8_t>(c), 8);
    }

    addTerminatorAndPadding(buffer, capacityBits);

    return buffer;
}