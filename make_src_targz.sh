# export the current directory

################################################################################
## This file is part of CodeLite IDE and is released
## under the terms of the GNU General Public License as published by
##    the Free Software Foundation; either version 2 of the License, or
##    (at your option) any later version.
################################################################################

cur_rev=`svn info | grep Revision | awk '{print $2;}'`
curdir=`pwd`
rm -fr /tmp/codelite-${cur_rev}/

svn export . /tmp/codelite-2.1.0.${cur_rev}/

## set correct permissions

cp ./LiteEditor/svninfo.cpp /tmp/codelite-2.1.0.${cur_rev}/LiteEditor/
echo "Creating tar-ball"
cd /tmp/
rm -fr /tmp/codelite-2.1.0.${cur_rev}/sdk/curl
tar cvfz ${curdir}/codelite-2.1.0.${cur_rev}.tar.gz codelite-2.1.0.${cur_rev}/*
