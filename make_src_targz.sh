################################################################################
## This file is part of CodeLite IDE and is released
## under the terms of the GNU General Public License as published by
##    the Free Software Foundation; either version 2 of the License, or
##    (at your option) any later version.
################################################################################

# Create tarballs suitable for the 3 platforms
# The script assumes that git HEAD has a suitable tag

curdir=`pwd`
HEAD=`git describe --tags`
# HEAD should have been tagged, but the tag probably reads vX.Y. If so, amputate the 'v'
initial=$(echo ${HEAD} | cut -c1)
if [ ${initial} = "v" ]; then
  HEAD=$(echo ${HEAD} | cut -c2-)
fi

codelite_ver="codelite-${HEAD}"
temptarball="CL-"`git rev-parse HEAD`

tmpdir="/tmp/"
rm -f ${tmpdir}${temptarball}
rm -fr ${tmpdir}${codelite_ver}

# Create a temporary tarball and extract it
git archive --format=tar --prefix=${codelite_ver}/ HEAD -o ${tmpdir}/${temptarball}
(cd ${tmpdir} && tar -xf ${temptarball} && rm -f ${temptarball})

# Create an up-to-date LiteEditor/autoversion.cpp and copy it into the 'archive'
$(./git-revision.sh)
cp ./LiteEditor/autoversion.cpp ${tmpdir}/${codelite_ver}/LiteEditor/

rm -fr ${tmpdir}/${codelite_ver}/sdk/curl

echo "Creating tarballs"
cd ${tmpdir}

# First for Linux
GZIP="-9" tar cvzf ${curdir}/${codelite_ver}-gtk.src.tar.gz ${codelite_ver}/* --exclude *.exe --exclude *.dll --exclude *.dylib
tar cv --xz -f ${curdir}/${codelite_ver}-gtk.src.tar.xz ${codelite_ver}/* --exclude *.exe --exclude *.dll --exclude *.dylib

# then for MSWin
#tar cv --lzma -f ${curdir}/${codelite_ver}-win.src.tar.7z ${codelite_ver}/* --exclude *.so --exclude *.dylib

# and OS/X
#GZIP="-9" tar cvzf ${curdir}/${codelite_ver}-mac.src.tar.gz ${codelite_ver}/* --exclude *.exe --exclude *.dll --exclude *.so

# Clean up
rm -rf ${codelite_ver}
