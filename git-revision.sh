#!/bin/bash

GIT_VERSION=`git describe --tags`

if test -f LiteEditor/autoversion.cpp ; then 
    rm -f LiteEditor/autoversion.cpp
fi

echo "#ifndef CL_GIT_REVISION" > LiteEditor/autoversion.cpp
echo "#define CL_GIT_REVISION" >> LiteEditor/autoversion.cpp
echo "" >> LiteEditor/autoversion.cpp

echo "#include \"wx/string.h\"" >> LiteEditor/autoversion.cpp

echo "const wxChar* clGitRevision = wxT(\"${GIT_VERSION}\");" >> LiteEditor/autoversion.cpp
echo "#endif" >> LiteEditor/autoversion.cpp
echo "" >> LiteEditor/autoversion.cpp


