#!/bin/bash

CURRENT_DIR=`pwd`
rm -fr /tmp/codelite-icons
svn export . /tmp/codelite-icons --force
cd /tmp/codelite-icons
zip -r codelite-icons-fresh-farm.zip *
mv codelite-icons-fresh-farm.zip $CURRENT_DIR/../Runtime
cd $CURRENT_DIR

