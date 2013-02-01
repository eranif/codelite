#!/bin/bash

GIT_VERSION=`git rev-parse --short HEAD`

if test -f autoversion.cpp ; then 
    rm autoversion.cpp
fi

echo "#ifndef CL_GIT_REVISION" > autoversion.cpp
echo "#define CL_GIT_REVISION" >> autoversion.cpp
echo "" >> autoversion.cpp
echo "const char* clGitRevision = \"${GIT_VERSION}\";" >> autoversion.cpp
echo "#endif" >> autoversion.cpp
echo "" >> autoversion.cpp


