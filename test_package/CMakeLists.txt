cmake_minimum_required(VERSION 3.13)
project(unit-test)

# Use modern C++ with support for concepts and mp-units
set(CMAKE_CXX_STANDARD 20)
# Prevent use of non-portable compiler extensions
set(CMAKE_CXX_EXTENSIONS OFF)
# This makes C++20 a requirement and prevents a "decay" to C++98 when the compiler does not support C++20.
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

add_executable(${PROJECT_NAME} main.cpp)
conan_target_link_libraries(${PROJECT_NAME})
