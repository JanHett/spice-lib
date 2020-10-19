#!/usr/bin/env bash

cd build
cmake --build . --config Debug --target spice-lib-test -- -j 18 && \
./tests/spice-lib-test
cd --