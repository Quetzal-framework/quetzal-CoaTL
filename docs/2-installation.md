# Installation and Usage

@note
Quetzal-CoaTL uses C++20 features: it requires the latest C++ compilers.
The following compilers (or newer) are supported:
- gcc-12
- clang-14

The library provides many options for reuse, and the most suitable approach depends on your specific requirements. 

@tableofcontents

## Structure and Dependencies

This repository contains three independent CMake-based projects:

* `./src`
  - header-only project containing the Quetzal-CoaTL library
  - `./src/CMakeList.txt` file is intended as an entry point for library users
  - it depends on the following external dependencies:
    - [GDAL](https://gdal.org/) for geographic data manipulation
    - [Boost](https://www.boost.org/doc/libs/master/index.html) for general purpose C++ utilities
    - [range-v3](https://github.com/ericniebler/range-v3), a range library for C++14/17/20. 
* `.`
  - entry point for library development and continuous deployment (CI/CD) processes.
  - it wraps `./src` project with a cmake project, examples, tests and docs.
  - if you want to build the documentation without Conan, [Doxygen](https://www.doxygen.nl/)
    is required to generate the documentation from the annotated C++ sources.
* `./test_package`
  - library installation and Conan packaging verification

@remark
There are two entry points:
- The top-level `./CMakeLists.txt` file is intended solely for Quetzal developers and contributors to build the entire project code (example and tests) with highly restrictive compilation flags.
- The `./src/CMakeLists.txt` file, which contains a pure library definition and is intended for customers who prefer using CMake's `add_subdirectory()` to manage dependencies.

---

## Installation and Reuse

---

### Easiest: DevContainer on Visual Studio Code

The easiest option to try out the project.

To swiftly open an integrated development environment (IDE), begin coding to explore the library, and ensure seamless execution and compilation, the most efficient solution is the following:

1. [Install Visual Studio Code](https://code.visualstudio.com/download)
2. [Install Docker Desktop](https://docs.docker.com/desktop/)
3. Open Docker and leave it active in the background
4. Click [here](https://vscode.dev/redirect?url=vscode://ms-vscode-remote.remote-containers/cloneInVolume?url=https://github.com/Quetzal-framework/quetzal-CoaTL) to get started.

This will download the project, builds a Docker Image, manage the dependencies and automatically build and run the tests. If it's the first time you build the image, it may take some time, so you're free to go get a coffee!

Then, 
1. navigate to the `template/main.cpp` file
2. Open the VSC Command Palette with `Shift + Command + P` (Mac) or `Ctrl + Shift + P` (Windows/Linux)
3. Click on `Run Task > Run Template Project (Debug)`
4. Read the output in the integrated terminal
5. Edit `template/main.cpp` to your liking and re-run the task, observe the difference in the terminal.

---

### Copy

Since Quetzal-CoaTL is a header-only library, you can copy-paste the `./src` subdirectory
to your source tree.

@note
If you go this way, you will be responsible for ensuring that the dependencies are installed,
that the header files can be located at build-time and that the right flags are
passed to the compiler.

---

### Copy + CMake

If you copy the entire Quetzal-CoaTL repository to your source tree, you can reuse
the CMake targets defined by the library. To do so you should use `CMakeLists.txt` file
from the `./` directory:

```cmake
add_subdirectory(<path_to_quetzal_folder>/src)
# ...
target_link_libraries(<your_target> <PUBLIC|PRIVATE|INTERFACE> quetzal::quetzal)
```

@note
You are still responsible to ensure that the dependencies are installed
and that the header files can be located at build-time.

---

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
@note
If you want to build the library tests, examples and documentation, please refer to
the Developer's Notes section.

### Docker

[Quetzal-NEST](https://github.com/Quetzal-framework/quetzal-NEST), a dedicated Docker Image for Quetzal, is a self-contained package containing all the necessary components (installed library, code, dependencies, and runtime environment) to run a Quetzal-based application. See Tooling section for more details on how to use it. 

```
docker pull arnaudbecheler/quetzal-nest
docker run -it --entrypoint bash arnaudbecheler/quetzal-nest 
```

This will start the container and you will gain an interactive shell prompt within the container `home` folder. This allows you to execute commands and interact with the container's filesystem and environment using the Bash shell. 

### Starter template + VSC

DevContainer, a Visual Studio Code extension, provides developers with a convenient way to create and operate within a consistent and isolated development environment using Docker, ensuring all dependencies and configurations are readily available.

With this extension, you can open a **Quetzal** project in a development container directly from VS Code and gain access to an integrated terminal: it will use the [Quetzal-NEST](https://github.com/Quetzal-framework/quetzal-NEST) Docker Image behind the scenes to create a containerized environment that includes all the tools and dependencies needed for the project.

To open the DevContainer, you can follow these steps:

1. Ensure you have [Visual Studio Code](https://code.visualstudio.com/) and [Docker](https://docs.docker.com/get-docker/) installed on your system. If not, you can download it from the official Visual Studio Code website: https://code.visualstudio.com/

2. Install the `Remote - Containers` extension in Visual Studio Code. This extension enables you to work with [DevContainers](https://code.visualstudio.com/docs/devcontainers/containers). You can find and install the extension from the Visual Studio Code Extensions view.

3. Clone the [Quetzal Starter Template](https://github.com/Quetzal-framework/starter-template.git) to your local machine at a convenient location:
   ```bash
   git clone https://github.com/Quetzal-framework/starter-template.git
   ```

4. Open Visual Studio Code and navigate to the location of the cloned repository folder `starter-template` by selecting `Open Folder...` from the `File` menu.

5. Visual Studio Code should detect the DevContainer configuration in the repository and prompt you to reopen the folder in a DevContainer. Click on the `Reopen in Container` button that appears in the lower-right corner of the Visual Studio Code window.

6. Visual Studio Code will automatically build the Docker image and create a container based on the DevContainer configuration. This may take a few moments, especially if it's the first time you're opening the DevContainer: you have time to go get a coffee, to check on a your loved ones, or read your favorite blog.

7. Once the container is ready, Visual Studio Code will reopen the folder inside the DevContainer. Open an integrated terminal by selecting `New Terminal` from the `Terminal` menu. You can now start working with the development environment provided by the DevContainer.

8. The `starter-template` repository contains a `src/main.cpp` that has been populated with a dummy project. You can build and run the project by typing in the integrated terminal:
    ```bash
    cmake -S . -B ./build
    cmake --build ./build
    /build/bin/app
    ```

---

## Tooling Overview

In software development workflow, "tooling" refers to the set of tools, software, and utilities that developers use to enhance their productivity, automate tasks, and streamline the development process.

Quetzal-CoaTL assumes that most of the dependencies will be retrieved by the Conan Package Manager
and the code built with CMake. If you don't wish to use Conan, some modifications of the CMake files may be necessary.

Quetzal also comes with a dedicated Docker Image called **Quetzal-NEST** that can be described as a 
self-contained package that includes everything needed to run a Quetzal-based application, such as the 
code, dependencies, and runtime environment.

The "Remote - Containers" extension for Visual Studio Code is a tool we use that lets you develop projects inside isolated environments called containers. These containers provide a consistent and reproducible setup for the development, making it easier to work with complex dependencies.

For those with a background in Biological Sciences, here's an overview of these resources, as you may not be familiar with them.

---

### CMake

[CMake](https://cmake.org/cmake/help/latest/manual/cmake.1.html) is the C++ build
systems first choice for cross-platform development. 

Technically, CMake is a build
system generator but the level of abstraction it offers allows us to consider
it as a cross-platform build system.

Users can build, test, and install packages with `cmake` and `ctest` commands.

@note
Please refer to the [CMake official documentation](https://cmake.org/install/)
for installation on your specific OS.

---

### Conan

[Conan](https://conan.io/) is one of the leading options for cross-platform package
manager with C/C++ projects. 

We chose it because it interfaces with CMake in a nice
way. Conan will handle the dependencies and version conflicts management, and pass
the paths of the installed dependencies to CMake so it can build the project.

@note
Please refer to the [Conan official documentation](https://docs.conan.io/en/latest/installation.html)
for installation on your specific OS.

After installing Conan, you may need a custom profile file in `~/.conan/profiles` directory.

[Profiles](https://docs.conan.io/en/latest/reference/profiles.html) simply allow users to define a
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

@note
Quetzal-CoaTL requires C++20 to be activated either in a Conan profile or with Conan command line.
By setting the option once for all in the profile, you will avoid typing `-s compiler.cppstd=20`
every time you run a Conan command.

---

### Docker

A Docker image is a lightweight, standalone, and executable package that contains everything needed to run a piece of software, including the code, runtime environment, libraries, dependencies, and system tools. It is created from a Dockerfile, which specifies the instructions to build the image.

Think of a Docker image as a snapshot of a software application or service at a specific point in time. It is designed to be portable and can be run consistently across different computing environments that have Docker installed, without worrying about compatibility issues or dependency conflicts.

Docker images are stored in a registry, such as Docker Hub or a private registry, and can be easily shared and distributed. When running a Docker image, it is instantiated as a container, which is a running instance of the image. Multiple containers can be created from the same image, each running independently and isolated from one another.

**Quetzal** projects are all hosted on the same Docker image that we call **Quetzal-NEST**. If you are able to get this Docker 
container up and running, you will avoid most of the dependencies hell.

To run the Docker image, you can follow these steps:

1. **Install Docker:** 

  Ensure that Docker is installed on your system. Docker provides platform-specific installers for various operating systems, such as Windows, macOS, and Linux. You can download the appropriate installer from the Docker website and follow the installation instructions.

2. **Pull the Docker image:** 

  If you haven't already, you need to pull the Docker image from the Docker Hub registry. You can use the following command in your terminal or command prompt to pull an image: 
  
  `docker pull arnaudbecheler/quetzal-nest`

3. **Run the Docker image as a container:** 

  Once the image is available locally, you can run it as a container using the docker `run` command: 
  
  `docker run -it --entrypoint bash arnaudbecheler/quetzal-nest`. 
  
  This command starts a container based on the specified image. Breaking down the command:
  - `docker run` is the command to run a Docker container.
  - `-it` is a combination of options: 
    - `-i` enables interactive mode to allow user input
    - `-t` allocates a pseudo-TTY (terminal).
  - `--entrypoint bash` specifies that the entry point for the container should be the Bash shell.
  - `arnaudbecheler/quetzal-nest` is the name of the Docker image used to create the container.

  By running this command, you will start a container based on the `arnaudbecheler/quetzal-nest` image and gain an interactive shell prompt within the container. This allows you to execute commands and interact with the container's filesystem and environment using the Bash shell. You can also add additional options to the docker run command to customize the container's behavior, such as exposing ports, mounting volumes, setting environment variables, and more. Refer to the Docker documentation for detailed information on available options.

4. **Interact with the running container:** 

  After running the Docker image, you can interact with the container. Since it's a command-line application, you can interact with it through the terminal or command prompt.

5. **Manage the container:** 

  You can view the running containers on your system using the `docker ps` command. To stop a running container, you can use the `docker stop arnaudbecheler/quetzal-nest`  command followed by the container ID or name.

These steps provide a basic overview of running a Docker image as a container. Docker offers many additional features and commands to manage containers. For more advanced usage, it's recommended to explore the Docker documentation and resources available online. Don't hesitate to ask for help by opening a Github issue on the **Quetzal** repository.
