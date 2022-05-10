#!/bin/sh

## Convert SVG files into PNG 
## A CodeLite script


files_one_size=`ls *.svg`
cd 16/
files_16=`ls *.svg`
cd ../24/
files_24=`ls *.svg`
cd ..

rm -f ../bitmaps/*.png

for file in $files_one_size
do
    filename="${file%.*}"
    
    ## 16x16 version
    inkscape $file  --export-filename=../bitmaps/16-${filename}.png --export-width=16 --export-height=16 
    
    ## 16x16 version (hi-res)
    inkscape $file  --export-filename=../bitmaps/16-${filename}@2x.png --export-width=32 --export-height=32 
    
    ## 24x24 version
    inkscape $file  --export-filename=../bitmaps/24-${filename}.png --export-width=24 --export-height=24
    
    ## 24x24 version (hi-res)
    inkscape $file  --export-filename=../bitmaps/24-${filename}@2x.png --export-width=48 --export-height=48
done

for file in $files_16
do
    filename="${file%.*}"
    
    ## 16x16 version
    inkscape 16/$file  --export-filename=../bitmaps/16-${filename}.png --export-width=16 --export-height=16 
    
    ## 16x16 version (hi-res)
    inkscape 16/$file  --export-filename=../bitmaps/16-${filename}@2x.png --export-width=32 --export-height=32 
done

for file in $files_24
do
    filename="${file%.*}"
    
    ## 24x24 version
    inkscape 24/$file  --export-filename=../bitmaps/24-${filename}.png --export-width=24 --export-height=24
    
    ## 24x24 version (hi-res)
    inkscape 24/$file  --export-filename=../bitmaps/24-${filename}@2x.png --export-width=48 --export-height=48
done


cd ../bitmaps

##
## Create a zip file named codelite-bitmaps.zip
##
rm -f ../Runtime/codelite-bitmaps.zip
zip ../Runtime/codelite-bitmaps.zip *.png

