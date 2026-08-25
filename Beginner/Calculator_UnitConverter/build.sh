#!/usr/bin/env bash
set -e

echo "========================================================"
echo "  Building CLI Calculator & Unit Converter (C++17)      "
echo "========================================================"

mkdir -p bin

echo "Compiling Main Application (calculator)..."
g++ -std=c++17 -O2 -Wall -Wextra -Isrc \
    src/main.cpp src/UI.cpp src/Calculator.cpp src/UnitConverter.cpp \
    src/ProgrammerCalc.cpp src/FinanceHealthCalc.cpp src/History.cpp \
    -o bin/calculator

echo "Compiling Test Suite (test_runner)..."
g++ -std=c++17 -O2 -Wall -Wextra -Isrc \
    tests/test_runner.cpp src/Calculator.cpp src/UnitConverter.cpp \
    src/ProgrammerCalc.cpp src/FinanceHealthCalc.cpp \
    -o bin/test_runner

echo ""
echo "[OK] Build completed successfully!"
echo "Executables created:"
echo "  - bin/calculator"
echo "  - bin/test_runner"
echo ""
