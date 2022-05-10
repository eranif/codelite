#!/bin/sh
files=`ls *.svg`

for file in $files
do
    echo "Converting file ${file}"
    filename="${file%.*}"
    inkscape $file --export-png=12-${filename}.png --export-width=12 --export-height=12

    ## 12x12 version (hi-res)
    inkscape $file --export-png=12-${filename}@2x.png --export-width=24 --export-height=24 
done

