## lldb requires C++11
add_definitions(-std=c++11)
set(PLUGIN_NAME "DebugAdapterClient")
project(DebugAdapterClient)

add_subdirectory("${CL_SRC_ROOT}/wxdap/dap" ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

# wxWidgets include (this will do all the magic to configure everything)
include("${wxWidgets_USE_FILE}")

# Include paths
include_directories("${CL_SRC_ROOT}/Plugin"
                    "${CL_SRC_ROOT}/sdk/wxsqlite3/include"
                    "${CL_SRC_ROOT}/CodeLite"
                    "${CL_SRC_ROOT}/PCH"
                    "${CL_SRC_ROOT}/Interfaces"
                    "${CL_SRC_ROOT}/wxdap"
                    )
## Definitions
add_definitions(-DWXUSINGDLL_WXSQLITE3)
add_definitions(-DWXUSINGDLL_CL)
add_definitions(-DWXUSINGDLL_SDK)
add_definitions(-DWXUSINGDLL_DAP)

## By default, use the sources under the current folder
FILE(GLOB PLUGIN_SRCS "*.cpp")

# Define the output - shared library
add_library(${PLUGIN_NAME} SHARED ${PLUGIN_SRCS})

# Codelite plugins doesn't use the "lib" prefix.
set_target_properties(${PLUGIN_NAME} PROPERTIES PREFIX "")
target_link_libraries(${PLUGIN_NAME}
                      ${LINKER_OPTIONS}
                      ${wxWidgets_LIBRARIES}
                      ${LIBLLDB}
                      libcodelite
                      plugin
                      dapcxx)
# Installation destination
CL_INSTALL_PLUGIN(${PLUGIN_NAME})
codelite_install_library_target(dapcxx)
