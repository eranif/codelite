# Our project is called 'codelite' this is how it will be called in visual studio, and in our makefiles.
project(codelite-generate-themes)

# wxWidgets include (this will do all the magic to configure everything)
include(${wxWidgets_USE_FILE})

# prepare list of files

wxcrafter_target(
    CodeLiteGenerateThemes
    "${CMAKE_CURRENT_SOURCE_DIR}/codelite-generate-themes/wxcrafter.wxcp"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "wxcrafter.cpp"
    "wxcrafter.hpp"
    "wxcrafter_codelite-generate-themes_bitmaps.cpp")

file(GLOB SRCS "codelite-generate-themes/*.cpp")

set(CGT_RES_FILE "")
if(MINGW)
    set(LINKER_OPTIONS "-mwindows")
    set(CGT_RES_FILE "codelite-generate-themes/resources.rc")
endif()

# Define the output
add_executable(codelite-generate-themes ${SRCS} ${CGT_RES_FILE} ${WXCRAFTER_CodeLiteGenerateThemes_OUTPUTS})
target_compile_definitions(codelite-generate-themes PUBLIC wxDEBUG_LEVEL=0)
target_include_directories(codelite-generate-themes PUBLIC "${CL_SRC_ROOT}/Plugin/ThemeImporters" "${WXCRAFTER_CodeLiteGenerateThemes_OUTPUTDIR}")
target_link_libraries(codelite-generate-themes libcodelite plugin ${LINKER_OPTIONS})

install(TARGETS codelite-generate-themes DESTINATION ${CL_INSTALL_BIN})
