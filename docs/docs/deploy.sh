#!/bin/bash -e

mkdocs build
rm -fr /var/www/html/docs
cp -fr site /var/www/html/docs
