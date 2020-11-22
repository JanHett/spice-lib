#!/usr/bin/env bash

# get doxygen, LaTex and m.css set up for documentation
pip3 install jinja2 Pygments

# TinyTeX
sudo chown -R $(whoami) /usr/local/bin
curl -sL "https://yihui.org/tinytex/install-bin-unix.sh" | sh

# clone, compile and make Google test
# git clone https://github.com/google/googletest.git ./external/gtest
# cd ./external/gtest
# git checkout tags/release-1.10.0
# mkdir build
# cd build
# cmake -Dgtest_build_samples=OFF -Dgtest_build_tests=OFF ../
# make
# make install
