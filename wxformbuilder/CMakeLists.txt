# set the plugin name here
set(PLUGIN_NAME "wxFormBuilder")

# Our project is called 'plugin' this is how it will be called in visual studio, and in our makefiles.
project(${PLUGIN_NAME})

# wxWidgets include (this will do all the magic to configure everything)
include("${wxWidgets_USE_FILE}")

if(USE_PCH AND NOT MINGW)
    add_definitions(-include "${CL_PCH_FILE}")
    add_definitions(-Winvalid-pch)
endif()

wxcrafter_target(
    wxFormBuilderNewWxItem
    "${CMAKE_CURRENT_SOURCE_DIR}/NewWxItem.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "wxfbitembasedlg.cpp"
    "wxfbitembasedlg.hpp"
    "NewWxItem_wxformbuilder_bitmaps.cpp")

wxcrafter_target(
    wxFormBuilderSettingsBaseDlg
    "${CMAKE_CURRENT_SOURCE_DIR}/formbuildsettingsbasedlg.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "formbuildsettingsbasedlg.cpp"
    "formbuildsettingsbasedlg.hpp"
    "formbuildsettingsbasedlg_wxformbuilder_bitmaps.cpp")

file(GLOB SRCS "*.cpp")

# Define the output
add_library(${PLUGIN_NAME} SHARED ${SRCS} ${WXCRAFTER_wxFormBuilderNewWxItem_OUTPUTS} ${WXCRAFTER_wxFormBuilderSettingsBaseDlg_OUTPUTS})

if(UNIX OR APPLE)
    set_target_properties(${PLUGIN_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

if(USE_PCH)
    target_precompile_headers(${PLUGIN_NAME} REUSE_FROM PCH)
endif()

# Remove the "lib" prefix from the plugin name
set_target_properties(${PLUGIN_NAME} PROPERTIES PREFIX "")
target_include_directories(${PLUGIN_NAME} PUBLIC "${WXCRAFTER_wxFormBuilderSettingsBaseDlg_OUTPUTDIR}")
target_link_libraries(${PLUGIN_NAME} ${LINKER_OPTIONS} libcodelite plugin)
cl_install_plugin(${PLUGIN_NAME})
