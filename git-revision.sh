#!/bin/bash

GIT_VERSION=`git rev-parse --short HEAD`

if test -f LiteEditor/autoversion.cpp ; then 
    rm LiteEditor/autoversion.cpp
fi

echo "#ifndef CL_GIT_REVISION" > LiteEditor/autoversion.cpp
echo "#define CL_GIT_REVISION" >> LiteEditor/autoversion.cpp
echo "" >> LiteEditor/autoversion.cpp

#echo "#include \"wx/chartype.h\"" >> LiteEditor/autoversion.cpp

echo "const char* clGitRevision = \"${GIT_VERSION}\";" >> LiteEditor/autoversion.cpp
echo "#endif" >> LiteEditor/autoversion.cpp
echo "" >> LiteEditor/autoversion.cpp


