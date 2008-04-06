#!/bin/sh
file_exist=`test -f LiteEditor/svninfo.cpp`
file_exist=$?

if [ $file_exist -eq 1 ]
then
       ## file does not exist, create an empty file
       touch LiteEditor/svninfo.cpp
fi

cur_rev=`svn info | grep Revision | awk '{print $2;}'`

rm -fr LiteEditor/svninfo.cpp

## generate the svninfo file 
echo "#include <wx/string.h>" >> LiteEditor/svninfo.cpp
printf "const wxChar * SvnRevision = wxT(\"%s\");\n" ${cur_rev} >> LiteEditor/svninfo.cpp
echo "" >> LiteEditor/svninfo.cpp
echo Generating svninfo file...

