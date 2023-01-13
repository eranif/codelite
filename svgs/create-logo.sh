#!/bin/bash -x

# Locate the workspace folder
CL_HOME=$(pwd)
while [ 1 ]; do
    if [ -f CodeLiteIDE-MSW.workspace ]; then
        CL_HOME=$(pwd)
        echo "-- CL_HOME is set to ${CL_HOME}"
        break
    fi
    cd ..
    if [ $(pwd) = "/" ]; then
        echo -e "Could not locate CodeLite's root directory"
        exit 1
    fi
done

cd ${CL_HOME}/svgs
filename=codelite-logo

mkdir -p logo/osx/icon.iconset
OS_NAME=$(uname -s)

file=${CL_HOME}/svgs/dark-theme/codelite-logo.svg
if [ "${OS_NAME}" != "Darwin" ]; then
    ## 16x16 version
    inkscape -w 16 -h 16 $file -o ${CL_HOME}/svgs/logo/16-${filename}.png
    inkscape -w 32 -h 32 $file -o ${CL_HOME}/svgs/logo/16-${filename}@2x.png
    cp ${CL_HOME}/svgs/logo/16-${filename}.png ${CL_HOME}/svgs/logo/osx/icon.iconset/icon_16x16.png
    cp ${CL_HOME}/svgs/logo/16-${filename}@2x.png ${CL_HOME}/svgs/logo/osx/icon.iconset/icon_16x16@2x.png

    ## 24x24 version, not needed for OSX
    inkscape -w 24 -h 24 $file -o ${CL_HOME}/svgs/logo/24-${filename}.png
    inkscape -w 48 -h 48 $file -o ${CL_HOME}/svgs/logo/24-${filename}@2x.png

    ## 32x32 version
    inkscape -w 32 -h 32 $file -o ${CL_HOME}/svgs/logo/32-${filename}.png
    inkscape -w 64 -h 64 $file -o ${CL_HOME}/svgs/logo/32-${filename}@2x.png
    cp ${CL_HOME}/svgs/logo/32-${filename}.png ${CL_HOME}/svgs/logo/osx/icon.iconset/icon_32x32.png
    cp ${CL_HOME}/svgs/logo/32-${filename}@2x.png ${CL_HOME}/svgs/logo/osx/icon.iconset/icon_32x32@2x.png

    ## 64x64 version
    inkscape -w 64 -h 64 $file -o ${CL_HOME}/svgs/logo/64-${filename}.png
    inkscape -w 128 -h 128 $file -o ${CL_HOME}/svgs/logo/64-${filename}@2x.png

    ## Use the hi-res image from hereon
    ## 128x128 version
    inkscape -w 128 -h 128 $file -o ${CL_HOME}/svgs/logo/128-${filename}.png
    inkscape -w 256 -h 256 $file -o ${CL_HOME}/svgs/logo/128-${filename}@2x.png
    cp ${CL_HOME}/svgs/logo/128-${filename}.png ${CL_HOME}/svgs/logo/osx/icon.iconset/icon_128x128.png
    cp ${CL_HOME}/svgs/logo/128-${filename}@2x.png ${CL_HOME}/svgs/logo/osx/icon.iconset/icon_128x128@2x.png

    ## 256x256 version
    inkscape -w 256 -h 256 $file -o ${CL_HOME}/svgs/logo/256-${filename}.png
    inkscape -w 512 -h 512 $file -o ${CL_HOME}/svgs/logo/256-${filename}@2x.png
    cp ${CL_HOME}/svgs/logo/256-${filename}.png ${CL_HOME}/svgs/logo/osx/icon.iconset/icon_256x256.png
    cp ${CL_HOME}/svgs/logo/256-${filename}@2x.png ${CL_HOME}/svgs/logo/osx/icon.iconset/icon_256x256@2x.png

    ## 512x512 version
    inkscape -w 512 -h 512 $file -o ${CL_HOME}/svgs/logo/512-${filename}.png
    inkscape -w 1024 -h 1024 $file -o ${CL_HOME}/svgs/logo/512-${filename}@2x.png
    cp ${CL_HOME}/svgs/logo/512-${filename}.png ${CL_HOME}/svgs/logo/osx/icon.iconset/icon_512x512.png
    cp ${CL_HOME}/svgs/logo/512-${filename}@2x.png ${CL_HOME}/svgs/logo/osx/icon.iconset/icon_512x512@2x.png
fi

cd logo

if [ "${OS_NAME}" == "Darwin" ]; then
    cd ${CL_HOME}/svgs/logo/osx
    echo iconutil -c icns icon.iconset/
    iconutil -c icns icon.iconset/
fi

if [[ "${OS_NAME}" == *"MSYS_NT"* ]]; then
    # create codelite-log.ico
    CL_HOME_NATIVE=$(cygpath -w ${CL_HOME})
    png2ico ${CL_HOME}/LiteEditor/codelite-logo.ico         \
            ${CL_HOME}/svgs/logo/16-codelite-logo.png       \
            ${CL_HOME}/svgs/logo/24-codelite-logo.png       \
            ${CL_HOME}/svgs/logo/24-codelite-logo@2x.png    \
            ${CL_HOME}/svgs/logo/32-codelite-logo.png       \
            ${CL_HOME}/svgs/logo/64-codelite-logo.png       \
            ${CL_HOME}/svgs/logo/128-codelite-logo.png
fi
