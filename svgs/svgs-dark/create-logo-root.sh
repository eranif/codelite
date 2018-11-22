#!/bin/bash
filename=codelite-logo-root

mkdir -p ../../bitmaps-dark/osx/icon.iconset
os_name=`uname -s`

if [ "${os_name}" != "Darwin" ]; then
    ## 16x16 version
    file=codelite-logo-low-res-root.svg
    inkscape $file --without-gui --export-png=../../bitmaps-dark/16-${filename}.png --export-width=16 --export-height=16 
    inkscape $file --without-gui --export-png=../../bitmaps-dark/16-${filename}@2x.png --export-width=32 --export-height=32 
    
    ## 24x24 version, not needed for OSX
    inkscape $file --without-gui --export-png=../../bitmaps-dark/24-${filename}.png --export-width=24 --export-height=24
    inkscape $file --without-gui --export-png=../../bitmaps-dark/24-${filename}@2x.png --export-width=48 --export-height=48 

    ## 32x32 version
    inkscape $file --without-gui --export-png=../../bitmaps-dark/32-${filename}.png --export-width=32 --export-height=32 
    inkscape $file --without-gui --export-png=../../bitmaps-dark/32-${filename}@2x.png --export-width=64 --export-height=64 

    ## 64x64 version
    inkscape $file --without-gui --export-png=../../bitmaps-dark/64-${filename}.png --export-width=64 --export-height=64 
    inkscape $file --without-gui --export-png=../../bitmaps-dark/64-${filename}@2x.png --export-width=128 --export-height=128 

    ## 128x128 version
    file=codelite-logo-root.svg
    inkscape $file --without-gui --export-png=../../bitmaps-dark/128-${filename}.png --export-width=128 --export-height=128 
    inkscape $file --without-gui --export-png=../../bitmaps-dark/128-${filename}@2x.png --export-width=256 --export-height=256 
    
    ## 256x256 version
    inkscape $file --without-gui --export-png=../../bitmaps-dark/256-${filename}.png --export-width=256 --export-height=256 
    inkscape $file --without-gui --export-png=../../bitmaps-dark/256-${filename}@2x.png --export-width=512 --export-height=512 
    
    ## 512x512 version
    inkscape $file --without-gui --export-png=../../bitmaps-dark/512-${filename}.png --export-width=512 --export-height=512 
    inkscape $file --without-gui --export-png=../../bitmaps-dark/512-${filename}@2x.png --export-width=1024 --export-height=1024 
fi

cd ../../bitmaps-dark/

##
## Create a zip file named codelite-bitmaps.zip
##
rm -f ../Runtime/codelite-bitmaps-dark.zip
zip ../Runtime/codelite-bitmaps-dark.zip *.png
