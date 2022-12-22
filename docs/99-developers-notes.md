# Developer's notes

## Conventions

Like the C++ Standard Library and Boost and numerous other libraries, Quetzal follows thede guidelines:

- Macro names use upper case with underscores: `INT_MAX`.
- Template parameter names use camel case: `InputIterator`.
- All other names use snake case: `unordered_map`.

Besides,

- Private data have a `m_` prefix to distinguish it from public data. `m_` stands for "member" data


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
