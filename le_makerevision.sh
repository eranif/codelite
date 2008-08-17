#!/bin/sh
has_file=0
has_svn=0

if (test -f LiteEditor/svninfo.cpp); then
	has_file=1
fi

if (test -d ".svn"); then
	has_svn=1
fi

# not under svn, but we do have the svninfo file, do nothing 
if [ $has_file -eq 1 ] && [ $has_svn -eq 0 ]; then
       ## file does not exist, create an empty file
       return 0
fi

# under svn
rm -fr LiteEditor/svninfo.cpp
if [ $has_svn -eq 1 ]; then
	cur_rev=`svn info | grep Revision | awk '{print $2;}'`
else
	cur_rev=""
fi

## generate the svninfo file 
echo "#include <wx/string.h>" >> LiteEditor/svninfo.cpp
printf "const wxChar * SvnRevision = wxT(\"%s\");\n" ${cur_rev} >> LiteEditor/svninfo.cpp
echo "" >> LiteEditor/svninfo.cpp
echo Generating svninfo file...

