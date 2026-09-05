# set the plugin name here
set(PLUGIN_NAME "abbreviation")

# Our project is called 'plugin' this is how it will be called in visual studio, and in our makefiles.
project(${PLUGIN_NAME})

# wxWidgets include (this will do all the magic to configure everything)
include("${wxWidgets_USE_FILE}")

wxcrafter_target(
    AbbreviationsSettingsBase
    "${CMAKE_CURRENT_SOURCE_DIR}/abbreviationssettingsbase.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "abbreviationssettingsbase.cpp"
    "abbreviationssettingsbase.hpp"
    "abbreviationssettingsbase_abbreviation_bitmaps.cpp")

file(GLOB SRCS "*.cpp")

# Define the output
add_library(${PLUGIN_NAME} SHARED ${SRCS} ${WXCRAFTER_AbbreviationsSettingsBase_OUTPUTS})

if(UNIX OR APPLE)
    set_target_properties(${PLUGIN_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

if(USE_PCH)
    target_precompile_headers(${PLUGIN_NAME} REUSE_FROM PCH)
endif()

# CodeLite plugins doesn't use the "lib" prefix.
set_target_properties(${PLUGIN_NAME} PROPERTIES PREFIX "")
target_include_directories(${PLUGIN_NAME} PRIVATE "${WXCRAFTER_AbbreviationsSettingsBase_OUTPUTDIR}")
target_link_libraries(${PLUGIN_NAME} ${LINKER_OPTIONS} libcodelite plugin)
# Installation destination
cl_install_plugin(${PLUGIN_NAME})
