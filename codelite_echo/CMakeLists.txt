# define minimum cmake version
cmake_minimum_required(VERSION 2.6.2)
 
# Our project is called 'codelite' this is how it will be called in
# visual studio, and in our makefiles. 
project(codelite-echo)

# prepare list of files
set(SRCS "main.c")

# Define the output
add_executable(codelite-echo ${SRCS})
install(TARGETS codelite-echo DESTINATION ${CL_PREFIX}/bin)
