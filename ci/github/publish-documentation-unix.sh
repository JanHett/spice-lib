#!/usr/bin/env bash

# create a temp branch
git checkout -b gh-pages-local

# remove everything except the built documentation
mkdir __swap__
mv * __swap__
mv __swap__/doc/html/* .
rm -rf __swap__

# push the doc
git add .
git commit -m "Deploy spice-lib documentation"
git push origin/gh-pages
