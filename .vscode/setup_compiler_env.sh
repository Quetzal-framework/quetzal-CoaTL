#!/bin/bash

# Exit on any error
set -e

# Color definitions for pretty print
GREEN="\033[0;32m"
YELLOW="\033[0;33m"
CYAN="\033[0;36m"
RED="\033[0;31m"
RESET="\033[0m"

# Input arguments
COMPILER=$1
BUILD_MODE=$2

# Extract base and version from the compiler
BASE="${COMPILER%-*}"
VERSION="${COMPILER#*-}"

# Set CC and CXX based on the compiler
if [[ "$BASE" == "clang" ]]; then
    export CC="/usr/bin/$COMPILER"
    export CXX="${CC/clang/clang++}"
elif [[ "$BASE" == "gcc" ]]; then
    export CC="/usr/bin/$COMPILER"
    export CXX="${CC/gcc/g++}"
else
    echo -e "${RED}Error: Unsupported compiler '${COMPILER}'${RESET}"
    exit 1
fi

# Set the build type
export CMAKE_BUILD_TYPE="$BUILD_MODE"

# Pretty print the compiler environment
echo -e "${YELLOW}=== Compiler Environment Setup ===${RESET}"
echo -e "${CYAN}Compiler Base:    ${GREEN}$BASE${RESET}"
echo -e "${CYAN}Compiler Version: ${GREEN}$VERSION${RESET}"
echo -e "${CYAN}Build Mode:       ${GREEN}$CMAKE_BUILD_TYPE${RESET}"
echo -e "${CYAN}CC:               ${GREEN}$CC${RESET}"
echo -e "${CYAN}CXX:              ${GREEN}$CXX${RESET}"
echo -e "${YELLOW}===================================${RESET}"
