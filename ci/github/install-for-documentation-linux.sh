#!/usr/bin/env bash

# Which distro are we on?
. /etc/os-release
# OS=$NAME
# VER=$VERSION_ID

echo "Running on $NAME v$VERSION_ID"

PACKAGE_MANAGER="apt-get"

sudo $PACKAGE_MANAGER install -y texlive-extra-utils

# TinyTeX for docs
wget -qO- "https://yihui.org/tinytex/install-bin-unix.sh" | sh

echo "$HOME/bin" >> $GITHUB_PATH

# All the packages needed to build the docs. Isn't TeX marvellous...?
sudo $HOME/bin/tlmgr install standalone
sudo $HOME/bin/tlmgr install preview
sudo $HOME/bin/tlmgr install ucs
sudo $HOME/bin/tlmgr install was
sudo $HOME/bin/tlmgr install newtx
sudo $HOME/bin/tlmgr install fontaxes
sudo $HOME/bin/tlmgr install xstring
sudo $HOME/bin/tlmgr install carlisle
sudo $HOME/bin/tlmgr install collection-fontsrecommended
# sudo $HOME/bin/tlmgr install collection-fontsextra # probably not needed

sudo $PACKAGE_MANAGER install -y doxygen
