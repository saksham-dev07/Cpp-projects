#include "ProgrammerCalc.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cctype>

namespace ProgrammerCalc {

uint64_t BaseEngine::applyWordMask(uint64_t val, WordSize ws) {
    switch (ws) {
        case WordSize::Bit8:  return val & 0xFFULL;
        case WordSize::Bit16: return val & 0xFFFFULL;
        case WordSize::Bit32: return val & 0xFFFFFFFFULL;
        case WordSize::Bit64: return val;
    }
    return val;
}

uint64_t BaseEngine::parseNumber(const std::string& input, Radix defaultRadix) {
    size_t start = input.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) throw ProgrammerCalcError("Empty number string");
    size_t end = input.find_last_not_of(" \t\r\n");
    std::string s = input.substr(start, end - start + 1);

    // Strip visual separators like underscores or spaces (e.g. 0b1111_0000)
    std::string clean;
    for (char c : s) {
        if (c != ' ' && c != '_') clean += c;
    }
    s = clean;

    Radix radix = defaultRadix;
    bool isNegative = false;
    if (!s.empty() && s[0] == '-') {
        isNegative = true;
        s = s.substr(1);
    } else if (!s.empty() && s[0] == '+') {
        s = s.substr(1);
    }

    if (s.empty()) throw ProgrammerCalcError("Invalid number format");

    // Check for base prefixes: 0x (hex), 0b (bin), 0o (oct)
    if (s.length() >= 2 && s[0] == '0') {
        char prefix = static_cast<char>(std::tolower(static_cast<unsigned char>(s[1])));
        if (prefix == 'x') {
            radix = Radix::Hexadecimal;
            s = s.substr(2);
        } else if (prefix == 'b') {
            radix = Radix::Binary;
            s = s.substr(2);
        } else if (prefix == 'o') {
            radix = Radix::Octal;
            s = s.substr(2);
        }
    }

    if (s.empty()) throw ProgrammerCalcError("No digits provided after base prefix");

    uint64_t result = 0;
    int base = static_cast<int>(radix);

    for (char c : s) {
        int digit = -1;
        if (std::isdigit(static_cast<unsigned char>(c))) {
            digit = c - '0';
        } else if (std::isalpha(static_cast<unsigned char>(c))) {
            digit = std::tolower(static_cast<unsigned char>(c)) - 'a' + 10;
        }

        if (digit < 0 || digit >= base) {
            std::ostringstream ss;
            ss << "Invalid digit '" << c << "' for base " << base;
            throw ProgrammerCalcError(ss.str());
        }

        result = result * base + digit;
    }

    if (isNegative) {
        result = static_cast<uint64_t>(-static_cast<int64_t>(result));
    }

    return result;
}

std::string BaseEngine::formatBinaryGrouped(uint64_t val, WordSize ws) {
    val = applyWordMask(val, ws);
    int totalBits = static_cast<int>(ws);
    std::string bin;
    for (int i = totalBits - 1; i >= 0; --i) {
        bin += ((val >> i) & 1ULL) ? '1' : '0';
        if (i > 0 && (i % 4 == 0)) {
            bin += ' ';
        }
    }
    return bin;
}

std::string BaseEngine::formatHexFormatted(uint64_t val, WordSize ws) {
    val = applyWordMask(val, ws);
    int hexDigits = static_cast<int>(ws) / 4;
    std::ostringstream ss;
    ss << "0x" << std::uppercase << std::hex << std::setfill('0') << std::setw(hexDigits) << val;
    return ss.str();
}

int BaseEngine::countSetBits(uint64_t val) {
    int count = 0;
    while (val) {
        val &= (val - 1); // Brian Kernighan's algorithm
        count++;
    }
    return count;
}

RadixRepresentations BaseEngine::getAllRepresentations(uint64_t value, WordSize wordSize) {
    uint64_t masked = applyWordMask(value, wordSize);
    RadixRepresentations rep;

    rep.hex = formatHexFormatted(masked, wordSize);
    rep.bin = formatBinaryGrouped(masked, wordSize);

    std::ostringstream octSS;
    octSS << "0o" << std::oct << masked;
    rep.oct = octSS.str();

    rep.decUnsigned = std::to_string(masked);

    switch (wordSize) {
        case WordSize::Bit8:
            rep.decSigned = std::to_string(static_cast<int8_t>(masked & 0xFF));
            break;
        case WordSize::Bit16:
            rep.decSigned = std::to_string(static_cast<int16_t>(masked & 0xFFFF));
            break;
        case WordSize::Bit32:
            rep.decSigned = std::to_string(static_cast<int32_t>(masked & 0xFFFFFFFF));
            break;
        case WordSize::Bit64:
            rep.decSigned = std::to_string(static_cast<int64_t>(masked));
            break;
    }

    // ASCII representation byte-by-byte
    std::string asciiStr;
    int bytes = static_cast<int>(wordSize) / 8;
    for (int i = bytes - 1; i >= 0; --i) {
        uint8_t byteVal = static_cast<uint8_t>((masked >> (i * 8)) & 0xFF);
        if (byteVal >= 32 && byteVal <= 126) {
            asciiStr += static_cast<char>(byteVal);
        } else {
            asciiStr += '.';
        }
    }
    rep.ascii = "'" + asciiStr + "'";
    rep.popCount = countSetBits(masked);

    return rep;
}

