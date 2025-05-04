#!/bin/bash

# Color definitions for pretty print
GREEN="\033[0;32m"
YELLOW="\033[0;33m"
CYAN="\033[0;36m"
MAGENTA="\033[0;35m"
RED="\033[0;31m"
GREY="\033[2m"
RESET="\033[0m"

# Input arguments
COMPILER=$1
BUILD_MODE=$2
STD_VERSION=$3
PACKAGER=$4

BASE="${COMPILER%-*}"
VERSION="${COMPILER#*-}"

# Pretty print the build configuration
echo -e "${YELLOW}=== Package Manager Setup =========${RESET}"
echo -e "${CYAN}Compiler Base:    ${GREEN}${BASE}${RESET}"
echo -e "${CYAN}Compiler Version: ${GREEN}${VERSION}${RESET}"
echo -e "${CYAN}Build Mode:       ${GREEN}${BUILD_MODE}${RESET}"
echo -e "${CYAN}C++ Standard:     ${GREEN}${STD_VERSION}${RESET}"
echo -e "${CYAN}Package Manager:  ${GREEN}${PACKAGER}${RESET}"
echo -e "${YELLOW}===================================${RESET}"

# Check the package manager and execute corresponding commands
if [[ "$PACKAGER" == "vcpkg" ]]; then
    echo -e "[INFO] Setting up vcpkg..."
    if [ "$(uname -m)" != "x86_64" ]; then
      echo -e "${MAGENTA}[INFO] Non-x86 platform detected.${RESET}"
      echo -e "[INFO]  VCPKG_FORCE_SYSTEM_BINARIES set to ${VCPKG_FORCE_SYSTEM_BINARIES}."
    fi
    echo -e "[COMMAND] vcpkg --version"
    echo -e "${GREY}$(/opt/vcpkg/vcpkg --version)${RESET}"
    echo -e "[INFO] vcpkg installation verification completed successfully.${RESET}"
elif [[ "$PACKAGER" == "conan" ]]; then
    echo -e "${MAGENTA}Setting up conan...${RESET}"
    if ! conan profile detect > /dev/null 2>&1; then
        if [[ ! -f ~/.conan2/profiles/default ]]; then
            conan profile detect > /dev/null 2>&1
        fi
    fi
    conan install . -b missing -of build/conan \
        -s compiler="$BASE" \
        -s compiler.version="$VERSION" \
        -s build_type="$BUILD_MODE" \
        -s compiler.cppstd="$STD_VERSION"
else
    echo -e "${RED}Error: Unknown package manager '${PACKAGER}'${RESET}"
    exit 1
fi

echo -e "${GREEN}=== Package Manager Setup Complete ===${RESET}"