# set the plugin name heres
set(PLUGIN_NAME "wxCrafter")

# Our project is called 'plugin' this is how it will be called in visual studio, and in our makefiles.
project(${PLUGIN_NAME})

# wxWidgets include (this will do all the magic to configure everything)
include("${wxWidgets_USE_FILE}")

if(USE_PCH AND NOT MINGW)
    add_definitions(-include "${CL_PCH_FILE}")
    add_definitions(-Winvalid-pch)
endif()

file(GLOB_RECURSE SRCS "*.cpp")

# Define the output
add_library(${PLUGIN_NAME} SHARED ${SRCS})

if(UNIX OR APPLE)
  set_target_properties(${PLUGIN_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

if(USE_PCH)
  target_precompile_headers(${PLUGIN_NAME} REUSE_FROM PCH)
endif()

# Remove the "lib" prefix from the plugin name
set_target_properties(${PLUGIN_NAME} PROPERTIES PREFIX "")
target_include_directories(${PLUGIN_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}")
target_link_libraries(${PLUGIN_NAME} PRIVATE wxCrafterCommonGui)

# Add RPATH When building an application, set the install library to the proper
# location which is now under /usr/lib/x86_64-linux-gnu/wxcrafter
if(NOT MINGW AND NOT OFFICIAL_CL_BUILD)
  set(LINKER_OPTIONS -Wl,-rpath,"${PLUGINS_DIR}")
endif()
if(NOT OFFICIAL_CL_BUILD)
  target_link_libraries(
    ${PLUGIN_NAME} PRIVATE ${LINKER_OPTIONS} ${SQLite3_LIBRARIES} libcodelite plugin)
else(OFFICIAL_CL_BUILD)
  target_link_libraries(${PLUGIN_NAME} PRIVATE ${LINKER_OPTIONS})
endif(NOT OFFICIAL_CL_BUILD)

cl_install_plugin(${PLUGIN_NAME})

if(NOT CL_SRC_ROOT)
  message(FATAL_ERROR "Missing CL_SRC_ROOT")
endif()
cl_install_file_shared("${CL_SRC_ROOT}/wxcrafter/wxgui.zip")
