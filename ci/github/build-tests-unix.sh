#!/usr/bin/env bash

cd build

cmake --build . --config Release --target spice-lib-test -- -j 2
