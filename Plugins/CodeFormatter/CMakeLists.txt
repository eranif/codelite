# set the plugin name here
set(PLUGIN_NAME "CodeFormatter")

# Our project is called 'plugin' this is how it will be called in visual studio, and in our makefiles.
project(${PLUGIN_NAME})

# wxWidgets include (this will do all the magic to configure everything)
include("${wxWidgets_USE_FILE}")

if(USE_PCH AND NOT MINGW)
    add_definitions(-include "${CL_PCH_FILE}")
    add_definitions(-Winvalid-pch)
endif()

wxcrafter_target(
    CodeFormatter
    "${CMAKE_CURRENT_SOURCE_DIR}/codeformatterdlg.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "codeformatterdlgbase.cpp"
    "codeformatterdlgbase.hpp"
    "codeformatterdlg_codeformatter_bitmaps.cpp")

file(GLOB SRCS "*.cpp")

# Define the output
add_library(${PLUGIN_NAME} SHARED ${SRCS} ${WXCRAFTER_CodeFormatter_OUTPUTS})

if(UNIX OR APPLE)
    set_target_properties(${PLUGIN_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

if(USE_PCH)
    target_precompile_headers(${PLUGIN_NAME} REUSE_FROM PCH)
endif()

# Remove the "lib" prefix from the plugin name
set_target_properties(${PLUGIN_NAME} PROPERTIES PREFIX "")
target_include_directories(${PLUGIN_NAME} PUBLIC "${WXCRAFTER_CodeFormatter_OUTPUTDIR}")
target_link_libraries(${PLUGIN_NAME} ${LINKER_OPTIONS} libcodelite plugin)

cl_install_plugin(${PLUGIN_NAME})

if(MINGW)
    # install clang-format
    msys_install_clang64_tool(clang-format.exe ${CL_INSTALL_BIN})
endif()
