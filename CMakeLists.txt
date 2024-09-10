# Copyright 2021 Arnaud Becheler
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.

cmake_minimum_required(VERSION 3.19)

project(quetzal-coatl-dev LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# Support for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# add project code
add_subdirectory(src)

# generate project documentation
add_subdirectory(docs)

# generate project documentation
add_subdirectory(mrdox)

# add template project
add_subdirectory(template)

# We default to build tests if option -D CMAKE_BUILD_TESTS=ON not provided (no FORCE, so the previous value, if set, stays)
set(BUILD_TESTS OFF CACHE STRING "Build module test")

if(BUILD_TESTS)
    enable_testing()
    # add unit tests
    add_subdirectory(test)
    # add usage example
    add_subdirectory(example)
endif()