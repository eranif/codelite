# set the plugin name here
set(PLUGIN_NAME "CallGraph")

# Our project is called 'plugin' this is how it will be called in visual studio, and in our makefiles.
project(${PLUGIN_NAME})

# wxWidgets include (this will do all the magic to configure everything)
include("${wxWidgets_USE_FILE}")

if(USE_PCH)
    add_definitions(-include "${CL_PCH_FILE}")
    add_definitions(-Winvalid-pch)
endif(USE_PCH)

wxcrafter_target(
    CallGraphUiCallGraph
    "${CMAKE_CURRENT_SOURCE_DIR}/uicallgraph.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "uicallgraph.cpp"
    "uicallgraph.hpp"
    "uicallgraph_callgraph_bitmaps.cpp")

wxcrafter_target(
    CallGraphUiSettings
    "${CMAKE_CURRENT_SOURCE_DIR}/uisettings.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "uisettings.cpp"
    "uisettings.hpp"
    "uisettings_callgraph_bitmaps.cpp")

file(GLOB SRCS "*.cpp")

# Define the output
add_library(${PLUGIN_NAME} SHARED ${SRCS} ${WXCRAFTER_CallGraphUiCallGraph_OUTPUTS} ${WXCRAFTER_CallGraphUiSettings_OUTPUTS})

if(UNIX OR APPLE)
    set_target_properties(${PLUGIN_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# Remove the "lib" prefix from the plugin name
set_target_properties(${PLUGIN_NAME} PROPERTIES PREFIX "")
target_include_directories(${PLUGIN_NAME} PUBLIC "${WXCRAFTER_CallGraphUiCallGraph_OUTPUTDIR}")
target_link_libraries(${PLUGIN_NAME} ${LINKER_OPTIONS} -L"${CL_LIBPATH}" libcodelite plugin)
# Make sure that the plugin will not start build before 'plugin.so' is ready
add_dependencies(${PLUGIN_NAME} plugin)

if(NOT APPLE)
    install(TARGETS ${PLUGIN_NAME} DESTINATION ${PLUGINS_DIR})
endif()
