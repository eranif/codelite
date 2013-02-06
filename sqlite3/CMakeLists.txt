# define minimum cmake version
cmake_minimum_required(VERSION 2.6.2)
 
# Our project is called 'sqlite' this is how it will be called in
# visual studio, and in our makefiles. 
project(sqlite)

set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ../lib )
 
# For convenience we define the sources as a variable. You can add 
# header files and cpp/c files and CMake will sort them out
FILE(GLOB SRCS "*.c")
 
#if(WIN32)
#  set(SRCS ${SRCS} sqlite.rc)
#endif(WIN32)

# Define the output
add_library(sqlite SHARED ${SRCS})
