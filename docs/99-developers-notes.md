# Developer's notes

## Build process

```bash
conan install conanfile.py --build=missing --install-folder=build -pr:b=conan/profiles/clang_13 -pr:h=conan/profiles/clang_13
cd build
cmake -D CMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake ..
cmake --build .
ctest
make docs
```

## Packaging

```bash
conan remote add becheler-conan https://arnaudbecheler.jfrog.io/artifactory/api/conan/becheler-conan
conan create . test/demo -pr:b=conan/profiles/clang_13 -pr:h=conan/profiles/clang_13
conan upload quetzal-CoaTL/0.1@demo/testing --all -r=becheler-conan
```
