################################################################################
## This file is part of CodeLite IDE and is released
## under the terms of the GNU General Public License as published by
##    the Free Software Foundation; either version 2 of the License, or
##    (at your option) any later version.
################################################################################

#!/bin/sh
has_file=0
has_svn=0

if (test -f LiteEditor/svninfo.cpp); then
	has_file=1
fi

if (test -d ".svn"); then
	has_svn=1
	cur_rev=`LC_ALL=C svn info | grep Revision | awk '{print $2;}'`
else
	cur_rev=""
fi

## generate the svninfo file
## we always do this if svn is available, but even if it isn't, create a 'blank' file if none exists
if [ $has_svn -eq 1 ] || [ $has_file -eq 0 ]; then
	echo "#include <wx/string.h>" > LiteEditor/svninfo.cpp
	printf "const wxChar * SvnRevision = wxT(\"%s\");\n" ${cur_rev} >> LiteEditor/svninfo.cpp
	echo "" >> LiteEditor/svninfo.cpp
	echo "Generating svninfo file..."
fi

