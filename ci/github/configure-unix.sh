#!/usr/bin/env bash

mkdir build
cd build

# Configure the build. Will also install submodules and conan deps.
cmake .. \
    -DCMAKE_C_COMPILER=${CC} \
    -DCMAKE_CXX_COMPILER=${CXX} \
    -DCMAKE_BUILD_TYPE=Debug \
    "$@"
