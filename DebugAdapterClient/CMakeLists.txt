# lldb requires C++11
add_definitions(-std=c++17)
set(PLUGIN_NAME "DebugAdapterClient")
project(DebugAdapterClient)

add_subdirectory("${CL_SRC_ROOT}/submodules/wxdap/dap" ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

# wxWidgets include (this will do all the magic to configure everything)
include("${wxWidgets_USE_FILE}")

# By default, use the sources under the current folder
file(GLOB PLUGIN_SRCS "*.cpp")

# Define the output - shared library
add_library(${PLUGIN_NAME} SHARED ${PLUGIN_SRCS})

# Codelite plugins doesn't use the "lib" prefix.
set_target_properties(${PLUGIN_NAME} PROPERTIES PREFIX "")
target_link_libraries(${PLUGIN_NAME} ${LINKER_OPTIONS} ${LIBLLDB} libcodelite plugin dapcxx)
target_include_directories(${PLUGIN_NAME} PRIVATE "${CL_SRC_ROOT}/submodules/wxdap")
# Installation destination
cl_install_plugin(${PLUGIN_NAME})
codelite_install_library_target(dapcxx)
