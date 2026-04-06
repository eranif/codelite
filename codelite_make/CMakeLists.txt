# Our project is called 'codelite' this is how it will be called in visual studio, and in our makefiles.
project(codelite-make)

# wxWidgets include (this will do all the magic to configure everything)
include(${wxWidgets_USE_FILE})

# prepare list of files
file(GLOB SRCS "*.cpp")

add_compile_definitions(wxDEBUG_LEVEL=0)

# Define the output
add_executable(codelite-make ${SRCS})
set(UTIL_LIB "")
if(UNIX)
    set(UTIL_LIB "-lutil")
endif(UNIX)

target_link_libraries(codelite-make ${LINKER_OPTIONS} -L"${CL_LIBPATH}" libcodelite plugin ${UTIL_LIB})

cl_install_executable(codelite-make)
