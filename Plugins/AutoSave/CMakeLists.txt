# set the plugin name here
set(PLUGIN_NAME "AutoSave")

# Our project is called 'plugin' this is how it will be called in visual studio, and in our makefiles.
project(${PLUGIN_NAME})

# wxWidgets include (this will do all the magic to configure everything)
include("${wxWidgets_USE_FILE}")

wxcrafter_target(
    AutoSave
    "${CMAKE_CURRENT_SOURCE_DIR}/AutoSaveUI.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "AutoSaveUI.cpp"
    "AutoSaveUI.hpp"
    "AutoSaveUI_autosave_bitmaps.cpp")

file(GLOB SRCS "*.cpp")

# Define the output
add_library(${PLUGIN_NAME} SHARED ${SRCS} ${WXCRAFTER_AutoSave_OUTPUTS})

if(UNIX OR APPLE)
    set_target_properties(${PLUGIN_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

if(USE_PCH)
    target_precompile_headers(${PLUGIN_NAME} REUSE_FROM PCH)
endif()

# Remove the "lib" prefix from the plugin name
set_target_properties(${PLUGIN_NAME} PROPERTIES PREFIX "")
target_include_directories(${PLUGIN_NAME} PUBLIC "${WXCRAFTER_AutoSave_OUTPUTDIR}")
target_link_libraries(${PLUGIN_NAME} ${LINKER_OPTIONS} libcodelite plugin)

# Use CodeLite's macro: CL_INSTALL_PLUGIN which handles both OSX and Linux installation
cl_install_plugin(${PLUGIN_NAME})
