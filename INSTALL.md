# Installing spice

## Prerequisites

- [C++17](https://en.cppreference.com/w/cpp/compiler_support)
- [CMake 3.16](https://cmake.org/)

## Dependencies

### For the library

- [OpenImageIO](https://github.com/OpenImageIO/oiio) [1][2]
- [FFTW](http://fftw.org/) [1][2]
- [Halide](https://halide-lang.org/) [3]
- [LLVM](http://llvm.org/) - this is a dependency of Halide [1]

### For the tests

- [Google Test](https://github.com/google/googletest) [4]
- [Google Benchmark](https://github.com/google/benchmark) [3]

### For the documentation

- [Doxygen](http://www.doxygen.nl/) [1]
- [m.css](https://github.com/mosra/m.css) (included as a git submodule, will be pulled by CMake during configuration if building documentation is enabled)

> [1] On macOS you can install these dependencies by running `brew bundle install`. Linuxbrew _might_ work, too.
>
> [2] These can also be pulled in via `conan`. If you set `USE_CONAN=ON`, they will be automatically installed during configuration.
>
> [3] These dependencies are included as git submodules. They will be pulled by CMake during configuration if the relevant target is enabled.
>
> [4] These dependencies will be automatically pulled with CMake's `FetchContent` module during configuration.

### Supported operating systmes and compilers

| Operating system | Compiler | Versions |
| --- | --- | --- |
| macOS | Apple `clang` | 11 |
| Linux (tested on CentOS and Ubuntu) | `clang` | 11 |
| Linux (tested on CentOS and Ubuntu) | `gcc` | > 7 |

## Building spice as a CMake subdirectory

The recommended way to install spice-lib is to include it as a subdirectory in your CMake build.

```CMake
# Set the minimum CMake version - spice-lib itself uses 3.16, so we'll follow
# this example
cmake_minimum_required(VERSION 3.16)

# Give the project a name
project("spice-lib-cmake-demo")

# Some boilerplate to make sure we find system libraries - depending on your
# setup you might not need this
include(GNUInstallDirs)

if(APPLE)
    # Fix linking on 10.14+. See https://stackoverflow.com/questions/54068035
    LINK_DIRECTORIES(/usr/local/lib)
endif()

# Optional: automatically pull spice-lib. It must be set up as a git submodule
# for this to work
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/spice-lib/CMakeLists.txt")
    # we couldn't find spice-lib
    message("Unable to find spice-lib. Attempting to pull.")

    # we have a submodule set up for spice-lib, so we clone it
    execute_process(COMMAND git submodule update --init -- external/spice-lib
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()

# Include spice-lib from a subdirectory
add_subdirectory(external/spice-lib)

# Set up a target for our toy application
add_executable(${PROJECT_NAME})

# Link spice-lib to our application
target_link_libraries(${PROJECT_NAME} PRIVATE
    spice-lib)
```

Depending on your system configuration you may or may not have to set the CMake options for Halide during the configuration as shown below.

## Building it in standalone mode (e.g. for tests or development)

```bash
$ cd <spice repo directory>
$ mkdir build
$ cd build
# configure
$ cmake .. \
    [-DUSE_CONAN=OFF|ON] \
    # Conan will install the libraries with the corresponding compiler \
    [-DCMAKE_C_COMPILER=clang|gcc[-<version>]] \
    [-DCMAKE_CXX_COMPILER=clang++|g++[-<version>]] \
    -DCMAKE_INSTALL_PREFIX=<"where spice should be installed"> \
    # tests and docs are enabled by default, use the options to override this \
    [-DENABLE_TESTS=OFF|ON] [-DENABLE_DOCS=OFF|ON] \
    # some additional info for Halide \
    -DCMAKE_BUILD_TYPE=Debug|Release \
    -DLLVM_DIR=<"where your llvm installation's CMake info is">
# build
$ cmake --build . --config Debug|Release [--target <"target name">]
# if you chose to build the tests, you can test if everything works
tests/spice-lib-test
```
The following target names are supported:

| Target | Description |
| --- | --- |
| `doc` | Build the documentation |
| `spice-lib-test` | Build the unit tests |
| `spice-lib-benchmark` | Build the benchmark suite |
| `install` | Install spice-lib system wide |


## Notes

spice has been tested to build with clang on macOS and may or may not build in other configurations. I plan to extend support to other operating systems and platforms, but for now, features are the primary focus.