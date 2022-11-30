# Installation


# Installation

```bash
conan install conan/conanfile.py --build=missing --install-folder=build -pr:b=conan/profiles/clang_13 -pr:h=conan/profiles/clang_13
cd build
cmake -D CMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake ..
cmake --build .
ctest
make docs
```

## With Conan + CMake

### Install Conan

[Conan](https://conan.io/) is one of the leading options for cross-platform package
manager with C/C++ projects. We chose it because it interfaces with CMake in a nice
way. Conan will handle the dependencies and version conflicts management, and pass
the paths of the installed dependencies to CMake so it can build the project. It's
as simple to install as `pip install conan`! Of course if you are careful about not messing
up with your system, you may want to use conan from a virtual environment.

```bash
python3 -m venv virtual-env
source virtual-env/bin/activate
pip install --upgrade pip
pip install conan
deactivate
```

### Install CMake

[CMake](https://cmake.org/cmake/help/latest/manual/cmake.1.html) is the C++ build
systems first choice for cross-platform development. Technically, CMake is a build
system generator but the level of abstraction it offers allows us to consider it as a cross-platform build system.
Users can build, test, and install packages with `cmake` and `ctest` commands.
Please refer to the [CMake official documentation](https://cmake.org/install/)
for installation on your specific OS.

### Configure Conan

To ask conan to download, build and install Quetzal-CoaTL dependencies, run:

```bash
source virtual-env/bin/activate
export CONAN_V2_MODE=1
export CC=`which gcc` && export CXX=`which g++`
chmod u+x conan/configure.sh && ./conan/configure.sh
deactivate
```

#### Configure CMake

Lets start in the project root folder:

```bash
rm -rf build
mkdir build && cd build
cmake -D CMAKE_BUILD_TYPE=Release \
        -D CMAKE_C_COMPILER=${CC} \
        -D CMAKE_CXX_COMPILER=${CXX} \
        -D CMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake \
        ..
```

#### 3 - Build, test, install :rocket:

```bash
# Works on Linux, OSX, and Windows.
$ ncpus=$(python -c 'import multiprocessing as mp; print(mp.cpu_count())')
$ cmake --build . --parallel ${ncpus}
$ ctest --parallel ${ncpus}
$ cmake --build . --target install
```
