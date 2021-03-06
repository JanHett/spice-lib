#!/usr/bin/env bash

# get doxygen, LaTex and m.css set up for documentation
pip3 install jinja2 Pygments

# TinyTeX
sudo chown -R $(whoami) /usr/local/bin
curl -sL "https://yihui.org/tinytex/install-bin-unix.sh" | sh

# All the packages needed to build the docs. Isn't TeX marvellous...?
sudo tlmgr install standalone
sudo tlmgr install preview
sudo tlmgr install ucs
sudo tlmgr install was
sudo tlmgr install newtx
sudo tlmgr install fontaxes
sudo tlmgr install xstring
sudo tlmgr install carlisle
sudo tlmgr install collection-fontsrecommended
# sudo tlmgr install collection-fontsextra # probably not needed
