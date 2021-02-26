# define minimum cmake version
cmake_minimum_required(VERSION 3.0)
 
# Our project is called 'codelite' this is how it will be called in
# visual studio, and in our makefiles. 
project(PCH)

set(os "")

if(APPLE)
    set(os "osx")
endif()

if ( CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES DebugFull )
    add_custom_target (
        ${CL_PCH_TARGET} ALL
        DEPENDS "${CL_PCH_FILE}"
        COMMAND "$(MAKE)" type=debug os=$(os)
        WORKING_DIRECTORY ${CL_SRC_ROOT}/PCH
     )
else()
    add_custom_target (
        ${CL_PCH_TARGET} ALL
        DEPENDS "${CL_PCH_FILE}"
        COMMAND "$(MAKE)" type=release os=$(os)
        WORKING_DIRECTORY ${CL_SRC_ROOT}/PCH
     )
endif()

message("-- Adding PCH ${CL_PCH_FILE} " )
find_package(wxWidgets COMPONENTS std REQUIRED)

# wxWidgets include (this will do all the magic to configure everything)
include( "${wxWidgets_USE_FILE}" )

FILE(GLOB SRCS "*.cpp")

add_library(PCH STATIC ${SRCS})
