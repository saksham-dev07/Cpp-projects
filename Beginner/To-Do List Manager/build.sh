#!/usr/bin/env bash
set -e

echo "========================================================"
echo "  Building To-Do List Manager (C++17)"
echo "========================================================"

mkdir -p bin

echo "Compiling Main Application (todo_manager)..."
g++ -std=c++17 -O2 -Wall -Wextra -I src \
    src/main.cpp \
    src/Storage.cpp \
    src/TaskManager.cpp \
    src/UI.cpp \
    -o bin/todo_manager

echo "Compiling Test Suite (todo_tests)..."
g++ -std=c++17 -O2 -Wall -Wextra -I src \
    tests/test_runner.cpp \
    src/Storage.cpp \
    src/TaskManager.cpp \
    -o bin/todo_tests

echo ""
echo "[SUCCESS] Build completed successfully!"
echo "Executables created:"
echo "  - bin/todo_manager"
echo "  - bin/todo_tests"
echo ""
