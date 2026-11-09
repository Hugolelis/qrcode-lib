# QrCode

<div align="left">

[![Status](https://img.shields.io/badge/Status-In_Development-a16207?style=for-the-badge&labelColor=1a1a1a&logoColor=white)]()
[![Version](https://img.shields.io/badge/Version-0.1.0-0f766e?style=for-the-badge&labelColor=1a1a1a&logoColor=white)]()
[![License](https://img.shields.io/badge/License-MIT-0f766e?style=for-the-badge&labelColor=1a1a1a&logoColor=white)](LICENSE)

</div>

> **QrCode** is a QR Code generator built from scratch in C++17, with no external QR or image libraries. It implements the pipeline described in ISO/IEC 18004 (byte-mode encoding, Reed-Solomon error correction, matrix construction, data placement, masking and format information) as a hands-on way to understand how QR codes actually work under the hood.

<details>
<summary><strong>Table of Contents</strong></summary>

- [About](#about)
- [Demo](#demo)
- [Features](#features)
- [Tech Stack](#tech-stack)
- [Architecture & Design Decisions](#architecture--design-decisions)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Public API](#public-api)
- [Output](#output)
- [Build Targets](#build-targets)
- [Configuration](#configuration)
- [Author](#author)
- [License](#license)

</details>

---

## About

Most QR code generators are a call to a library that hides the actual algorithm. This one implements ISO/IEC 18004 directly (bit encoding, Galois-field arithmetic for Reed-Solomon, module matrix, function patterns, data placement, format information) with zero external QR or image dependencies, as a way to actually learn the spec instead of trusting a black box.

The generated codes are real, scannable QR codes: the output of `make run` decodes back to the original text with a standard decoder. What is still missing is generality (more versions, error-correction levels and mask selection), listed under [Known limitations](#architecture--design-decisions).

---

## Demo

```bash
make run
```

The program encodes a URL and prints the matrix to the terminal (each module is two characters wide, with the 4-module quiet zone around it), then writes `output/qrcode.pbm`:

```
        ██████████████      ██      ██        ██    ██████████████
        ██          ██    ██      ██        ████    ██          ██
        ██  ██████  ██  ██    ██      ██    ████    ██  ██████  ██
        ██  ██████  ██    ██    ████    ██  ██      ██  ██████  ██
        ██  ██████  ██    ██    ████    ██  ██████  ██  ██████  ██
        ██          ██      ██████  ████████████    ██          ██
        ██████████████  ██  ██  ██  ██  ██  ██  ██  ██████████████
                        ██    ██      ██  ████  ██
        ██████  ██████████      ██      ████      ████      ██
        ████████  ██  ██  ██  ██████  ████████  ██████    ██    ██
        ██  ████████████    ██████  ██████████    ██  ██    ██████
        ████  ██        ██████  ██████  ████  ██████    ██    ██
        ██  ██████  ██████  ████    ████████    ██████    ██  ████
          ██████  ██  ██████████    ████  ██    ██████    ██    ██
          ████  ██  ██████        ██      ████  ████████  ██  ████
              ██████  ██████████        ██████████        ██  ██
          ████      ████  ████  ██    ██████    ██████    ██  ████
          ██  ██  ██  ████    ██████      ██    ██████    ████  ██
        ██    ██    ██      ██████  ██  ██        ██  ████    ████
          ██████████  ██  ████  ████████  ██████████      ██  ██
        ██      ██  ████  ██████    ██    ██  ████████████
                        ████  ██    ██  ██    ████      ██  ██████
        ██████████████  ██  ██    ██    ██    ████  ██  ████  ████
        ██          ██  ████  ██      ██  ██  ████      ████
        ██  ██████  ██  ██████  ██        ████  ██████████      ██
        ██  ██████  ██    ██████████    ██      ██      ██  ██████
        ██  ██████  ██  ██    ████  ██    ██    ██    ██████    ██
        ██          ██  ██████  ████████  ██    ████████      ██
        ██████████████  ████  ██    ██  ████  ████████        ████
```

Convert the `.pbm` to PNG (see [Output](#output)) and scan it with a phone to get the encoded URL back.

---

## Features

| Capability | Description |
|---|---|
| **Byte-mode encoding** | Mode indicator, 8-bit character count, data, terminator and `0xEC`/`0x11` padding up to the capacity you pass in |
| **Reed-Solomon error correction** | GF(256) arithmetic and generator polynomial implemented from scratch; EC codewords are appended to the data codewords |
| **Function patterns** | Finder, separator, timing and alignment patterns plus the dark module |
| **Data placement** | Zig-zag placement of data and EC codewords, skipping the timing column and reserved areas |
| **Masking** | Mask pattern 0, applied only to data modules |
| **Format information** | 15-bit BCH(15,5) code with the spec's XOR mask, written to both copies around the finder patterns |
| **Terminal rendering** | Prints the matrix with a quiet zone using block characters |
| **PBM export** | Writes a plain-text `.pbm` image (10x10 pixels per module) with no image library |
| **Zero dependencies** | Pure C++17, standard library only |

---

## Tech Stack

- **Language:** C++17
- **Build system:** GNU Make
- **Dependencies:** none (standard library only, including `<filesystem>`)
- **Tests:** plain `assert`-based runner, no framework

---

## Architecture & Design Decisions

```
.
├── includes/               # Public headers
│   ├── bitbuffer.h         # Bit-level buffer
│   ├── encoder.h           # Byte-mode encoding
│   ├── formatinfo.h        # Format information bits
│   ├── galoisfield.h       # GF(256) arithmetic
│   ├── matrix.h            # Module grid, patterns, placement and masking
│   ├── output.h            # Terminal and PBM rendering
│   └── reedsolomon.h       # Generator polynomial and EC codewords
├── src/                    # Library sources (one .cpp per header)
├── bin/
│   └── main.cpp            # Example program wiring the pipeline together
├── tests/
│   └── test_runner.cpp     # Unit tests (make test)
├── makefile
└── LICENSE
```

The generation pipeline is split into independent stages, each in its own translation unit:

1. **Encoder**: converts input text into a bitstream (`BitBuffer`) following the QR byte-mode format, padded to the data capacity
2. **Reed-Solomon**: computes the error-correction codewords over GF(256) and appends them to the data
3. **Matrix**: builds the module grid, draws the function patterns and reserves the format-information area
4. **Data placement**: walks the matrix in the spec's zig-zag order and writes the data and EC bits into the free modules
5. **Masking**: flips data modules according to the mask pattern (function patterns are never touched)
6. **Format information**: computes the BCH-protected format bits for the EC level and mask, and writes them into the reserved area

**Why this shape:** headers under `includes/` and implementation under `src/` keep the library reusable on its own: `bin/main.cpp` only wires the pieces together, so the encoding and matrix logic can be `#include`d by another program without pulling in a CLI. Each stage is its own translation unit, so it can be tested and reasoned about independently (the test runner compiles `src/` plus `tests/test_runner.cpp`, without `main.cpp`). The capacity (data codewords) and the number of EC codewords are passed in by the caller instead of being looked up from a version table, which keeps the stages simple while the spec tables are not implemented yet.

**Known limitations:**
- Only error-correction level L has been exercised, with the capacity and EC codeword count supplied by the caller (there is no version/level capacity table yet). Format bits for other levels can be computed, but nothing validates the matching capacity.
- Only single-block layouts work. Versions 1 to 5 at level L were checked and decode correctly; from version 6 the data is split into several Reed-Solomon blocks that must be interleaved, which is not implemented.
- Only mask pattern 0 is applied. The other 7 patterns and the penalty-score selection are not implemented, so codes may be harder to scan than necessary on some inputs.
- Alignment patterns are drawn for versions 2 to 6 only (a single pattern), and version information (versions 7+) is not implemented.
- Byte mode only, and the character count is a single byte, so the input is limited to 255 characters.
- The input text, version and capacity are hard-coded in `bin/main.cpp` (V3-L, 55 data + 15 EC codewords); there is no command-line interface yet.
- The test suite covers the building blocks (bit buffer, encoder, Galois field, Reed-Solomon, format bits and their placement) but not a full encode-and-decode round trip.

---

## Getting Started

### Prerequisites

- A C++17 compiler with `<filesystem>` support (e.g. `g++` 8 or newer)
- GNU Make

### Installation

```bash
git clone https://github.com/Hugolelis/qrcode-lib.git
cd qrcode-lib
make
```

---

## Usage

### Build and run

```bash
make run
```

This builds `build/qrcode`, prints the QR code to the terminal and writes `output/qrcode.pbm`. Run it from the repository root, since the output path is relative.

To encode something else, change the text and the version parameters in `bin/main.cpp`.

### Run the tests

```bash
make test
```

### Library usage

The same pipeline `bin/main.cpp` uses, for a version 3, level L code (55 data codewords, 15 EC codewords):

```cpp
#include "matrix.h"
#include "encoder.h"
#include "galoisfield.h"
#include "reedsolomon.h"
#include "formatinfo.h"
#include "output.h"

int main() {
    Matrix matrix(3);                      // version 3 (29x29 modules)
    matrix.drawFunctionPatterns();
    matrix.reserveFormatInfoArea();

    GaloisField gf;
    BitBuffer buffer = encodeByteMode("HELLO", 55 * 8);               // pad to 55 data codewords
    std::vector<uint8_t> data = buffer.toBytes();
    std::vector<uint8_t> ec = generateECCodewords(data, 15, gf);      // 15 EC codewords

    std::vector<uint8_t> codewords = data;
    codewords.insert(codewords.end(), ec.begin(), ec.end());

    matrix.placeData(codewords);
    matrix.applyMask0();
    matrix.placeFormatBits(computeFormatBits(0b01, 0));               // EC level L, mask 0

    printToTerminal(matrix);
    writePBM(matrix, "output/qrcode.pbm");
}
```

---

## Public API

| Function / class | Header | Description |
|---|---|---|
| `encodeByteMode(text, capacityBits)` | `encoder.h` | Encodes text in byte mode and pads it to `capacityBits` |
| `BitBuffer` | `bitbuffer.h` | `appendBits`, `size`, `at`, `toBytes` |
| `GaloisField` | `galoisfield.h` | GF(256) `expTable`, `logTable` and `multiply` |
| `buildGeneratorPolynomial(ecCount, gf)` | `reedsolomon.h` | Reed-Solomon generator polynomial |
| `generateECCodewords(data, ecCount, gf)` | `reedsolomon.h` | EC codewords for a data block |
| `Matrix(version)` | `matrix.h` | Module grid of size `4 * version + 17` |
| `Matrix::drawFunctionPatterns()` | `matrix.h` | Finders, separators, timing, dark module and alignment pattern |
| `Matrix::reserveFormatInfoArea()` | `matrix.h` | Marks the format-information modules as reserved |
| `Matrix::placeData(codewords)` | `matrix.h` | Zig-zag placement of data and EC codewords |
| `Matrix::applyMask0()` | `matrix.h` | Applies mask pattern 0 to data modules |
| `Matrix::placeFormatBits(bits)` | `matrix.h` | Writes the 15 format bits to both locations |
| `computeFormatBits(ecLevelBits, mask)` | `formatinfo.h` | BCH-encoded and masked format bits (`0b01` = level L) |
| `printToTerminal(matrix)` | `output.h` | Prints the matrix with a quiet zone |
| `writePBM(matrix, filename)` | `output.h` | Writes a `.pbm` image, creating parent folders |

---

## Output

Running the program writes a `.pbm` file to `output/qrcode.pbm`, a plain-text image format that needs no compression, so it can be written without any external library. To view it or scan it with a phone, convert it to PNG:

```bash
# Linux / macOS
convert output/qrcode.pbm output/qrcode.png

# Windows (ImageMagick 7+)
magick output/qrcode.pbm output/qrcode.png
```

**Important:** don't pass `-resize`/`-scale` with a target size. The program already renders each module as a solid `10x10` pixel block (`MODULE_SCALE` in `src/output.cpp`), so the `.pbm` is already full-size. Resizing to a size that isn't an exact multiple of the original introduces blur at the module edges, which breaks scanning.

---

## Build Targets

| Target | Description |
|---|---|
| `make` / `make all` | Build the project into `build/qrcode` |
| `make run` | Build (if needed) and run the executable |
| `make test` | Build and run the unit tests (`build/tests`) |
| `make clean` | Remove the `build/` directory |

---

## Configuration

Build behavior is controlled through `makefile` variables:

| Variable | Default | Description |
|---|---|---|
| `CXX` | `g++` | Compiler used to build the project |
| `CXXFLAGS` | `-std=c++17 -Wall -Wextra -Wpedantic -g -Iincludes` | Compiler flags |
| `TARGET` | `build/qrcode` | Output binary path |
| `TEST_TARGET` | `build/tests` | Test binary path |

---

## Author

[Hugolelis](https://github.com/Hugolelis)

---

## License

Distributed under the **MIT License**. See [LICENSE](LICENSE) for more information.
