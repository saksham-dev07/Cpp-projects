#include <iostream>
#include <cmath>
#include <string>
#include <vector>

#include "../src/Calculator.hpp"
#include "../src/UnitConverter.hpp"
#include "../src/ProgrammerCalc.hpp"
#include "../src/FinanceHealthCalc.hpp"

static int g_passed = 0;
static int g_failed = 0;

static void checkApprox(double actual, double expected, double eps, const std::string& name) {
    if (std::abs(actual - expected) <= eps) {
        std::cout << "  [PASS] " << name << " (got: " << actual << ")\n";
        g_passed++;
    } else {
        std::cerr << "  [FAIL] " << name << " (expected: " << expected << ", got: " << actual << ")\n";
        g_failed++;
    }
}

static void checkEqual(const std::string& actual, const std::string& expected, const std::string& name) {
    if (actual == expected) {
        std::cout << "  [PASS] " << name << " ('" << actual << "')\n";
        g_passed++;
    } else {
        std::cerr << "  [FAIL] " << name << " (expected: '" << expected << "', got: '" << actual << "')\n";
        g_failed++;
    }
}

static void checkTrue(bool condition, const std::string& name) {
    if (condition) {
        std::cout << "  [PASS] " << name << "\n";
        g_passed++;
    } else {
        std::cerr << "  [FAIL] " << name << "\n";
        g_failed++;
    }
}

void testMathCalculator() {
    std::cout << "\n[1/4] Testing Mathematical Expression Evaluator...\n";
    MathCalc::Calculator calc;

    // Operator precedence & associativity
    checkApprox(calc.evaluate("2 + 3 * 4"), 14.0, 1e-6, "Precedence: 2 + 3 * 4 = 14");
    checkApprox(calc.evaluate("(2 + 3) * 4"), 20.0, 1e-6, "Parentheses: (2 + 3) * 4 = 20");
    checkApprox(calc.evaluate("10 - 3 - 2"), 5.0, 1e-6, "Left-associativity: 10 - 3 - 2 = 5");
    checkApprox(calc.evaluate("2 ^ 3 ^ 2"), 512.0, 1e-6, "Right-associative power: 2^3^2 = 512");
    checkApprox(calc.evaluate("10 % 3"), 1.0, 1e-6, "Modulo: 10 % 3 = 1");
    checkApprox(calc.evaluate("-5 + 12"), 7.0, 1e-6, "Unary minus: -5 + 12 = 7");
    checkApprox(calc.evaluate("-(3 * 4)"), -12.0, 1e-6, "Unary negation of subexpression");

    // Standard math functions
    checkApprox(calc.evaluate("sqrt(144)"), 12.0, 1e-6, "sqrt(144) = 12");
    checkApprox(calc.evaluate("cbrt(27)"), 3.0, 1e-6, "cbrt(27) = 3");
    checkApprox(calc.evaluate("sin(pi / 2)"), 1.0, 1e-6, "sin(pi/2) = 1");
    checkApprox(calc.evaluate("cos(0)"), 1.0, 1e-6, "cos(0) = 1");
    checkApprox(calc.evaluate("log(100)"), 2.0, 1e-6, "log10(100) = 2");
    checkApprox(calc.evaluate("ln(e)"), 1.0, 1e-6, "ln(e) = 1");
    checkApprox(calc.evaluate("log2(64)"), 6.0, 1e-6, "log2(64) = 6");
    checkApprox(calc.evaluate("fact(5)"), 120.0, 1e-6, "fact(5) = 120");
    checkApprox(calc.evaluate("5!"), 120.0, 1e-6, "5! postfix = 120");
    checkApprox(calc.evaluate("abs(-42.5)"), 42.5, 1e-6, "abs(-42.5) = 42.5");
    checkApprox(calc.evaluate("floor(3.7)"), 3.0, 1e-6, "floor(3.7) = 3");
    checkApprox(calc.evaluate("ceil(3.2)"), 4.0, 1e-6, "ceil(3.2) = 4");
    checkApprox(calc.evaluate("round(3.5)"), 4.0, 1e-6, "round(3.5) = 4");
    checkApprox(calc.evaluate("deg(pi)"), 180.0, 1e-6, "deg(pi) = 180");
    checkApprox(calc.evaluate("rad(180)"), 3.141592653589793, 1e-6, "rad(180) = pi");

    // Complex nested evaluation
    checkApprox(calc.evaluate("sqrt(3^2 + 4^2) * 2 + fact(4) / 4"), 16.0, 1e-6, "Pythagorean + factorial combo");

    // Error checks
    bool threwDivZero = false;
    try { calc.evaluate("5 / 0"); } catch (const MathCalc::EvaluationError&) { threwDivZero = true; }
    checkTrue(threwDivZero, "Caught division by zero exception");

    bool threwMismatchedParen = false;
    try { calc.evaluate("(3 + 4 * 2"); } catch (const MathCalc::EvaluationError&) { threwMismatchedParen = true; }
    checkTrue(threwMismatchedParen, "Caught unclosed parenthesis exception");

    bool threwSqrtNeg = false;
    try { calc.evaluate("sqrt(-4)"); } catch (const MathCalc::EvaluationError&) { threwSqrtNeg = true; }
    checkTrue(threwSqrtNeg, "Caught negative sqrt domain error");
}

