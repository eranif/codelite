#!/bin/bash

# locate the root directory
ROOT_DIR=$1
if [ ! -f "${ROOT_DIR}/install_manifest.txt" ]; then
    echo "$0 <codelite-build-dir>"
    exit 1
fi

dylibs=$(cat ${ROOT_DIR}/install_manifest.txt|grep ".dylib")
echo $dylibs
