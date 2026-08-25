#pragma once

#include <string>
#include <cstdint>
#include <vector>
#include <stdexcept>

namespace ProgrammerCalc {

class ProgrammerCalcError : public std::runtime_error {
public:
    explicit ProgrammerCalcError(const std::string& msg) : std::runtime_error(msg) {}
};

enum class Radix {
    Binary      = 2,
    Octal       = 8,
    Decimal     = 10,
    Hexadecimal = 16
};

enum class WordSize {
    Bit8  = 8,
    Bit16 = 16,
    Bit32 = 32,
    Bit64 = 64
};

struct RadixRepresentations {
    std::string hex;
    std::string decSigned;
    std::string decUnsigned;
    std::string oct;
    std::string bin;
    std::string ascii;
    int popCount = 0;
};

class BaseEngine {
public:
    // Parses string in hex (0x...), binary (0b...), octal (0o...), or decimal
    static uint64_t parseNumber(const std::string& input, Radix defaultRadix = Radix::Decimal);

    // Returns all 4 radix strings + ascii + bit count for display
    static RadixRepresentations getAllRepresentations(uint64_t value, WordSize wordSize = WordSize::Bit64);

    static std::string convertBase(const std::string& input, Radix fromRadix, Radix toRadix, WordSize wordSize = WordSize::Bit64);

    // 64-bit Bitwise Operations with word-size masking
    static uint64_t bitAnd(uint64_t a, uint64_t b, WordSize ws = WordSize::Bit64);
    static uint64_t bitOr(uint64_t a, uint64_t b, WordSize ws = WordSize::Bit64);
    static uint64_t bitXor(uint64_t a, uint64_t b, WordSize ws = WordSize::Bit64);
    static uint64_t bitNot(uint64_t a, WordSize ws = WordSize::Bit64);
    static uint64_t bitNand(uint64_t a, uint64_t b, WordSize ws = WordSize::Bit64);
    static uint64_t bitNor(uint64_t a, uint64_t b, WordSize ws = WordSize::Bit64);
    static uint64_t shiftLeft(uint64_t a, int shift, WordSize ws = WordSize::Bit64);
    static uint64_t shiftRightLogical(uint64_t a, int shift, WordSize ws = WordSize::Bit64);
    static uint64_t shiftRightArithmetic(uint64_t a, int shift, WordSize ws = WordSize::Bit64);
    static uint64_t rotateLeft(uint64_t a, int shift, WordSize ws = WordSize::Bit64);
    static uint64_t rotateRight(uint64_t a, int shift, WordSize ws = WordSize::Bit64);

    static uint64_t applyWordMask(uint64_t val, WordSize ws);
    static std::string formatBinaryGrouped(uint64_t val, WordSize ws);
    static std::string formatHexFormatted(uint64_t val, WordSize ws);
    static int countSetBits(uint64_t val);
    static std::string getBitGrid(uint64_t val, WordSize ws);
    static Radix parseRadixName(const std::string& name);
};

} // namespace ProgrammerCalc
