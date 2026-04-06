# Our project is called 'codelite' this is how it will be called in visual studio, and in our makefiles.
project(codelite-generate-themes)

# wxWidgets include (this will do all the magic to configure everything)
include(${wxWidgets_USE_FILE})

# prepare list of files
file(GLOB SRCS "codelite-generate-themes/*.cpp")

include_directories("${CL_SRC_ROOT}/Plugin/ThemeImporters")

add_compile_definitions(wxDEBUG_LEVEL=0)

set(CGT_RES_FILE "")
if(MINGW)
    set(LINKER_OPTIONS "-mwindows")
    set(CGT_RES_FILE "codelite-generate-themes/resources.rc")
endif()

# Define the output
add_executable(codelite-generate-themes ${SRCS} ${CGT_RES_FILE})

target_link_libraries(codelite-generate-themes libcodelite plugin ${LINKER_OPTIONS})

install(TARGETS codelite-generate-themes DESTINATION ${CL_INSTALL_BIN})
