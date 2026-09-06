#include "reedsolomon.h"

namespace {
    // Multiplies two polynomials in GF(256). Coefficients are ordered
    // highest-degree-first, same convention used throughout this file.
    std::vector<uint8_t> multiplyPolynomials(const std::vector<uint8_t>& a,
                                              const std::vector<uint8_t>& b,
                                              const GaloisField& gf) {
        std::vector<uint8_t> result(a.size() + b.size() - 1, 0);

        for (size_t i = 0; i < a.size(); ++i) {
            for (size_t j = 0; j < b.size(); ++j) {
                result[i + j] ^= gf.multiply(a[i], b[j]);
            }
        }

        return result;
    }
}

std::vector<uint8_t> buildGeneratorPolynomial(int ecCount, const GaloisField& gf) 
{
    std::vector<uint8_t> generator = {1};

    for (int i = 0; i < ecCount; ++i) 
    {
        std::vector<uint8_t> term = {1, gf.expTable(i)};  // represents (x + alpha^i)
        generator = multiplyPolynomials(generator, term, gf);
    }

    return generator;
}

std::vector<uint8_t> generateECCodewords(const std::vector<uint8_t>& data,
                                           int ecCount,
                                           const GaloisField& gf) {
    std::vector<uint8_t> generator = buildGeneratorPolynomial(ecCount, gf);

    // Message polynomial: data codewords followed by ecCount zero bytes
    std::vector<uint8_t> message = data;
    message.resize(data.size() + ecCount, 0);

    for (size_t i = 0; i < data.size(); ++i) 
    {
        uint8_t coefficient = message[i];
        if (coefficient != 0) {
            for (size_t j = 0; j < generator.size(); ++j) {
                message[i + j] ^= gf.multiply(generator[j], coefficient);
            }
        }
    }

    // The last ecCount bytes of the message are the remainder: the EC codewords
    return std::vector<uint8_t>(message.end() - ecCount, message.end());
}