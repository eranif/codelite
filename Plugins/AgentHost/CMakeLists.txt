# set the plugin name here
set(PLUGIN_NAME "AgentHost")

# Our project is called 'plugin' this is how it will be called in visual studio,
# and in our makefiles.
project(${PLUGIN_NAME})

# wxWidgets include (this will do all the magic to configure everything)
include("${wxWidgets_USE_FILE}")

if(USE_PCH AND NOT MINGW)
  add_definitions(-include "${CL_PCH_FILE}")
  add_definitions(-Winvalid-pch)
endif()

wxcrafter_target(
  AgentHost "${CMAKE_CURRENT_SOURCE_DIR}/AgentHostUI.wxcp"
  "${CMAKE_CURRENT_BINARY_DIR}" "AgentHostUI.cpp" "AgentHostUI.hpp"
  "AgentHostUI_agenthost_bitmaps.cpp")

file(GLOB SRCS "*.cpp")

# Define the output
add_library(${PLUGIN_NAME} SHARED ${SRCS} ${WXCRAFTER_AgentHost_OUTPUTS})

if(UNIX OR APPLE)
  set_target_properties(${PLUGIN_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

if(USE_PCH)
  target_precompile_headers(${PLUGIN_NAME} REUSE_FROM PCH)
endif()

message(
  STATUS "WXCRAFTER_AgentHost_OUTPUTDIR => ${WXCRAFTER_AgentHost_OUTPUTDIR}")
# Remove the "lib" prefix from the plugin name
set_target_properties(${PLUGIN_NAME} PROPERTIES PREFIX "")
target_include_directories(${PLUGIN_NAME}
                           PUBLIC "${WXCRAFTER_AgentHost_OUTPUTDIR}")
target_link_libraries(${PLUGIN_NAME} ${LINKER_OPTIONS} libcodelite plugin)

cl_install_plugin(${PLUGIN_NAME})
