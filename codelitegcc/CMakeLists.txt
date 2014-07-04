# define minimum cmake version
cmake_minimum_required(VERSION 2.6.2)
 
# Our project is called 'codelite' this is how it will be called in
# visual studio, and in our makefiles. 
project(codelite-cc)

# Add RPATH
if (UNIX)
set (LINKER_OPTIONS -Wl,-rpath,"${PLUGINS_DIR}")
endif (UNIX)

FILE(GLOB SRCS "*.cpp")

# Define the output
add_executable(codelite-cc ${SRCS})
target_link_libraries(codelite-cc ${LINKER_OPTIONS})

install(TARGETS codelite-cc DESTINATION ${CL_PREFIX}/bin)
