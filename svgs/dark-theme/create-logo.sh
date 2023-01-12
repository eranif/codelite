#!/bin/sh

filename=codelite-logo

mkdir -p ../../bitmaps-dark/osx/icon.iconset
os_name=`uname -s`

file=codelite-logo.svg
if [ "${os_name}" != "Darwin" ]; then
    ## 16x16 version
    inkscape -w 16 -h 16 $file -o ../../bitmaps-dark/16-${filename}.png
    inkscape -w 32 -h 32 $file -o ../../bitmaps-dark/16-${filename}@2x.png 
    cp ../../bitmaps-dark/16-${filename}.png ../../bitmaps-dark/osx/icon.iconset/icon_16x16.png
    cp ../../bitmaps-dark/16-${filename}@2x.png ../../bitmaps-dark/osx/icon.iconset/icon_16x16@2x.png
    
    ## 24x24 version, not needed for OSX
    inkscape -w 24 -h 24 $file -o ../../bitmaps-dark/24-${filename}.png 
    inkscape -w 48 -h 48 $file -o ../../bitmaps-dark/24-${filename}@2x.png

    ## 32x32 version
    inkscape -w 32 -h 32 $file -o ../../bitmaps-dark/32-${filename}.png
    inkscape -w 64 -h 64 $file -o ../../bitmaps-dark/32-${filename}@2x.png
    cp ../../bitmaps-dark/32-${filename}.png ../../bitmaps-dark/osx/icon.iconset/icon_32x32.png
    cp ../../bitmaps-dark/32-${filename}@2x.png ../../bitmaps-dark/osx/icon.iconset/icon_32x32@2x.png

    ## 64x64 version
    inkscape -w 64 -h 64 $file -o ../../bitmaps-dark/64-${filename}.png 
    inkscape -w 128 -h 128 $file -o ../../bitmaps-dark/64-${filename}@2x.png 
    
    ## Use the hi-res image from hereon
    ## 128x128 version
    inkscape -w 128 -h 128 $file -o ../../bitmaps-dark/128-${filename}.png 
    inkscape -w 256 -h 256 $file -o ../../bitmaps-dark/128-${filename}@2x.png 
    cp ../../bitmaps-dark/128-${filename}.png ../../bitmaps-dark/osx/icon.iconset/icon_128x128.png
    cp ../../bitmaps-dark/128-${filename}@2x.png ../../bitmaps-dark/osx/icon.iconset/icon_128x128@2x.png

    ## 256x256 version
    inkscape -w 256 -h 256 $file -o ../../bitmaps-dark/256-${filename}.png 
    inkscape -w 512 -h 512 $file -o ../../bitmaps-dark/256-${filename}@2x.png 
    cp ../../bitmaps-dark/256-${filename}.png ../../bitmaps-dark/osx/icon.iconset/icon_256x256.png
    cp ../../bitmaps-dark/256-${filename}@2x.png ../../bitmaps-dark/osx/icon.iconset/icon_256x256@2x.png

    ## 512x512 version
    inkscape -w 512 -h 512 $file -o ../../bitmaps-dark/512-${filename}.png 
    inkscape -w 1024 -h 1024 $file -o ../../bitmaps-dark/512-${filename}@2x.png 
    cp ../../bitmaps-dark/512-${filename}.png ../../bitmaps-dark/osx/icon.iconset/icon_512x512.png
    cp ../../bitmaps-dark/512-${filename}@2x.png ../../bitmaps-dark/osx/icon.iconset/icon_512x512@2x.png
fi

cd ../../bitmaps-dark

##
## Create a zip file named codelite-bitmaps.zip
##
rm -f ../Runtime/codelite-bitmaps-dark.zip
zip ../Runtime/codelite-bitmaps-dark.zip *.png

if [ "${os_name}" == "Darwin" ]; then
    cd osx
    echo iconutil -c icns icon.iconset/
    iconutil -c icns icon.iconset/
fi

# create codelite-log.ico
convert 16-codelite-logo.png    \
        24-codelite-logo.png    \
        24-codelite-logo@2x.png \
        32-codelite-logo.png    \
        64-codelite-logo.png    \
        128-codelite-logo.png   \
        256-codelite-logo.png   \
        ../LiteEditor/res/codelite-logo.ico
