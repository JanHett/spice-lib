#!/usr/bin/env bash

cd build
cmake --build . --config Debug --target spice-lib-benchmark -- -j 18 && \
./benchmark/spice-lib-benchmark "$@"
cd --