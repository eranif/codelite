#!/bin/bash

file=codelite-logo.svg
filename=codelite-logo

mkdir -p ../bitmaps/osx/icon.iconset
os_name=`uname -s`

if [ "${os_name}" != "Darwin" ]; then
    ## 16x16 version
    inkscape 16-$file --without-gui --export-png=../bitmaps/16-${filename}.png --export-width=16 --export-height=16 
    inkscape 32-$file --without-gui --export-png=../bitmaps/16-${filename}@2x.png --export-width=32 --export-height=32 
    cp ../bitmaps/16-${filename}.png ../bitmaps/osx/icon.iconset/icon_16x16.png
    cp ../bitmaps/16-${filename}@2x.png ../bitmaps/osx/icon.iconset/icon_16x16@2x.png

    ## 24x24 version
    inkscape 24-$file --without-gui --export-png=../bitmaps/24-${filename}.png --export-width=24 --export-height=24 
    inkscape 48-$file --without-gui --export-png=../bitmaps/24-${filename}@2x.png --export-width=48 --export-height=48 
    cp ../bitmaps/24-${filename}.png ../bitmaps/osx/icon.iconset/icon_24x24.png
    cp ../bitmaps/24-${filename}@2x.png ../bitmaps/osx/icon.iconset/icon_24x24@2x.png

    ## 32x32 version
    inkscape 32-$file --without-gui --export-png=../bitmaps/32-${filename}.png --export-width=32 --export-height=32 
    inkscape 64-$file --without-gui --export-png=../bitmaps/32-${filename}@2x.png --export-width=64 --export-height=64 
    cp ../bitmaps/32-${filename}.png ../bitmaps/osx/icon.iconset/icon_32x32.png
    cp ../bitmaps/32-${filename}@2x.png ../bitmaps/osx/icon.iconset/icon_32x32@2x.png

    ## 48x48 version
    inkscape 48-$file --without-gui --export-png=../bitmaps/48-${filename}.png --export-width=48 --export-height=48 
    inkscape    $file --without-gui --export-png=../bitmaps/48-${filename}@2x.png --export-width=96 --export-height=96 
    cp ../bitmaps/48-${filename}.png ../bitmaps/osx/icon.iconset/icon_48x48.png
    cp ../bitmaps/48-${filename}@2x.png ../bitmaps/osx/icon.iconset/icon_48x48@2x.png

    ## 64x64 version
    inkscape 64-$file --without-gui --export-png=../bitmaps/64-${filename}.png --export-width=64 --export-height=64 
    inkscape    $file --without-gui --export-png=../bitmaps/64-${filename}@2x.png --export-width=128 --export-height=128 
    cp ../bitmaps/64-${filename}.png ../bitmaps/osx/icon.iconset/icon_64x64.png
    cp ../bitmaps/64-${filename}@2x.png ../bitmaps/osx/icon.iconset/icon_64x64@2x.png

    ## 128x128 version
    inkscape $file --without-gui --export-png=../bitmaps/128-${filename}.png --export-width=128 --export-height=128 
    inkscape $file --without-gui --export-png=../bitmaps/128-${filename}@2x.png --export-width=256 --export-height=256 
    cp ../bitmaps/128-${filename}.png ../bitmaps/osx/icon.iconset/icon_128x128.png
    cp ../bitmaps/128-${filename}@2x.png ../bitmaps/osx/icon.iconset/icon_128x128@2x.png

    ## 256x256 version
    inkscape $file --without-gui --export-png=../bitmaps/256-${filename}.png --export-width=256 --export-height=256 
    inkscape $file --without-gui --export-png=../bitmaps/256-${filename}@2x.png --export-width=512 --export-height=512 
    cp ../bitmaps/256-${filename}.png ../bitmaps/osx/icon.iconset/icon_256x256.png
    cp ../bitmaps/256-${filename}@2x.png ../bitmaps/osx/icon.iconset/icon_256x256@2x.png

    ## 512x512 version
    inkscape $file --without-gui --export-png=../bitmaps/512-${filename}.png --export-width=512 --export-height=512 
    inkscape $file --without-gui --export-png=../bitmaps/512-${filename}@2x.png --export-width=1024 --export-height=1024 
    cp ../bitmaps/512-${filename}.png ../bitmaps/osx/icon.iconset/icon_512x512.png
    cp ../bitmaps/512-${filename}@2x.png ../bitmaps/osx/icon.iconset/icon_512x512@2x.png
fi

cd ../bitmaps

##
## Create a zip file named codelite-bitmaps.zip
##
rm -f ../Runtime/codelite-bitmaps.zip
zip ../Runtime/codelite-bitmaps.zip *.png

if [ "${os_name}" == "Darwin" ]; then
    cd osx
    echo iconutil -c icns icon.iconset/
    iconutil -c icns icon.iconset/
fi
