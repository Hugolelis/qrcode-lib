# QrCode

<div align="left">

[![License](https://img.shields.io/badge/License-MIT-1a1a2e?style=for-the-badge&logoColor=white)](LICENSE)
[![C++](https://img.shields.io/badge/C%2B%2B-17-1a1a2e?style=for-the-badge&logo=cplusplus&logoColor=white)]()
[![Build](https://img.shields.io/badge/Build-Make-1a1a2e?style=for-the-badge&logo=gnu&logoColor=white)]()
[![Status](https://img.shields.io/badge/Status-In%20Development-1a1a2e?style=for-the-badge&logoColor=white)]()

</div>

> **QrCode** is a QR Code generator built from scratch in C++, with no external QR or image libraries. It implements the encoding, matrix construction, and rendering pipeline described in ISO/IEC 18004, as a hands-on way to understand how QR codes actually work under the hood.

---

## Table of Contents

- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Build Targets](#build-targets)
- [Configuration](#configuration)
- [Architecture](#architecture)
- [Contributing](#contributing)
- [License](#license)

---

## Features

| Capability | Description |
|---|---|
| **Data Encoding** | Encode input text using Byte mode |
| **Error Correction** | Reed-Solomon error correction across levels L, M, Q, H |
| **Matrix Patterns** | Finder, separator, timing, alignment and dark module patterns |
| **Masking** | All 8 data mask patterns, selected by penalty scoring |
| **Format & Version Info** | Format and version information encoding |
| **Multiple Versions** | Support for QR versions 1 through 40 |
| **Terminal Rendering** | Render the generated matrix directly in the terminal |
| **Zero Dependencies** | Pure C++17, no external QR or image libraries |

---

## Installation

```bash
# Clone the repository
git clone https://github.com/Hugolelis/QrCode.git
cd QrCode

# Build
make
```

**Requirements:** a C++17 compiler (e.g. `g++`) and GNU Make.

---

## Usage

### Build and run

```bash
make run
```

### Library usage

The core pieces — bit encoding and matrix construction — are usable directly:

```cpp
#include "encoder.h"
#include "matrix.h"

int main() {
    // Encode text into a QR-formatted bitstream
    BitBuffer buffer = encodeByteMode("HELLO");

    // Build a matrix and draw its function patterns
    Matrix matrix(21);
    matrix.drawFunctionPatterns();
    matrix.print();
}
```

---

## Build Targets

| Target | Description |
|---|---|
| `make` / `make all` | Build the project into `build/qrcode` |
| `make run` | Build (if needed) and run the executable |
| `make clean` | Remove the `build/` directory |

---

## Configuration

Build behavior is controlled through `makefile` variables:

| Variable | Default | Description |
|---|---|---|
| `CXX` | `g++` | Compiler used to build the project |
| `CXXFLAGS` | `-std=c++17 -Wall -Wextra -Wpedantic -g -Iincludes` | Compiler flags |
| `TARGET` | `build/qrcode` | Output binary path |

---

## Architecture

```
includes/               # Public headers
├── bitbuffer.h
├── encoder.h
└── matrix.h

src/                    # Core QR generation logic (library sources)
├── bitbuffer.cpp       # Bit-level buffer used to assemble encoded data
├── encoder.cpp         # Byte-mode data encoding
└── matrix.cpp          # Module grid and function pattern rendering

bin/
└── main.cpp            # Program entry point

tests/                  # Unit tests (planned)

makefile                # Build configuration
```

The generation pipeline is split into independent stages:

1. **Encoder** — converts input text into a bitstream (`BitBuffer`), following the QR mode/character-count/data format
2. **Matrix** — builds the module grid and places the function patterns (finder, separator, timing, dark module)
3. **Error Correction** *(planned)* — Reed-Solomon codewords appended to the encoded data
4. **Data Placement & Masking** *(planned)* — interleaving data/EC codewords into the matrix and applying the best mask pattern
5. **Format & Version Information** *(planned)* — encoding metadata modules that make the code scannable

---

## Contributing

This is a personal learning project built to understand the QR Code specification (ISO/IEC 18004) from scratch. Suggestions and issues are welcome:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/my-feature`)
3. Commit your changes (`git commit -am 'Add my feature'`)
4. Push to the branch (`git push origin feature/my-feature`)
5. Open a Pull Request

---

## License

Distributed under the **MIT License**. See [LICENSE](LICENSE) for more information.
