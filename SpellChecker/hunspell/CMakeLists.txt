# define minimum cmake version
cmake_minimum_required(VERSION 2.6.2)
project(libhunspell)

FILE(GLOB SRCS "*.cxx")
add_definitions(-DUNICODE)
add_definitions(-DHUNSPELL_STATIC)
add_definitions(-fPIC)
add_library(libhunspell STATIC ${SRCS})
