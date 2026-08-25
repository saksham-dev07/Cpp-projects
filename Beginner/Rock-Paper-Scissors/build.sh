#!/usr/bin/env bash
set -e

echo "========================================================"
echo "  Building Rock-Paper-Scissors Tournament Arena (C++17) "
echo "========================================================"

mkdir -p bin

CXX=${CXX:-g++}
CXXFLAGS="-std=c++17 -O3 -Wall -Wextra -Isrc"

echo "Compiling Main Application with ${CXX}..."
${CXX} ${CXXFLAGS} \
    src/main.cpp \
    src/Terminal.cpp \
    src/InputValidator.cpp \
    src/RandomGenerator.cpp \
    src/RulesEngine.cpp \
    src/AiPredictor.cpp \
    src/GameTheoryAnalyzer.cpp \
    src/StatsManager.cpp \
    src/AsciiArt.cpp \
    src/GameEngine.cpp \
    -o bin/rps_game

echo "Compiling Test Runner with ${CXX}..."
${CXX} ${CXXFLAGS} \
    tests/test_runner.cpp \
    src/Terminal.cpp \
    src/InputValidator.cpp \
    src/RandomGenerator.cpp \
    src/RulesEngine.cpp \
    src/AiPredictor.cpp \
    src/GameTheoryAnalyzer.cpp \
    src/StatsManager.cpp \
    src/AsciiArt.cpp \
    -o bin/rps_tests

echo ""
echo "[SUCCESS] Build completed successfully!"
echo "Executables created:"
echo "  - bin/rps_game"
echo "  - bin/rps_tests"
