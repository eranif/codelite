# Our project is called 'codelite' this is how it will be called in visual studio, and in our makefiles.
project(PCH)

# wxWidgets include (this will do all the magic to configure everything)
include("${wxWidgets_USE_FILE}")

file(GLOB SRCS "dummy.cpp")
add_library(PCH STATIC ${SRCS})

if(UNIX OR APPLE)
    set_target_properties(PCH PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# generate PCH for this project, we will reuse it later
codelite_add_exported_pch(PCH)
