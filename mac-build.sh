#!/bin/sh

BUILD_DIR=build-release
CMAKE_COMMAND="cmake .. "
RUN_CMAKE=0
MAKE_COMMAND="make -j4 "
CLEAN_CMAKE_CACHE=0

##
## Usage
##
usage()
{
    cat <<EOF
Usage:
    `basename $0` [options]
    
Options:
    -h | --help         show this help screen
    -d | --debug        build codelite.app bundle in debug mode (the default is to build in release mode)
    -w | --wxc          enable wxCrafter plugin
    -c | --clean        perform clean
         --cmake        Run cmake before building
         --clear-cache  Clear cmake cache
EOF
    exit 0
}

##
## Parse command line arguments
##
for i in "$@"
do
    case $i in
        "--clear-cache")
            CLEAN_CMAKE_CACHE=1
            ;;
        "-h" | "--help")
            usage
            ;;
        "-d" | "--debug")
            BUILD_DIR=build-debug
            CMAKE_COMMAND="${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug "
            ;;
        "-w" | "--wxc")
            CMAKE_COMMAND="${CMAKE_COMMAND} -DWITH_WXC=1 "
            ;;
        "--cmake")
            RUN_CMAKE=1
            ;;
        "--clean" | "-c")
            MAKE_COMMAND="${MAKE_COMMAND} clean "
            ;;
        * )
            ;;
    esac
done

echo "-- Build directory: " ${BUILD_DIR}
echo "-- CMake command  : " ${CMAKE_COMMAND}

## Make sure we got the build directory properly
mkdir -p ${BUILD_DIR}

## cd ...
cd ${BUILD_DIR}

## Execute cmake
if [ ${RUN_CMAKE} -eq 1 ]; then
    if [ ${CLEAN_CMAKE_CACHE} -eq 1 ]; then
        echo "-- Clearing CMake cache file"
        rm -f CMakeCache.txt
    fi
    ${CMAKE_COMMAND}
fi

${MAKE_COMMAND}

echo "-- Creating bundle codelite.app ...."
cd pack && ./make_mac_bundle.sh
echo "-- Done"
