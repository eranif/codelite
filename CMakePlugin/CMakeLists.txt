#
# This file is based on QMakePlugin version of the CMakeLists.txt file.
#

# Don't support very old versions
cmake_minimum_required(VERSION 3.0)

# Create variable for plugin name
set(PLUGIN_NAME "CMakePlugin")

# Define project name
project(${PLUGIN_NAME})

# Find wxWidgets with required components
find_package(wxWidgets COMPONENTS ${WX_COMPONENTS} REQUIRED)

# wxWidgets include
include("${wxWidgets_USE_FILE}")

# Include paths
include_directories(
    "${CL_SRC_ROOT}/Plugin"
    "${CL_SRC_ROOT}/sdk/wxsqlite3/include"
    "${CL_SRC_ROOT}/CodeLite"
    "${CL_SRC_ROOT}/LiteEditor"
    "${CL_SRC_ROOT}/PCH"
    "${CL_SRC_ROOT}/Interfaces"
)

# Define some macros about DLL
add_definitions(-DWXUSINGDLL_CL)
add_definitions(-DWXUSINGDLL_SDK)
add_definitions(-DWXUSINGDLL_SDK)

# Only with precompiled headers
if (USE_PCH)
    add_definitions(-include "${CL_PCH_FILE}")
    add_definitions(-Winvalid-pch)
endif (USE_PCH)

if (UNIX AND NOT APPLE)
    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC" )
    set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC" )
endif()

if ( APPLE )
    add_definitions(-fPIC)
endif()

# Add all CPP files
file(GLOB SRCS "*.cpp")

# Define the output - shared library
add_library(${PLUGIN_NAME} SHARED ${SRCS})

# Codelite plugins doesn't use the "lib" prefix.
set_target_properties(${PLUGIN_NAME} PROPERTIES PREFIX "")
target_link_libraries(${PLUGIN_NAME}
                      ${LINKER_OPTIONS}
                      ${wxWidgets_LIBRARIES}
                      libcodelite
                      plugin
                      )

CL_INSTALL_PLUGIN(${PLUGIN_NAME})
