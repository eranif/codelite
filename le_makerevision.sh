#!/bin/sh
file_exist=`test -f LiteEditor/svninfo.cpp`
file_exist=$?

cur_rev=`svn info | grep Revision | awk '{print $2;}'`
if [ $file_exist -eq 1 ] 
then
	## file does not exist, create an empty file
	touch LiteEditor/svninfo.cpp
fi
src_rev=`cat LiteEditor/svninfo.cpp | awk '{print $4;}' | sed s/\"//g | sed s/\;//g`

if [ "$src_rev" != "$cur_rev" ]
then
	## generate the svninfo file again
	\cp /dev/null LiteEditor/svninfo.cpp
	echo char *SvnRevision = \"$cur_rev\"";" >> LiteEditor/svninfo.cpp
	echo "" >> LiteEditor/svninfo.cpp
	echo Generating svninfo file...
fi

