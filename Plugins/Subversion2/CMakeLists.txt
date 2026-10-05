# set the plugin name here
set(PLUGIN_NAME "Subversion")

# Our project is called 'plugin' this is how it will be called in visual studio, and in our makefiles.
project(${PLUGIN_NAME})

# wxWidgets include (this will do all the magic to configure everything)
include("${wxWidgets_USE_FILE}")

if(USE_PCH AND NOT MINGW)
    add_definitions(-include "${CL_PCH_FILE}")
    add_definitions(-Winvalid-pch)
endif()

wxcrafter_target(
    SubversionSubversion2
    "${CMAKE_CURRENT_SOURCE_DIR}/subversion2.wxcp"
    "subversion2_ui.cpp"
    "subversion2_ui.hpp")

wxcrafter_target(
    SubversionWxCrafter
    "${CMAKE_CURRENT_SOURCE_DIR}/wxcrafter.wxcp"
    "wxcrafter.cpp"
    "wxcrafter.hpp")

file(GLOB SRCS "*.cpp")

# Define the output
add_library(${PLUGIN_NAME} SHARED ${SRCS} ${WXCRAFTER_SubversionSubversion2_OUTPUTS} ${WXCRAFTER_SubversionWxCrafter_OUTPUTS})

if(UNIX OR APPLE)
    set_target_properties(${PLUGIN_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

if(USE_PCH)
    target_precompile_headers(${PLUGIN_NAME} REUSE_FROM PCH)
endif()

# CodeLite plugins doesn't use the "lib" prefix.
set_target_properties(${PLUGIN_NAME} PROPERTIES PREFIX "")
target_include_directories(${PLUGIN_NAME} PUBLIC "${WXCRAFTER_SubversionSubversion2_OUTPUTDIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(${PLUGIN_NAME} ${LINKER_OPTIONS} libcodelite plugin)

# Installation destination
cl_install_plugin(${PLUGIN_NAME})
