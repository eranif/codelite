# Our project is called 'codelite' this is how it will be called in visual studio, and in our makefiles.
project(codelite-generate-themes)

# wxWidgets include (this will do all the magic to configure everything)
include(${wxWidgets_USE_FILE})

# prepare list of files
file(GLOB SRCS "codelite-generate-themes/*.cpp")

# The resource file has the manifest that wxWidgets needs on Windows (Common Controls v6)
set(CGT_RES_FILE "")
if(MINGW)
    set(CGT_RES_FILE "codelite-generate-themes/resources.rc")
endif()

# Define the output (a command line tool: a console application, no -mwindows)
add_executable(codelite-generate-themes ${SRCS} ${CGT_RES_FILE})
target_compile_definitions(
    codelite-generate-themes
    PUBLIC wxDEBUG_LEVEL=0
           # Default folders: the themes of this tool, and where lexers.json lives
           CGT_DEFAULT_THEMES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/themes"
           CGT_DEFAULT_OUTPUT_DIR="${CL_SRC_ROOT}/Runtime/lexers")
target_include_directories(codelite-generate-themes PUBLIC "${CL_SRC_ROOT}/Plugin/ThemeImporters")
target_link_libraries(codelite-generate-themes libcodelite plugin)

install(TARGETS codelite-generate-themes DESTINATION ${CL_INSTALL_BIN})
