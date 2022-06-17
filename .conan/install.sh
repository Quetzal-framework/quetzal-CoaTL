#!/usr/bin/env bash

conan profile new quetzal-profile --detect
conan profile update settings.compiler="gcc" quetzal-profile
conan profile update settings.compiler.version=10 quetzal-profile
conan profile update settings.compiler.cppstd=20 quetzal-profile
conan profile update settings.compiler.libcxx="libstdc++11" quetzal-profile
conan profile update env.CC=[/usr/bin/gcc-10] quetzal-profile
conan profile update env.CXX=[/usr/bin/g++-10] quetzal-profile
conan install /.conan/conanfile.txt --build missing
