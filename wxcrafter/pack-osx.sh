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

os_name=`uname -s`
if [ "${os_name}" == "Darwin" ]; then
    mkdir -p ../build-release-app/pack
    cp make_wxc_bundle.sh ../build-release-app/pack
    chmod +x ../build-release-app/pack/make_wxc_bundle.sh
fi

cd ../build-release-app
echo `pwd`
cmake -DCMAKE_BUILD_TYPE=Release -DWXC_APP=1 -DCOPY_WX_LIBS=1 .. -DNO_CORE_PLUGINS=1 -DSDK_ONLY=1 -DWITH_PCH=1
echo "Building..."
make -j4 

    cd pack && ./make_wxc_bundle.sh
    rm -f wxcrafter.app.tar.gz
    tar cvfz wxcrafter.app.tar.gz wxcrafter.app/ 

scp wxcrafter.app.tar.gz root@codelite.org:/var/www/html/downloads/wxCrafter/wip
