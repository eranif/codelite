# define minimum cmake version
cmake_minimum_required(VERSION 3.0)
 
# Our project is called 'sqlite3' this is how it will be called in
# visual studio, and in our makefiles. 
project(sqlite3lib)
 
# For convenience we define the sources as a variable. You can add 
# header files and cpp/c files and CMake will sort them out
FILE(GLOB SRCS "*.c")
 
# Define the output
add_library(sqlite3lib STATIC  ${SRCS})
if (UNIX)
SET_TARGET_PROPERTIES( sqlite3lib  PROPERTIES COMPILE_FLAGS -fPIC)
endif ()

#install(TARGETS sqlite3 DESTINATION ${PLUGINS_DIR})
