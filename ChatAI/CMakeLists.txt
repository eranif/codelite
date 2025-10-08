# set the plugin name here
set(PLUGIN_NAME "ChatAI")

# Our project is called 'plugin' this is how it will be called in visual studio,
# and in our makefiles.
project(ChatAI)

# It was noticed that when using MinGW gcc it is essential that 'core' is
# mentioned before 'base'.

# wxWidgets include (this will do all the magic to configure everything)
include("${wxWidgets_USE_FILE}")

if(USE_PCH AND NOT MINGW)
  add_definitions(-include "${CL_PCH_FILE}")
  add_definitions(-Winvalid-pch)
endif()

file(GLOB SRCS "*.cpp")

# Define the output
add_library(ChatAI SHARED ${SRCS})

if(UNIX OR APPLE)
  set_target_properties(ChatAI PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

if(USE_PCH)
  target_precompile_headers(ChatAI REUSE_FROM PCH)
endif()

# Remove the "lib" prefix from the plugin name
set_target_properties(ChatAI PROPERTIES PREFIX "")
target_link_libraries(ChatAI ${LINKER_OPTIONS} libcodelite plugin assistantlib)

cl_install_plugin(ChatAI)
