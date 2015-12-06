#!/bin/bash

## Convert SVG files into PNG 
## A CodeLite script


files_one_size=`ls *.svg`

rm -f ../../bitmaps/code-completion

for file in $files_one_size
do
    filename="${file%.*}"
    
    ## 16x16 version
    inkscape $file --without-gui --export-png=../../bitmaps/16-${filename}.png --export-width=16 --export-height=16 
    
    ## 16x16 version (hi-res)
    inkscape $file --without-gui --export-png=../../bitmaps/16-${filename}@2x.png --export-width=32 --export-height=32 
done

cd ../../bitmaps

##
## Create a zip file named codelite-bitmaps.zip
##
rm -f ../Runtime/codelite-bitmaps.zip
zip ../Runtime/codelite-bitmaps.zip *.png
