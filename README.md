# Quetzal-CoalTL <img align="right" width="200" src="https://github.com/Becheler/quetzal-CoalTL/blob/media/quetzal.png">

![GitHub release (latest by date)](https://img.shields.io/github/v/release/Becheler/quetzal-CoalTL)
[![Becheler](https://circleci.com/gh/Becheler/quetzal-CoaTL.svg?style=shield)](https://app.circleci.com/pipelines/github/Becheler/quetzal-CoaTL)
[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Website becheler.github.io](https://img.shields.io/website-up-down-green-red/https/becheler.github.io.svg)](https://becheler.github.io/softwares/quetzal-CoalTL/home/)
![Lines of code](https://img.shields.io/tokei/lines/github/Becheler/quetzal-CoalTL)
![GitHub last commit](https://img.shields.io/github/last-commit/Becheler/quetzal-CoalTL)

# A Coalescence Template Libary

Quetzal-CoalTL is a Modern C++ template scientific library for
simulating gene genealogies in explicit landscapes or phylogenetic networks. The basic
idea is to propose a library of reusable STL-compatible components to encourage
code reuse.

:egg::egg::egg:  If you're a not a programmer, or you are not familiar with modern C++,
then you are unlikely to benefit from installing Quetzal-CoaTL separately: instead it is more
likely you want to use some components of Quetzal-CoaTL through the Quetzal-EGGS simulators builder project:
please read [this page](https://github.com/Becheler/quetzal-EGGS).

# Usage

:seedling: Infering populations ecological features (such as migration or growth
patterns) from genetic datasets can be a complex task. In some settings it is
actually a mathematically intractable problem, so simulations methods are needed!

For example, Approximate Bayesian Computation (ABC) can be used to update current
knowledge on the processes shaping a genetic dataset by simulating the underlying
gene trees (coalescents) in an explicit geographic space.

Existing softwares like
[SPLATCHE](http://splatche.com/),
[simcoal2](http://cmpg.unibe.ch/software/simcoal2/),
[egglib](http://mycor.nancy.inra.fr/egglib/index.html), or
[msprime](http://msprime.readthedocs.io/en/stable/index.html) are very useful and
user-friendly resources that should be used whenever possible to achieve this task.

However if you are working on developing some border case simulation model, or
if you are not comfortable with hypothesis of the existing software solutions,
you will surely consider to build your own program :grimacing:

And that's why we need something that looks like a standard.

:v: Quetzal can help you doing so by offering atomic components (structures,
algorithms, concepts) that can be easily reused to build a new program.
The template mechanism allow to adapt them efficiently to each particular situation
you may encounter (STL-compatible).

Although the initial purpose of the library was mostly directed towards landscape
simulations, my intend is to propose general-purpose components for coalescence-simulations:
- generic containers for representing coalescent trees, forests (sets of trees), phylogenetic networks...
- generic algorithms (like DFS, BFS) to apply on containers
- policies to "furnish" the containers and specialize the containers (e.g. Newick format(**s*)
- polymoprhism statics running on abstract genealogies

# Installation

```
conan install conan/conanfile.py --build=missing --install-folder=build -pr:b=conan/profiles/clang_13 -pr:h=conan/profiles/clang_13
cd build
cmake -D CMAKE_TOOLCHAIN_FILE=conan_paths.cmake ..
cmake --build .
ctest
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

# Looking forward

- :crystal_ball: looking forward, we expect to develop the library related to genetic simulation and [C++20 concepts!](https://en.cppreference.com/w/cpp/language/constraints)
- :email: You are interested? Want to contribute? Want to give some feedback? Don't be shy, [contact me!](https://lsa.umich.edu/eeb/people/postdoctoral-fellows/arnaud-becheler.html)
- :star: You think this is a cool project? Drop a star on GitHub :point_up:
- :bug: A bug? Oopsie daisy! I'll fix it asap if you [email me](https://lsa.umich.edu/eeb/people/postdoctoral-fellows/arnaud-becheler.html) or open an issue :point_up:

![Quetzal-CoalTL concepts](https://github.com/Becheler/quetzal-CoalTL/blob/media/quetzal_scheme.png)

# Website

Please visit [Quetzal website](https://becheler.github.io/softwares/quetzal-CoalTL/home/) for more details and:
 - [user tutorials](https://becheler.github.io/softwares/quetzal-CoalTL/tutorials/)
 - [developer documentation](https://becheler.github.io/softwares/quetzal-CoalTL/API/html/index.html)
 - [FAQ](https://becheler.github.io/softwares/quetzal-CoalTL/FAQ/)

# Author

This github repository is basically the implementation of the probabilistic model of biological invasion developed during my PhD thesis under the direction of Stephane Dupas (laboratoire d'Evolution, Génomes, Comportements et Ecologie de Gif-sur-Yvette) and Camille Coron (Laboratoire de Mathématiques d'Orsay).

- AUTHOR : Arnaud Becheler
- DATE   : 2016
- LICENCE : This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
- CONTACT : abechele@umich.edu
- WEBSITE : https://becheler.github.io/

# How to cite:

Becheler, A, Coron, C, Dupas, S. The Quetzal Coalescence template library: A C++ programmers resource for integrating distributional, demographic and coalescent models. Mol Ecol Resour. 2019; 19: 788– 793. https://doi.org/10.1111/1755-0998.12992

Becheler, A., & Knowles, L. L. (2020). Occupancy spectrum distribution: application for coalescence simulation with generic mergers. Bioinformatics, 36(10), 3279-3280.

# Acknowledgements

- Florence Jornod who was a tremendous intern and add much to the project.
- Ambre Marques for his help, tips and the implementation of the expressive library.
- Arnaud Le Rouzic for help and tips.
- The developpez.com community, for their large help. This project would have failed without the constant support of Bousk, ternel, Koala, bacelar, dalfab, Medinoc, jo_link_noir and many others...
