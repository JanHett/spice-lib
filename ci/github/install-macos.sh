#!/usr/bin/env bash

# Conan
sudo pip3 install conan
conan --version

brew install llvm

if [ ! -z ${INSTALL_GCC_VERSION} ]; then
    brew install gcc@${INSTALL_GCC_VERSION}
fi

# Conan-provided dependencies will be installed during configuration
