cmake_minimum_required(VERSION 2.8)
project(dapcxx)
include_directories(${CMAKE_SOURCE_DIR})
FILE(GLOB SRCS "*.cpp")

add_library(dapcxx STATIC ${SRCS})
