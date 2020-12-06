#!/usr/bin/env bash

# get doxygen, LaTex and m.css set up for documentation
pip3 install jinja2 Pygments

# TinyTeX
sudo chown -R $(whoami) /usr/local/bin
curl -sL "https://yihui.org/tinytex/install-bin-unix.sh" | sh
