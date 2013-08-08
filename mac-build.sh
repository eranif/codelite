#!/bin/bash

BUILD_DIR=build-release
CMAKE_COMMAND="cmake .. "

##
## Usage
##
usage()
{
    cat <<EOF
Usage:
    `basename $0` [options]
    
Options:
    -h | --help     show this help screen
    -d | --debug    build codelite.app bundle in debug mode (the default is to build in release mode)
    -w | --wxc      enable wxCrafter plugin
EOF
    exit 0
}

##
## Parse command line arguments
##
parse_command_line()
{
    for i in "$@"
    do
    case -h|--help)
        usage()
        ;;
    case -d|--debug)
        BUILD_DIR=build-debug
        CMAKE_COMMAND="${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug "
        ;;
    
    case -w|--wxc)
        CMAKE_COMMAND="${CMAKE_COMMAND} -DWITH_WXC=1 "
        ;;
    done
}

echo "Build directory: " ${BUILD_DIR}
echo "CMake command  : " ${CMAKE_COMMAND}
exit 0
