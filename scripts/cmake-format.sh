#!/bin/bash

cmake_files=$(find . -name "CMakeLists.txt"|grep -v yaml-cpp|grep -v "build-*"|grep -v "wxdap"|grep -v ctagsd|grep -v wx-config-msys2|grep -v "ctags")
for cmake_lists in $cmake_files; do
  echo Formatting ... $cmake_lists
  cmake-format --line-width=120 --tab-size=4 -i $cmake_lists
done

