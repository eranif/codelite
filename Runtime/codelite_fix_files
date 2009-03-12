#!/bin/sh

files=`find . -name "*.project"`
wsp_files=`find . -name "*.workspace"`

##
## fix workspace
##
for file in $wsp_files
do
	echo fixing file $file ...
	## replace any windows \ with unix /
	\rm  -f $file.tmp
	sed 's/\\/\//g' $file > $file.tmp
	mv $file.tmp $file
done

##
## fix project files
##
for file in $files
do
	echo fixing file $file ...
	## replace any windows \ with unix /
	\rm  -f $file.tmp
	sed 's/\\/\//g' $file > $file.tmp
	mv $file.tmp $file
done
echo done

