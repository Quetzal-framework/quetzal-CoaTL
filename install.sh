#!/usr/bin/env bash

conan profile new quetzal-coatl-profile --detect
conan profile update settings.compiler="gcc" quetzal-coatl-profile
conan profile update settings.compiler.version=10 quetzal-coatl-profile
conan profile update settings.compiler.cppstd=20 quetzal-coatl-profile
conan profile update settings.compiler.libcxx="libstdc++11" quetzal-coatl-profile
conan profile update env.CC=[/usr/bin/gcc-10] quetzal-coatl-profile
conan profile update env.CXX=[/usr/bin/g++-10] quetzal-coatl-profile

conan install conanfile.txt --build=always --install-folder=build -pr:b=quetzal-coatl-profile
