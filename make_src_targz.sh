# export the current directory

################################################################################
## This file is part of CodeLite IDE and is released
## under the terms of the GNU General Public License as published by
##    the Free Software Foundation; either version 2 of the License, or
##    (at your option) any later version.
################################################################################

cur_rev=`svn info | grep Revision | awk '{print $2;}'`
curdir=`pwd`
codelite_ver="codelite-3.5.0"

rm -fr /tmp/${codelite_ver}.${cur_rev}/

svn export . /tmp/${codelite_ver}.${cur_rev}/

## set correct permissions

cp ./LiteEditor/svninfo.cpp /tmp/${codelite_ver}.${cur_rev}/LiteEditor/
echo "Creating tarballs"
cd /tmp/
rm -fr /tmp/${codelite_ver}.${cur_rev}/sdk/curl

# First for Linux
tar cvfz ${curdir}/${codelite_ver}.${cur_rev}-gtk.src.tar.gz ${codelite_ver}.${cur_rev}/* --exclude *.exe --exclude *.dll --exclude *.dylib

# then for MSWin
tar cv --lzma -f ${curdir}/${codelite_ver}.${cur_rev}-win.src.tar.7z ${codelite_ver}.${cur_rev}/* --exclude *.so --exclude *.dylib

# and OS/X
tar cvfz ${curdir}/${codelite_ver}.${cur_rev}-mac.src.tar.gz ${codelite_ver}.${cur_rev}/* --exclude *.exe --exclude *.dll --exclude *.so
