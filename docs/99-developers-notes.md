### Build process
```bash
conan install conanfile.py --build=missing --install-folder=build -pr:b=conan/profiles/clang_13 -pr:h=conan/profiles/clang_13
cd build
cmake -D CMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake ..
cmake --build .
ctest
make docs
```

### Packaging

```bash
cd build
conan create ..
conan upload -r quetzal quetzal-CoaTL/0.1 --all
```

```
conan create . test/demo -pr:b=conan/profiles/clang_13 -pr:h=conan/profiles/clang_13 --test-folder=conan/test_package
```

```
export CC=`which gcc` && export CXX=`which g++`
```
