# QrCode

<div align="left">

[![License](https://img.shields.io/badge/License-MIT-1a1a2e?style=for-the-badge&logoColor=white)](LICENSE)
[![C++](https://img.shields.io/badge/C%2B%2B-17-1a1a2e?style=for-the-badge&logo=cplusplus&logoColor=white)]()
[![Build](https://img.shields.io/badge/Build-Make-1a1a2e?style=for-the-badge&logo=gnu&logoColor=white)]()

</div>

> **QrCode** is a QR Code generator built from scratch in C++, with no external QR or image libraries. It implements the encoding, matrix construction, and rendering pipeline described in ISO/IEC 18004, as a hands-on way to understand how QR codes actually work under the hood.

<details>
<summary><strong>Table of Contents</strong></summary>

- [About](#about)
- [Features](#features)
- [Tech Stack](#tech-stack)
- [Architecture & Design Decisions](#architecture--design-decisions)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Output](#output)
- [Build Targets](#build-targets)
- [Configuration](#configuration)
- [License](#license)
- [Author](#author)

</details>

---

## About

Most QR code generators are a call to a library that hides the actual algorithm. This one implements ISO/IEC 18004 directly (bit encoding, module matrix, function patterns) with zero external QR or image dependencies, as a way to actually learn the spec instead of trusting a black box.

<!-- Adjust to your actual motivation, draft based on the tagline. -->

---

## Features

| Capability | Description |
|---|---|
| **Data Encoding** | Encode input text using Byte mode |
| **Matrix Patterns** | Finder, separator, timing, alignment and dark module patterns |
| **Terminal Rendering** | Render the generated matrix directly in the terminal |
| **Zero Dependencies** | Pure C++17, no external QR or image libraries |
| **Error Correction** | Reed-Solomon error correction across levels L, M, Q, H (*planned, not yet implemented*) |
| **Masking** | All 8 data mask patterns, selected by penalty scoring (*planned, not yet implemented*) |
| **Format & Version Info** | Format and version information encoding (*planned, not yet implemented*) |
| **Multiple Versions** | Support for QR versions 1 through 40 (*planned; only version 1 is wired up today*) |

<!--
  Heads up: your Architecture section below marks Error Correction, Masking, and
  Format & Version Info as "(planned)", but the Features table listed them as
  if already shipped. Without those three stages a generated code isn't
  actually spec-compliant or reliably scannable yet, so I annotated them here
  to match reality. If you'd rather track this as a full Roadmap section
  instead of inline notes, say the word, I know you generally don't want
  Roadmap sections, but this repo has real unfinished pipeline stages, which
  is a different situation than the others.
-->

---

## Tech Stack

- **Language:** C++17
- **Build system:** GNU Make
- **Dependencies:** none (standard library only)

---

## Architecture & Design Decisions

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

1. **Encoder**: converts input text into a bitstream (`BitBuffer`), following the QR mode/character-count/data format
2. **Matrix**: builds the module grid and places the function patterns (finder, separator, timing, dark module)
3. **Error Correction**: Reed-Solomon codewords appended to the encoded data
4. **Data Placement & Masking**: interleaving data/EC codewords into the matrix and applying the best mask pattern
5. **Format & Version Information**: encoding metadata modules that make the code scannable

**Why this shape:** headers under `includes/` and implementation under `src/` keep the library reusable on its own: `bin/main.cpp` only wires the pieces together, so the encoding/matrix logic can be `#include`d by another program without pulling in a CLI. Each pipeline stage (encoder, matrix, error correction, masking) is its own translation unit so a stage can be tested and reasoned about independently of the others.

**Known limitations:**
- Without error correction, masking, and format/version info, generated codes aren't spec-compliant yet and may not scan reliably on a real device.
- Only version 1 (21×21) is exercised today: larger versions need the alignment-pattern placement logic that scales with version.
- No test suite yet (`tests/` is a placeholder).

<!-- Adjust "Why this shape" and "Known limitations" to match your actual reasoning, draft based on the code structure. -->

---

## Getting Started

### Prerequisites

- A C++17 compiler (e.g. `g++`)
- GNU Make

### Installation

```bash
git clone https://github.com/Hugolelis/QrCode.git
cd QrCode
make
```

---

## Usage

### Build and run

```bash
make run
```

### Library usage

The core pieces (bit encoding and matrix construction) are usable directly:

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

## Output

Running the program writes a `.pbm` file to `output/qrcode.pbm`, a plain-text image format that needs no compression, so it can be written without any external library. To view it or scan it with a phone, convert it to PNG:

```bash
# Linux / macOS
convert output/qrcode.pbm output/qrcode.png

# Windows (ImageMagick 7+)
magick output/qrcode.pbm output/qrcode.png
```

**Important:** don't pass `-resize`/`-scale` with a target size. The program already renders each module as a solid `10x10` pixel block (`MODULE_SCALE` in `output.cpp`), so the `.pbm` is already full-size. Resizing to a size that isn't an exact multiple of the original introduces blur at the module edges, which breaks scanning.

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

## License

Distributed under the **MIT License**. See [LICENSE](LICENSE) for more information.

---

## Author

[Hugolelis](https://github.com/Hugolelis)
