#!/usr/bin/env bash

set -e

conan profile new quetzal-coatl-profile --detect --force
conan profile update settings.compiler.libcxx="libstdc++11" quetzal-coatl-profile
conan profile update settings.compiler="gcc" quetzal-coatl-profile
conan profile update settings.compiler.version=12 quetzal-coatl-profile
conan profile update settings.compiler.cppstd=20 quetzal-coatl-profile
conan profile update env.CC=[/usr/bin/gcc-12] quetzal-coatl-profile
conan profile update env.CXX=[/usr/bin/g++-12] quetzal-coatl-profile

conan install conan/conanfile.txt --build=always --install-folder=build -pr:b=quetzal-coatl-profile
