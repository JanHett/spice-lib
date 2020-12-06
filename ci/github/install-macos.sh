#!/usr/bin/env bash

# TinyTeX for docs
sudo chown -R $(whoami) /usr/local/bin
curl -sL "https://yihui.org/tinytex/install-bin-unix.sh" | sh

# Conan
sudo pip3 install conan
conan --version

brew install doxygen
brew install llvm

if [ ! -z ${INSTALL_GCC_VERSION} ]; then
    brew install gcc@${INSTALL_GCC_VERSION}
fi

# Conan-provided dependencies will be installed during configuration
