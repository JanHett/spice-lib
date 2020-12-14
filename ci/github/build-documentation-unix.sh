#!/usr/bin/env bash

# build the doc
cd build
cmake --build . --config Release --target doc

# copy CNAME file to deployed directory
cd ../doc
cp CNAME html
