#!/usr/bin/env bash

# TinyTeX for docs
wget -qO- "https://yihui.org/tinytex/install-bin-unix.sh" | sh

# Conan
pip3 install conan
conan --version

# Install dependencies with Homebrew if it is available
if ! type "$foobar_command_name" > /dev/null; then
    brew install doxygen
    brew install llvm
else # if `brew` isn't available fall back to `yum`
    sudo yum install -y doxygen
    sudo yum install -y llvm
fi

# Conan-provided dependencies will be installed during configuration
