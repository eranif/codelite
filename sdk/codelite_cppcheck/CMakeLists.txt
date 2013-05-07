# define minimum cmake version
cmake_minimum_required(VERSION 2.6.2)
 
# Our project is called 'codelite' this is how it will be called in
# visual studio, and in our makefiles. 
project(codelitegcc)

# Include paths
include_directories("${CL_SRC_ROOT}/sdk/codelite_cppcheck/lib" "${CL_SRC_ROOT}/sdk/codelite_cppcheck/cli")

# Add RPATH
set (LINKER_OPTIONS -Wl,-rpath,"${PLUGINS_DIR}")

FILE(GLOB SRCS "cli/*.cpp" "lib/*.cpp")

# Define the output
add_executable(codelite_cppcheck ${SRCS})
target_link_libraries(codelite_cppcheck ${LINKER_OPTIONS})

install(TARGETS codelite_cppcheck DESTINATION ${CL_PREFIX}/bin)