void testUnitConverter() {
    std::cout << "\n[2/4] Testing Multi-Category Unit Converter...\n";
    Units::UnitConverter conv;

    // Length
    checkApprox(conv.convert(1.0, "km", "m"), 1000.0, 1e-6, "1 km -> 1000 m");
    checkApprox(conv.convert(100.0, "cm", "m"), 1.0, 1e-6, "100 cm -> 1 m");
    checkApprox(conv.convert(1.0, "mi", "m"), 1609.344, 1e-6, "1 mi -> 1609.344 m");
    checkApprox(conv.convert(1.0, "ft", "in"), 12.0, 1e-6, "1 ft -> 12 in");
    checkApprox(conv.convert(1.0, "yd", "ft"), 3.0, 1e-6, "1 yd -> 3 ft");

    // Mass
    checkApprox(conv.convert(1.0, "kg", "g"), 1000.0, 1e-6, "1 kg -> 1000 g");
    checkApprox(conv.convert(1.0, "lb", "oz"), 16.0, 1e-4, "1 lb -> 16 oz");
    checkApprox(conv.convert(1000.0, "kg", "t"), 1.0, 1e-6, "1000 kg -> 1 metric ton");

    // Temperature (non-linear scaling)
    checkApprox(conv.convert(0.0, "C", "F"), 32.0, 1e-6, "0 C -> 32 F");
    checkApprox(conv.convert(100.0, "C", "F"), 212.0, 1e-6, "100 C -> 212 F");
    checkApprox(conv.convert(32.0, "F", "C"), 0.0, 1e-6, "32 F -> 0 C");
    checkApprox(conv.convert(0.0, "C", "K"), 273.15, 1e-6, "0 C -> 273.15 K");
    checkApprox(conv.convert(300.0, "K", "C"), 26.85, 1e-6, "300 K -> 26.85 C");

    // Digital Data
    checkApprox(conv.convert(1.0, "KiB", "B"), 1024.0, 1e-6, "1 KiB -> 1024 Bytes");
    checkApprox(conv.convert(1.0, "MiB", "KiB"), 1024.0, 1e-6, "1 MiB -> 1024 KiB");
    checkApprox(conv.convert(1.0, "GB", "MB"), 1000.0, 1e-6, "1 GB -> 1000 MB");
    checkApprox(conv.convert(8.0, "bit", "B"), 1.0, 1e-6, "8 bits -> 1 Byte");

    // Speed & Pressure
    checkApprox(conv.convert(36.0, "km/h", "m/s"), 10.0, 1e-4, "36 km/h -> 10 m/s");
    checkApprox(conv.convert(1.0, "mph", "ft/s"), 1.46667, 1e-4, "1 mph -> 1.467 ft/s");
    checkApprox(conv.convert(1.0, "atm", "Pa"), 101325.0, 1e-4, "1 atm -> 101325 Pa");
    checkApprox(conv.convert(1.0, "bar", "kPa"), 100.0, 1e-4, "1 bar -> 100 kPa");

    // Energy
    checkApprox(conv.convert(1.0, "kWh", "J"), 3.6e6, 1e-4, "1 kWh -> 3.6 MJ");
    checkApprox(conv.convert(1.0, "kcal", "cal"), 1000.0, 1e-4, "1 kcal -> 1000 cal");

    bool threwCategoryMismatch = false;
    try { conv.convert(10.0, "km", "kg"); } catch (const Units::ConversionError&) { threwCategoryMismatch = true; }
    checkTrue(threwCategoryMismatch, "Rejected cross-category conversion (km -> kg)");
}

