# Quetzal-CoalTL <img align="right" width="200" src="https://github.com/Becheler/quetzal-CoalTL/blob/media/quetzal.png">

[![Website becheler.github.io](https://img.shields.io/website-up-down-green-red/https/becheler.github.io.svg)](https://quetzal-framework.github.io/quetzal-CoaTL/)
[![GitHub release (latest by date)](https://img.shields.io/github/v/release/Becheler/quetzal-CoalTL)](https://github.com/Quetzal-framework/quetzal-CoaTL/releases)
[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
![GitHub last commit](https://img.shields.io/github/last-commit/Becheler/quetzal-CoalTL)

# C++ Template Libary for coalescence

Quetzal-CoalTL is a Modern C++ template scientific library for
simulating the coalescence of gene genealogies in explicit landscapes or
phylogenetic networks.

Its aim is to standardize a library of reusable STL-compatible components to encourage
code reuse.

> :egg::egg::egg:  If you are not a C++ programmer,
> then you are unlikely to benefit directly from this library: instead
> you may prefer the Quetzal-EGGS simulators,
> please read [this page](https://github.com/Quetzal-framework/quetzal-EGGS).

# Documentation

The documentation lives [here](https://quetzal-framework.github.io/quetzal-CoaTL/).

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

### Packaging

```bash
cd build
conan create ..
conan upload -r quetzal quetzal-CoaTL/0.1 --all
```

# Contact

- :email: You are interested? Want to contribute? Want to give some feedback? Don't be shy, [contact me!](abechele@umich.edu)
- :star: You think this is a cool project? Drop a star on GitHub :point_up:
- :bug: A bug? Oopsie daisy! I'll fix it asap if you [email me](abechele@umich.edu) or open an issue :point_up:

# Author

- AUTHOR : Arnaud Becheler
- DATE   : 2016
- LICENCE : This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
- CONTACT : abechele@umich.edu
- WEBSITE : https://becheler.github.io/

# How to cite:

Becheler, A, Coron, C, Dupas, S. The Quetzal Coalescence template library: A C++ programmers resource for integrating distributional, demographic and coalescent models. Mol Ecol Resour. 2019; 19: 788– 793. https://doi.org/10.1111/1755-0998.12992

Becheler, A., & Knowles, L. L. (2020). Occupancy spectrum distribution: application for coalescence simulation with generic mergers. Bioinformatics, 36(10), 3279-3280.
