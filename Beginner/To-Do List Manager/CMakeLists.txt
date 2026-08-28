cmake_minimum_required(VERSION 3.15)
project(TodoListManager VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(src)

set(CORE_SOURCES
    src/Storage.cpp
    src/TaskManager.cpp
    src/UI.cpp
)

add_executable(todo_manager src/main.cpp ${CORE_SOURCES})

add_executable(todo_tests tests/test_runner.cpp
    src/Storage.cpp
    src/TaskManager.cpp
)

enable_testing()
add_test(NAME AllUnitTests COMMAND todo_tests)
