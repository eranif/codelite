#!/bin/bash
PWD=`pwd`
base_name=`basename $PWD`
echo $base_name
if [ "$base_name" != "wxcrafter" ] ; then
	echo "You must run this script from within wxcrafter folder"
	exit 1
fi

## Update codelite sources
cd ..
echo `pwd`
echo "[codelite] git pull"
git pull --rebase

## Update wxC sources
cd wxcrafter 
echo `pwd`
echo "[wxcrafter] git pull"
git pull --rebase

## Making build directory
mkdir -p ../build-release-app
cd ../build-release-app
echo `pwd`
cmake -DCMAKE_BUILD_TYPE=Release -DWXC_APP=1 -DCOPY_WX_LIBS=1 .. -DNO_CORE_PLUGINS=1 -DSDK_ONLY=1 
echo "Building..."
make -j4 && make package

# Upload the binary
deb_file=`ls -lt *.deb|grep -v Unspecified | awk '{print $9;}'|head -n 1`
echo Uploading deb file ${deb_file}
scp ${deb_file} root@codelite.org:/var/www/html/downloads/wxCrafter/wip
