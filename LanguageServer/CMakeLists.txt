# set the plugin name here
set(PLUGIN_NAME "LanguageServer")

# Our project is called 'plugin' this is how it will be called in visual studio, and in our makefiles.
project(${PLUGIN_NAME})

# It was noticed that when using MinGW gcc it is essential that 'core' is mentioned before 'base'.

# wxWidgets include (this will do all the magic to configure everything)
include("${wxWidgets_USE_FILE}")

# Include paths
include_directories("${CL_SRC_ROOT}/LanguageServer")

if(USE_PCH AND NOT MINGW)
    add_definitions(-include "${CL_PCH_FILE}")
    add_definitions(-Winvalid-pch)
endif()

file(GLOB SRCS "*.cpp" "detectors/*.cpp")

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
target_link_libraries(${PLUGIN_NAME} ${LINKER_OPTIONS} libcodelite plugin)

# Use CodeLite's macro: CL_INSTALL_PLUGIN which handles both OSX and Linux installation
cl_install_plugin(${PLUGIN_NAME})

set(EXE_PERM
    OWNER_EXECUTE
    OWNER_WRITE
    OWNER_READ
    GROUP_EXECUTE
    GROUP_READ
    WORLD_EXECUTE
    WORLD_READ)
if(APPLE)
    install(
        FILES ${CL_SRC_ROOT}/LanguageServer/codelite-lsp/codelite-lsp-helper
        DESTINATION ${CMAKE_BINARY_DIR}/codelite.app/Contents/MacOS/
        PERMISSIONS ${EXE_PERM})
endif()

if(MINGW)
    # install clangd
    msys_install_clang64_tool(clangd.exe ${CL_INSTALL_BIN})
endif()
