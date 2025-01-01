# Don't support very old versions

# Create variable for plugin name
set(PLUGIN_NAME "CodeLiteDiff")

# Define project name
project(${PLUGIN_NAME})

# Find wxWidgets with required components

# wxWidgets include
include("${wxWidgets_USE_FILE}")

# Only with precompiled headers
if(USE_PCH AND NOT MINGW)
    add_definitions(-include "${CL_PCH_FILE}")
    add_definitions(-Winvalid-pch)
endif()

# Add all CPP files
file(GLOB SRCS "*.cpp")

# Define the output - shared library
add_library(${PLUGIN_NAME} SHARED ${SRCS})

if(UNIX OR APPLE)
    set_target_properties(${PLUGIN_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

if(USE_PCH)
    target_precompile_headers(${PLUGIN_NAME} REUSE_FROM PCH)
endif()

# Codelite plugins doesn't use the "lib" prefix.
set_target_properties(${PLUGIN_NAME} PROPERTIES PREFIX "")
target_link_libraries(${PLUGIN_NAME} ${LINKER_OPTIONS} libcodelite plugin)
# Installation destination
cl_install_plugin(${PLUGIN_NAME})
