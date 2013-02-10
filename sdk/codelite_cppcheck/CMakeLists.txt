# define minimum cmake version
cmake_minimum_required(VERSION 2.6.2)
 
# Our project is called 'codelite' this is how it will be called in
# visual studio, and in our makefiles. 
project(codelitegcc)

# It was noticed that when using MinGW gcc it is essential that 'core' is mentioned before 'base'.
#find_package(wxWidgets COMPONENTS ${WX_COMPONENTS} REQUIRED)

# wxWidgets include (this will do all the magic to configure everything)
#include( "${wxWidgets_USE_FILE}" )

# Include paths
include_directories("${CL_SRC_ROOT}/sdk/codelite_cppcheck/lib" "${CL_SRC_ROOT}/sdk/codelite_cppcheck/cli")

# Add RPATH
set (LINKER_OPTIONS -Wl,-rpath,"${PLUGINS_DIR}")

FILE(GLOB SRCS "cli/*.cpp" "lib/*.cpp")

# Define the output
add_executable(codelite_cppcheck ${SRCS})
target_link_libraries(codelite_cppcheck ${LINKER_OPTIONS})

install(TARGETS codelite_cppcheck DESTINATION ${CL_PREFIX}/bin)

