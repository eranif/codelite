# define minimum cmake version
cmake_minimum_required(VERSION 2.6.2)
 
# Our project is called 'codelite' this is how it will be called in
# visual studio, and in our makefiles. 
project(codelitegcc)

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CL_ROOT}/bin" )

# It was noticed that when using MinGW gcc it is essential that 'core' is mentioned before 'base'.
#find_package(wxWidgets COMPONENTS std aui propgrid stc REQUIRED)

# wxWidgets include (this will do all the magic to configure everything)
#include( "${wxWidgets_USE_FILE}" )

# Include paths
#include_directories("${CL_ROOT}/Plugin" "${CL_ROOT}/sdk/wxsqlite3/include" "${CL_ROOT}/CodeLite" "${CL_ROOT}/PCH" "${CL_ROOT}/Interfaces")

# Add RPATH
set (LINKER_OPTIONS -Wl,-rpath,"${PLUGINS_DIR}")

FILE(GLOB SRCS "*.cpp")

# Define the output
add_executable(codelitegcc ${SRCS})
target_link_libraries(codelitegcc ${LINKER_OPTIONS})

install(TARGETS codelitegcc DESTINATION ${PREFIX}/bin)

