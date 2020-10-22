#!/usr/bin/env bash

mkdir build
cd build
# TODO: make sure LLVM_DIR is not set for OSes other than OSX
cmake ..  -DCMAKE_BUILD_TYPE=Debug -DLLVM_DIR=/usr/local/opt/llvm/lib/cmake/llvm/ -DCMAKE_INSTALL_PREFIX=../../spice_install
cmake --build . --config Debug
tests/spice-lib-test
cmake --build . --config Debug --target doc