std::string BaseEngine::convertBase(const std::string& input, Radix fromRadix, Radix toRadix, WordSize wordSize) {
    uint64_t val = parseNumber(input, fromRadix);
    val = applyWordMask(val, wordSize);

    switch (toRadix) {
        case Radix::Hexadecimal:
            return formatHexFormatted(val, wordSize);
        case Radix::Binary:
            return formatBinaryGrouped(val, wordSize);
        case Radix::Octal: {
            std::ostringstream ss;
            ss << std::oct << val;
            return ss.str();
        }
        case Radix::Decimal:
            return std::to_string(val);
    }
    return std::to_string(val);
}

uint64_t BaseEngine::bitAnd(uint64_t a, uint64_t b, WordSize ws) {
    return applyWordMask(a & b, ws);
}

uint64_t BaseEngine::bitOr(uint64_t a, uint64_t b, WordSize ws) {
    return applyWordMask(a | b, ws);
}

uint64_t BaseEngine::bitXor(uint64_t a, uint64_t b, WordSize ws) {
    return applyWordMask(a ^ b, ws);
}

uint64_t BaseEngine::bitNot(uint64_t a, WordSize ws) {
    return applyWordMask(~a, ws);
}

uint64_t BaseEngine::bitNand(uint64_t a, uint64_t b, WordSize ws) {
    return applyWordMask(~(a & b), ws);
}

uint64_t BaseEngine::bitNor(uint64_t a, uint64_t b, WordSize ws) {
    return applyWordMask(~(a | b), ws);
}

uint64_t BaseEngine::shiftLeft(uint64_t a, int shift, WordSize ws) {
    if (shift < 0 || shift >= static_cast<int>(ws)) return 0;
    return applyWordMask(a << shift, ws);
}

uint64_t BaseEngine::shiftRightLogical(uint64_t a, int shift, WordSize ws) {
    uint64_t masked = applyWordMask(a, ws);
    if (shift < 0 || shift >= static_cast<int>(ws)) return 0;
    return masked >> shift;
}

uint64_t BaseEngine::shiftRightArithmetic(uint64_t a, int shift, WordSize ws) {
    if (shift < 0 || shift >= static_cast<int>(ws)) shift = static_cast<int>(ws) - 1;
    uint64_t masked = applyWordMask(a, ws);
    bool sign = (masked >> (static_cast<int>(ws) - 1)) & 1ULL;
    uint64_t res = masked >> shift;
    if (sign) {
        uint64_t fillMask = (~0ULL) << (static_cast<int>(ws) - shift);
        res |= fillMask;
    }
    return applyWordMask(res, ws);
}

uint64_t BaseEngine::rotateLeft(uint64_t a, int shift, WordSize ws) {
    int bits = static_cast<int>(ws);
    shift = ((shift % bits) + bits) % bits;
    if (shift == 0) return applyWordMask(a, ws);
    uint64_t masked = applyWordMask(a, ws);
    uint64_t res = (masked << shift) | (masked >> (bits - shift));
    return applyWordMask(res, ws);
}

uint64_t BaseEngine::rotateRight(uint64_t a, int shift, WordSize ws) {
    int bits = static_cast<int>(ws);
    shift = ((shift % bits) + bits) % bits;
    if (shift == 0) return applyWordMask(a, ws);
    uint64_t masked = applyWordMask(a, ws);
    uint64_t res = (masked >> shift) | (masked << (bits - shift));
    return applyWordMask(res, ws);
}

Radix BaseEngine::parseRadixName(const std::string& name) {
    std::string s = name;
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    if (s == "hex" || s == "hexadecimal" || s == "16" || s == "h" || s == "0x") return Radix::Hexadecimal;
    if (s == "bin" || s == "binary" || s == "2" || s == "b" || s == "0b") return Radix::Binary;
    if (s == "oct" || s == "octal" || s == "8" || s == "o" || s == "0o") return Radix::Octal;
    if (s == "dec" || s == "decimal" || s == "10" || s == "d") return Radix::Decimal;
    throw ProgrammerCalcError("Unknown base '" + name + "'. Options: hex, dec, bin, oct");
}

std::string BaseEngine::getBitGrid(uint64_t val, WordSize ws) {
    val = applyWordMask(val, ws);
    int bits = static_cast<int>(ws);
    std::ostringstream oss;

    oss << "Bit: ";
    for (int i = bits - 1; i >= 0; --i) {
        if (i % 8 == 7) {
            oss << std::setw(2) << i << " ";
        } else if (i % 8 == 0) {
            oss << std::setw(2) << i << "  ";
        }
    }
    oss << "\nVal: ";
    for (int i = bits - 1; i >= 0; --i) {
        oss << (((val >> i) & 1ULL) ? '1' : '0');
        if (i > 0 && i % 4 == 0) oss << " ";
        if (i > 0 && i % 8 == 0) oss << " ";
    }
    return oss.str();
}

} // namespace ProgrammerCalc
