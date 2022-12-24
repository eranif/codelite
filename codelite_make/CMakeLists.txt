# Our project is called 'codelite' this is how it will be called in
# visual studio, and in our makefiles.
project(codelite-make)

# It was noticed that when using MinGW gcc it is essential that 'core' is mentioned before 'base'.
include(${wxWidgets_USE_FILE})

# prepare list of files
FILE(GLOB SRCS "*.cpp")

# and finally prepare list of includes directories
include_directories("${CL_SRC_ROOT}/Plugin"
                    "${CL_SRC_ROOT}/sdk/wxsqlite3/include"
                    "${CL_SRC_ROOT}/CodeLite"
                    "${CL_SRC_ROOT}/PCH"
                    "${CL_SRC_ROOT}/Interfaces")
add_definitions(-DwxDEBUG_LEVEL=0)

# Define the output
add_executable(codelite-make ${SRCS})
set ( UTIL_LIB "" )
if ( UNIX )
    set( UTIL_LIB "-lutil")
endif ( UNIX )

target_link_libraries(codelite-make ${LINKER_OPTIONS} ${wxWidgets_LIBRARIES} -L"${CL_LIBPATH}"
                        libcodelite plugin ${UTIL_LIB} )

CL_INSTALL_EXECUTABLE(codelite-make)
