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
