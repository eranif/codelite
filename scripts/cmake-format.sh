#!/bin/bash

cmake_files=$(find . -name "CMakeLists.txt"|grep -v "submodules"|grep -v "build-*"|grep -v ctagsd)
for cmake_lists in $cmake_files; do
  echo Formatting ... $cmake_lists
  cmake-format --line-width=120 --tab-size=4 -i $cmake_lists
done

