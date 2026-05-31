# set the plugin name here
set(PLUGIN_NAME "QMakePlugin")

# Our project is called 'plugin' this is how it will be called in visual studio, and in our makefiles.
project(${PLUGIN_NAME})

# wxWidgets include (this will do all the magic to configure everything)
include("${wxWidgets_USE_FILE}")

if(USE_PCH AND NOT MINGW)
    add_definitions(-include "${CL_PCH_FILE}")
    add_definitions(-Winvalid-pch)
endif()

wxcrafter_target(
    QmakePluginNewProj
    "${CMAKE_CURRENT_SOURCE_DIR}/NewQtProj.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "NewQtProj.cpp"
    "NewQtProj.hpp"
    "NewQtProj_qmakeplugin_bitmaps.cpp")
wxcrafter_target(
    QmakePluginSettings
    "${CMAKE_CURRENT_SOURCE_DIR}/qmakesettingsbasedlg.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "qmakesettingsbasedlg.cpp"
    "qmakesettingsbasedlg.hpp"
    "qmakesettingsbasedlg_qmakeplugin_bitmaps.cpp")
wxcrafter_target(
    QmakePluginTab
    "${CMAKE_CURRENT_SOURCE_DIR}/qmaketabbase.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "qmaketabbase.cpp"
    "qmaketabbase.hpp"
    "qmaketabbase_qmakeplugin_bitmaps.cpp")

file(GLOB SRCS "*.cpp")

# Define the output
add_library(${PLUGIN_NAME} SHARED ${SRCS} ${WXCRAFTER_QmakePluginNewProj_OUTPUTS} ${WXCRAFTER_QmakePluginSettings_OUTPUTS} ${WXCRAFTER_QmakePluginTab_OUTPUTS})

if(UNIX OR APPLE)
    set_target_properties(${PLUGIN_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

if(USE_PCH)
    target_precompile_headers(${PLUGIN_NAME} REUSE_FROM PCH)
endif()

# CodeLite plugins doesn't use the "lib" prefix.
set_target_properties(${PLUGIN_NAME} PROPERTIES PREFIX "")
target_include_directories(${PLUGIN_NAME} PUBLIC "${WXCRAFTER_QmakePluginSettings_OUTPUTDIR}")
target_link_libraries(${PLUGIN_NAME} ${LINKER_OPTIONS} libcodelite plugin)
# Installation destination
cl_install_plugin(${PLUGIN_NAME})
