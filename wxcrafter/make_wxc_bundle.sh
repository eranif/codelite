#!/bin/sh

fix_shared_object_depends() {
    search_string=$1

    ## Get list of files to work on
    dylibs=`ls ./wxcrafter.app/Contents/MacOS/*.dylib`
    file_list="${dylibs} ${sos2} ${sos3} ./wxcrafter.app/Contents/MacOS/wxcrafter "

    ## Since the plugins must use the same wx configuration as the
    ## executable, we can run the following command only once and
    ## use the results to manipulate the plugins as well
    orig_path=`otool -L ../bin/wxcrafter  | grep ${search_string} | awk '{print $1;}'`

    ## Loop over the files, and update the path of the wx library
    for file in ${file_list}
    do
        echo "Fixing file $file..."
            for path in ${orig_path}
            do
                    new_path=`echo ${path} | xargs basename`
                    install_name_tool -change ${path} @executable_path/../MacOS/${new_path} ${file}
            done
    done
}

fix_non_plugins_depends() {
    search_string=$1
    ## Get list of files to work on
    dylibs=`ls ./wxcrafter.app/Contents/MacOS/*.dylib`
    file_list="${dylibs} ${sos2} ${sos3} ./wxcrafter.app/Contents/MacOS/wxcrafter "
 
    for SO in ${file_list}
    do   
    orig_path=`otool -L ${SO}  | grep ${search_string} | awk '{print $1;}'`
    if [ ! -z ${orig_path} ]; then
    ## Loop over the files, and update the path of the wx library
    for file in ${file_list}
    do
        new_path=`echo ${orig_path} | xargs basename`
        install_name_tool -change ${orig_path} @executable_path/../MacOS/${new_path} ${file}
    done
    fi
    done
}


## extract the file name from the Makefile
exe_name=wxcrafter

rm -rf wxcrafter.app
mkdir -p ./wxcrafter.app/Contents/MacOS
mkdir -p ./wxcrafter.app/Contents/Resources
mkdir -p ./wxcrafter.app/Contents/SharedSupport
mkdir -p ./wxcrafter.app/Contents/SharedSupport/config

wx_file_list=`otool -L ../bin/wxcrafter  | grep libwx_* | awk '{print $1;}'`

# fix the script
echo "Running install_name_tool..."

# copy the libs locally, the script will have an easier time finding them this way
mkdir -p lib
cp ../lib/*.dylib ./lib

## copy the wx dlls to the executable path which under Mac is located at ./codelite.app/Contents/MacOS/
for wx_file in ${wx_file_list}
do
    cp ${wx_file} ./wxcrafter.app/Contents/MacOS/
done
cp ../bin/wxcrafter ./wxcrafter.app/Contents/MacOS/wxcrafter

cp -r ../../Runtime/lexers ./wxcrafter.app/Contents/SharedSupport/
cp -pr ../../wxcrafter/wxgui.zip ./wxcrafter.app/Contents/SharedSupport/

## copy default Mac configuration file
cp ../../Runtime/config/codelite.xml.default.mac ./wxcrafter.app/Contents/SharedSupport/config/codelite.xml.default

## replace the executable name according to the configuration used in the build
cp ../../wxcrafter/Info.plist ./wxcrafter.app/Contents/Info.plist

## License
cp ../../wxcrafter/resources/*.icns ./wxcrafter.app/Contents/Resources/

cp ../lib/libplugin.dylib ./wxcrafter.app/Contents/MacOS/
cp ../lib/liblibcodelite.dylib ./wxcrafter.app/Contents/MacOS/
cp ../lib/libwxsqlite3.dylib ./wxcrafter.app/Contents/MacOS/

fix_shared_object_depends libwx_
fix_non_plugins_depends lib/liblibcodelite.dylib
fix_non_plugins_depends lib/libplugin.dylib
fix_non_plugins_depends lib/libwxsqlite3.dylib


