#!/usr/bin/env bash

mkdir build
cd build
cmake ..  -DCMAKE_BUILD_TYPE=Debug -DLLVM_DIR=/usr/local/opt/llvm/lib/cmake/llvm/ -DCMAKE_INSTALL_PREFIX=../../spice_install
cmake --build . --config Debug
tests/spice-lib-test
cmake --build . --config Debug --target doc