void testProgrammerCalc() {
    std::cout << "\n[3/4] Testing Programmer Radix & Bitwise Engine...\n";
    using namespace ProgrammerCalc;

    // Radix Parsing
    checkTrue(BaseEngine::parseNumber("255", Radix::Decimal) == 255, "Parsed decimal '255'");
    checkTrue(BaseEngine::parseNumber("0xFF") == 255, "Parsed hex '0xFF'");
    checkTrue(BaseEngine::parseNumber("0b11111111") == 255, "Parsed binary '0b11111111'");
    checkTrue(BaseEngine::parseNumber("0o377") == 255, "Parsed octal '0o377'");
    checkTrue(BaseEngine::parseNumber("1010", Radix::Binary) == 10, "Parsed raw binary '1010'");

    // Bitwise Logic
    checkTrue(BaseEngine::bitAnd(0b1100, 0b1010, WordSize::Bit32) == 0b1000, "Bitwise AND (1100 & 1010)");
    checkTrue(BaseEngine::bitOr(0b1100, 0b1010, WordSize::Bit32) == 0b1110, "Bitwise OR (1100 | 1010)");
    checkTrue(BaseEngine::bitXor(0b1100, 0b1010, WordSize::Bit32) == 0b0110, "Bitwise XOR (1100 ^ 1010)");
    checkTrue(BaseEngine::bitNot(0, WordSize::Bit8) == 0xFF, "Bitwise NOT 8-bit (~0 = 0xFF)");
    checkTrue(BaseEngine::shiftLeft(1, 4, WordSize::Bit32) == 16, "Shift Left (1 << 4 = 16)");
    checkTrue(BaseEngine::shiftRightLogical(16, 2, WordSize::Bit32) == 4, "Shift Right (16 >> 2 = 4)");
    checkTrue(BaseEngine::rotateLeft(1, 1, WordSize::Bit8) == 2, "Rotate Left (1 ROL 1 = 2)");
    checkTrue(BaseEngine::countSetBits(0b101101) == 4, "PopCount of 0b101101 = 4");
}

void testFinanceHealth() {
    std::cout << "\n[4/4] Testing Financial & Health Models...\n";
    using namespace FinanceHealth;

    // Loan Monthly EMI ($100k @ 12% for 12 months)
    auto emi = FinanceHealthEngine::calculateEMI(100000.0, 12.0, 12);
    checkApprox(emi.monthlyEMI, 8884.8788, 0.1, "EMI payment calculation");
    checkApprox(emi.totalPayment, 106618.55, 1.0, "Total payment calculation");
    checkApprox(emi.totalInterest, 6618.55, 1.0, "Total interest calculation");

    // Compound Interest ($1000 @ 10% for 2 years compounded annually)
    auto ci = FinanceHealthEngine::calculateCompoundInterest(1000.0, 10.0, 2.0, 1);
    checkApprox(ci.futureValue, 1210.0, 0.01, "Compound interest future value");
    checkApprox(ci.totalInterest, 210.0, 0.01, "Compound interest total earned");

    // Percentages & Discounts
    checkApprox(FinanceHealthEngine::percentOf(15.0, 200.0), 30.0, 1e-6, "15% of 200 = 30");
    checkApprox(FinanceHealthEngine::percentageChange(100.0, 150.0), 50.0, 1e-6, "+50% change from 100 to 150");
    checkApprox(FinanceHealthEngine::calculateDiscount(100.0, 20.0, 10.0), 88.0, 1e-6, "$100 with 20% discount + 10% tax");

    // BMI Calculation
    auto bmi = FinanceHealthEngine::calculateBMI(70.0, 1.75);
    checkApprox(bmi.bmiValue, 22.857, 0.01, "BMI score for 70kg / 1.75m");
    checkEqual(bmi.category, "Normal / Healthy Weight", "BMI category classification");
}

int main() {
    std::cout << "========================================\n";
    std::cout << "     CALCULATOR & CONVERTER TEST SUITE  \n";
    std::cout << "========================================\n";

    testMathCalculator();
    testUnitConverter();
    testProgrammerCalc();
    testFinanceHealth();

    std::cout << "\n========================================\n";
    std::cout << " RESULTS: Passed: " << g_passed << " | Failed: " << g_failed << "\n";
    std::cout << "========================================\n";

    return (g_failed == 0) ? 0 : 1;
}
