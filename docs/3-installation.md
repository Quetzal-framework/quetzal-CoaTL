# Installation and Usage

> **Note**
>
>  Quetzal-CoaTL uses C++20 features: it requires the latest C++ compilers.
>  The following compilers (or newer) are supported:
> - gcc-12
> - clang-13

## Structure and Dependencies

This repository contains three independent CMake-based projects:

* `./src`
  - header-only project containing whole Quetzal-CoaTL library
  - this library depends on:
    - [GDAL](https://gdal.org/) for geographic data manipulation
    - [Boost](https://www.boost.org/doc/libs/master/index.html) for general purpose C++ utilities
* `.`
  - project used as an entry point for library development
  - it wraps `./src` project with examples and tests
  - if you want to build the documentation, the following are required:
    - [Doxygen](https://www.doxygen.nl/) to generate documentation from annotated C++ sources
* `./test_package`
  - installs the library and verifies Conan packaging.

## Obtaining Dependencies

Quetzal-CoaTL assumes that most of the dependencies will be retrieved by the Conan Package Manager.
If you don't wish to use Conan, some modifications of the CMake files may be necessary.

### CMake

[CMake](https://cmake.org/cmake/help/latest/manual/cmake.1.html) is the C++ build
systems first choice for cross-platform development. Technically, CMake is a build
system generator but the level of abstraction it offers allows us to consider
it as a cross-platform build system
Users can build, test, and install packages with `cmake` and `ctest` commands.

Please refer to the [CMake official documentation](https://cmake.org/install/)
for installation on your specific OS.

### Conan

[Conan](https://conan.io/) is one of the leading options for cross-platform package
manager with C/C++ projects. We chose it because it interfaces with CMake in a nice
way. Conan will handle the dependencies and version conflicts management, and pass
the paths of the installed dependencies to CMake so it can build the project.

Please refer to the [Conan official documentation](https://docs.conan.io/en/latest/installation.html)
for installation on your specific OS.

#### Quick start with Conan

After installing Conan, you may need a custom profile file in `~/.conan/profiles` directory.

[Profiles](https://docs.conan.io/en/latest/reference/profiles.html) can be a bit
confusing at first, but they simply allows users to set a
complete configuration set for settings, options, environment variables,
and build requirements in a single reusable file.

An example profile can look as follows:
```ini
[settings]
os=Macos
os_build=Macos
arch=armv8
arch_build=armv8
compiler=apple-clang
compiler.version=13
compiler.libcxx=libc++
compiler.cppstd=20
build_type=Release
```

> Quetzal-CoaTL requires C++20 to be activated either in a Conan profile or with Conan command line.
> By setting the option once for all in the profile, you will avoid typing `-s compiler.cppstd=20`
> every time you run a Conan command.


## Installation and Reuse

Depending on your needs and constraints, there are several ways to reuse the library code.

### Copy

Since Quetzal-CoaTL is a header-only library, you can copy-paste the `include` subdirectory
to your source tree.

> :warning: If you go this way, you will be responsible for ensuring that the dependencies are installed,
> that the header files can be located at build-time and that the right flags are
> passed to the compiler.

### Copy + CMake

If you copy the entire Quetzal-CoaTL repository to your source tree, you can reuse
the CMake targets defined by the library. To do so you should use `CMakeLists.txt` file
from the `./src` directory:

```cmake
add_subdirectory(<path_to_quetzal_folder>/src)
# ...
target_link_libraries(<your_target> <PUBLIC|PRIVATE|INTERFACE> quetzal::quetzal)
```

> :warning: You are still responsible to ensure that the dependencies are installed
> and that the header files can be located at build-time.

### Conan + CMake

> If you are new to Conan, I recommend read [Obtaining Dependencies](##obtaining-dependencies),
> and the chapters [Getting Started](https://docs.conan.io/en/latest/getting_started.html)
> and [Using packages](https://docs.conan.io/en/latest/using_packages.html)
> from the official Conan documentation.

Quetzal-CoaTL releases are hosted on Artifactory. To obtain official library
release please do the following:

1. Add quetzal-CoaTL remote
```
conan remote add becheler-conan https://arnaudbecheler.jfrog.io/artifactory/api/conan/becheler-conan
```
2. Create Conan configuration file (either `conanfile.txt` or `conanfile.py`) in your project’s top-level directory and add quetzal-CoaTL as a dependency of your project. For example the simplest file could look like:
```
[requires]
quetzal-coatl/3.0.1@becheler/stable

[generators]
CMakeToolchain
CMakeDeps
```
3. Import Quetzal-CoaTL and its dependencies definitions to your project with `find_package`
```cmake
find_package(quetzal-coatl CONFIG REQUIRED)
```
4. Link your CMake targets with quetzal:
```cmake
target_link_libraries(<your_target> <PUBLIC|PRIVATE|INTERFACE> quetzal-coatl::quetzal-coatl)
target_compile_features(<your_target> <PUBLIC|PRIVATE|INTERFACE> cxx_std_20)
```
5. Ask Conan to download, build and install Quetzal-CoaTL dependencies and perform CMake configuration:
```bash
mkdir my_project/build && cd my_project/build
conan install .. -pr <your_conan_profile> -s compiler.cppstd=20 -b=missing
cmake .. -G "Ninja Multi-Config" -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
cmake --build . --config Release
```
