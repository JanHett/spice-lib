#!/usr/bin/env bash

# # TinyTeX for docs
# sudo chown -R $(whoami) /usr/local/bin
# curl -sL "https://yihui.org/tinytex/install-bin-unix.sh" | sh

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
# # sudo $HOME/bin/tlmgr install collection-fontsextra # probably not needed

brew install doxygen
brew cask install mactex-no-gui

echo "Setting up latex paths..."
eval "$(/usr/libexec/path_helper)"
