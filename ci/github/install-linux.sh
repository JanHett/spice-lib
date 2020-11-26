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
fi

sudo $PACKAGE_MANAGER install -y doxygen
# sudo $PACKAGE_MANAGER install -y llvm
if [ $NAME = 'Ubuntu' ]; then
    # if command -v "brew" > /dev/null; then
    #     # Install dependencies with Homebrew if it is available (i.e. if we are
    #     # running on a bare-metal GitHub action runner)
    #     # brew install doxygen
    #     brew install llvm
    # else
    #     # if `brew` isn't available for some reason fall back to the distro's
    #     # package manager
    #     sudo $PACKAGE_MANAGER install -y llvm
    # fi

    # download llvm and install it
    LLVM_BINARIES_NAME="clang+llvm-11.0.0-x86_64-linux-gnu-ubuntu-16.04"
    wget -q -O \
        llvm-prebuilt.tar.xz \
        https://github.com/llvm/llvm-project/releases/download/llvmorg-11.0.0/${LLVM_BINARIES_NAME}.tar.xz
    
    tar xf llvm-prebuilt.tar.xz

    mv ${LLVM_BINARIES_NAME} local_llvm

    echo "$(pwd)/local_llvm/bin" >> $GITHUB_PATH
else
    echo "No need to install LLVM on ASWF container"
    # sudo yum install -y centos-release-scl

    # sudo yum-config-manager --enable rhel-server-rhscl-7-rpms

    # sudo yum install -y llvm-toolset-9.0

    # scl enable llvm-toolset-9.0 bash
    # sudo $PACKAGE_MANAGER install -y llvm-toolset-7

    # git clone --depth 1 --branch llvmorg-10.0.0 https://github.com/llvm/llvm-project.git

    # mkdir llvm-build
    # cd llvm-build
    # cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../llvm-install \
    #       -DLLVM_ENABLE_PROJECTS="clang;lld" \
    #       -DLLVM_TARGETS_TO_BUILD="X86;ARM;NVPTX;AArch64;Mips;Hexagon" \
    #       -DLLVM_ENABLE_TERMINFO=OFF -DLLVM_ENABLE_ASSERTIONS=ON \
    #       -DLLVM_ENABLE_EH=ON -DLLVM_ENABLE_RTTI=ON -DLLVM_BUILD_32_BITS=OFF \
    #       ../llvm-project/llvm
    # cmake --build . --target install
fi

# Conan-provided dependencies will be installed during configuration
