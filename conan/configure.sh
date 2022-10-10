#!/usr/bin/env bash

set -e

conan profile new quetzal-coatl-profile --detect --force

# C++20
conan profile update settings.compiler.cppstd=20 quetzal-coatl-profile

#conan profile update settings.compiler=gcc quetzal-coatl-profile
#conan profile update settings.compiler.version=12 quetzal-coatl-profile
# ABI compatibility
#conan profile update settings.compiler.libcxx=libstdc++11 quetzal-coatl-profile
#conan profile update env.CC=[/usr/bin/gcc-12] quetzal-coatl-profile
#conan profile update env.CXX=[/usr/bin/g++-12] quetzal-coatl-profile

conan install conan/conanfile_macos.txt --build=missing --install-folder=build -pr:b=quetzal-coatl-profile -pr:h=quetzal-coatl-profile
