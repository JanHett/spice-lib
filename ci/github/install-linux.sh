#!/usr/bin/env bash

# TinyTeX for docs
wget -qO- "https://yihui.org/tinytex/install-bin-unix.sh" | sh

# Which distro are we on?
. /etc/os-release
# OS=$NAME
# VER=$VERSION_ID

echo "Running on $NAME v$VERSION_ID"

PACKAGE_MANAGER="apt-get"

# Bare-metal GitHub runners don't have setuptools installed
# we need to install them for conan
pip3 install wheel setuptools
# Install Conan
pip3 install conan
# bare-metal GitHub runners need some help exposing conan
echo "/home/runner/.local/bin" >> $GITHUB_PATH

sudo $PACKAGE_MANAGER install -y doxygen

# install llvm
sudo bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)"

# Conan-provided dependencies will be installed during configuration
