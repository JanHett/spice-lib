#!/usr/bin/env bash

# TinyTeX for docs
wget -qO- "https://yihui.org/tinytex/install-bin-unix.sh" | sh

# Which distro are we on?
. /etc/os-release
# OS=$NAME
# VER=$VERSION_ID

echo "Running on $NAME v$VERSION_ID"

if [ $NAME = 'Ubuntu' ]; then
    PACKAGE_MANAGER="apt-get"
else
    PACKAGE_MANAGER="yum"
fi

# Install Conan
if [ $NAME = 'Ubuntu' ]; then
    # bare-metal GitHub runners don't have setuptools installed
    pip3 install wheel setuptools
fi
pip3 install conan
# sudo ln -s ~/.local/bin/conan /usr/local/bin/conan
if [ $NAME = 'Ubuntu' ]; then
    # bare-metal GitHub runners need some help exposing conan
    echo "/home/runner/.local/bin" >> $GITHUB_PATH
    # export PATH=$PATH:/home/runner/.local/bin
    # echo $PATH
    # sudo ln -s ~/.local/bin/conan /usr/local/bin/conan
fi

# if command -v "brew" > /dev/null; then
#     # Install dependencies with Homebrew if it is available (i.e. if we are
#     # running on a bare-metal GitHub action runner)
#     brew install doxygen
#     brew install llvm
# else
#     # if `brew` isn't available (i.e. in a CentOS container) fall back to the
#     # distro's package manager
#     sudo $PACKAGE_MANAGER install -y doxygen
#     sudo $PACKAGE_MANAGER install -y llvm
# fi

sudo $PACKAGE_MANAGER install -y doxygen
# sudo $PACKAGE_MANAGER install -y llvm
if [ $NAME = 'Ubuntu' ]; then
    sudo $PACKAGE_MANAGER install -y llvm-dev
else
    sudo yum install -y centos-release-scl

    sudo yum-config-manager --enable rhel-server-rhscl-7-rpms

    sudo yum install -y llvm-toolset-7.0

    scl enable llvm-toolset-7.0 bash
    # sudo $PACKAGE_MANAGER install -y llvm-toolset-7
fi

# Conan-provided dependencies will be installed during configuration
