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

wxcrafter_target(
    CodeLiteDiff
    "${CMAKE_CURRENT_SOURCE_DIR}/CodeLiteDiff_UI.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "CodeLiteDiff_UI.cpp"
    "CodeLiteDiff_UI.hpp"
    "CodeLiteDiff_UI_codelitediff_bitmaps.cpp")

# Add all CPP files
file(GLOB SRCS "*.cpp")

# Define the output - shared library
add_library(${PLUGIN_NAME} SHARED ${SRCS} ${WXCRAFTER_CodeLiteDiff_OUTPUTS})

if(UNIX OR APPLE)
    set_target_properties(${PLUGIN_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

if(USE_PCH)
    target_precompile_headers(${PLUGIN_NAME} REUSE_FROM PCH)
endif()

# CodeLite plugins doesn't use the "lib" prefix.
set_target_properties(${PLUGIN_NAME} PROPERTIES PREFIX "")
target_include_directories(${PLUGIN_NAME} PUBLIC "${WXCRAFTER_CodeLiteDiff_OUTPUTDIR}")
target_link_libraries(${PLUGIN_NAME} ${LINKER_OPTIONS} libcodelite plugin)
# Installation destination
cl_install_plugin(${PLUGIN_NAME})
