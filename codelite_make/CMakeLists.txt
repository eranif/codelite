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

if(APPLE)
    # Embed an Info.plist with LSUIElement=1 so launch services knows this is a
    # background/console tool; this prevents a dock icon from flashing at startup.
    target_link_options(codelite-make PRIVATE
        "LINKER:-sectcreate,__TEXT,__info_plist,${CMAKE_CURRENT_SOURCE_DIR}/Info.plist")
endif()

cl_install_executable(codelite-make)
