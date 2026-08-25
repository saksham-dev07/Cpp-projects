#!/usr/bin/env bash
set -e

echo "========================================================"
echo "  Building Number Guessing Arena (C++17)"
echo "========================================================"

mkdir -p bin

CXX=${CXX:-g++}
CXXFLAGS="-std=c++17 -O2 -Wall -Wextra -I src"

echo "Compiling Main Application (number_guesser)..."
$CXX $CXXFLAGS \
    src/main.cpp src/Terminal.cpp src/InputValidator.cpp src/RandomGenerator.cpp \
    src/HintSystem.cpp src/StatsManager.cpp src/AiGuesser.cpp src/BullsAndCows.cpp src/GameEngine.cpp \
    -o bin/number_guesser

echo "Compiling Test Suite (number_tests)..."
$CXX $CXXFLAGS \
    tests/test_runner.cpp src/Terminal.cpp src/RandomGenerator.cpp \
    src/HintSystem.cpp src/StatsManager.cpp src/AiGuesser.cpp src/BullsAndCows.cpp \
    -o bin/number_tests

echo ""
echo "[SUCCESS] Build completed successfully!"
echo "Executables created:"
echo "  - bin/number_guesser"
echo "  - bin/number_tests"
echo ""
