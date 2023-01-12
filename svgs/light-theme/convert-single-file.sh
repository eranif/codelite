#!/bin/sh
## 16x16 version
files=`ls $@`

for file in $files
do
    filename="${file%.*}"
    inkscape $file  --export-filename=../../bitmaps-light/16-${filename}.png --export-width=16 --export-height=16 

    ## 16x16 version (hi-res)
    inkscape $file  --export-filename=../../bitmaps-light/16-${filename}@2x.png --export-width=32 --export-height=32 

    ## 24x24 version
    inkscape $file  --export-filename=../../bitmaps-light/24-${filename}.png --export-width=24 --export-height=24

    ## 24x24 version (hi-res)
    inkscape $file  --export-filename=../../bitmaps-light/24-${filename}@2x.png --export-width=48 --export-height=48
done

cd ../../bitmaps-light

##
## Create a zip file named codelite-bitmaps.zip
##
rm -f ../Runtime/codelite-bitmaps-light.zip
zip ../Runtime/codelite-bitmaps-light.zip *.png

