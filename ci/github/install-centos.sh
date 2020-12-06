#!/usr/bin/env bash

# TinyTeX for docs
wget -qO- "https://yihui.org/tinytex/install-bin-unix.sh" | sh

# Which distro are we on?
. /etc/os-release
# OS=$NAME
# VER=$VERSION_ID

echo "Running on $NAME v$VERSION_ID"

PACKAGE_MANAGER="yum"

sudo $PACKAGE_MANAGER install -y doxygen
sudo $PACKAGE_MANAGER install -y fftw-devel

# build OIIO from source

git clone https://github.com/OpenImageIO/oiio.git __temp/oiio
cd __temp/oiio

# build and install pybind11
sh ./src/build-scripts/build_pybind11.bash

# build OIIO
make
