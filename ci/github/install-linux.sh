#!/usr/bin/env bash

# # TinyTeX for docs
# wget -qO- "https://yihui.org/tinytex/install-bin-unix.sh" | sh

# # All the packages needed to build the docs. Isn't TeX marvellous...?
# sudo tlmgr install standalone
# sudo tlmgr install preview
# sudo tlmgr install ucs
# sudo tlmgr install was
# sudo tlmgr install newtx
# sudo tlmgr install fontaxes
# sudo tlmgr install xstring
# sudo tlmgr install carlisle
# sudo tlmgr install collection-fontsrecommended
# # sudo tlmgr install collection-fontsextra # probably not needed

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

# download llvm and install it
LLVM_BINARIES_NAME="clang+llvm-11.0.0-x86_64-linux-gnu-ubuntu-16.04"
wget -q -O \
    llvm-prebuilt.tar.xz \
    https://github.com/llvm/llvm-project/releases/download/llvmorg-11.0.0/${LLVM_BINARIES_NAME}.tar.xz

tar xf llvm-prebuilt.tar.xz

mv ${LLVM_BINARIES_NAME} local_llvm

# Conan-provided dependencies will be installed during configuration
