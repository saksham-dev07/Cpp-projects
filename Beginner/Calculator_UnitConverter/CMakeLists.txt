cmake_minimum_required(VERSION 3.15)
project(Calculator_UnitConverter VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include directory
include_directories(src)

# Core source files (reusable library)
set(CORE_SOURCES
    src/UI.cpp
    src/Calculator.cpp
    src/UnitConverter.cpp
    src/ProgrammerCalc.cpp
    src/FinanceHealthCalc.cpp
    src/History.cpp
)

# Main Executable
add_executable(calculator src/main.cpp ${CORE_SOURCES})

# Test Runner Executable
add_executable(test_runner tests/test_runner.cpp
    src/Calculator.cpp
    src/UnitConverter.cpp
    src/ProgrammerCalc.cpp
    src/FinanceHealthCalc.cpp
)

# Enable testing
enable_testing()
add_test(NAME AllUnitTests COMMAND test_runner)
