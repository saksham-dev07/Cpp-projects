cmake_minimum_required(VERSION 3.15)
project(RockPaperScissors VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include directory
include_directories(src)

# Core source files
set(CORE_SOURCES
    src/Terminal.cpp
    src/InputValidator.cpp
    src/RandomGenerator.cpp
    src/RulesEngine.cpp
    src/AiPredictor.cpp
    src/GameTheoryAnalyzer.cpp
    src/StatsManager.cpp
    src/AsciiArt.cpp
    src/GameEngine.cpp
)

# Main Game Executable
add_executable(rps_game src/main.cpp ${CORE_SOURCES})

# Test Runner Executable
add_executable(rps_tests tests/test_runner.cpp
    src/Terminal.cpp
    src/InputValidator.cpp
    src/RandomGenerator.cpp
    src/RulesEngine.cpp
    src/AiPredictor.cpp
    src/GameTheoryAnalyzer.cpp
    src/StatsManager.cpp
    src/AsciiArt.cpp
)

# Enable testing
enable_testing()
add_test(NAME AllUnitTests COMMAND rps_tests)
