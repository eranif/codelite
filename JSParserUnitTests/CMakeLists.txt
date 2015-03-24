# define minimum cmake version
cmake_minimum_required(VERSION 2.8)
project ("JSParserUnitTests")

# It was noticed that when using MinGW gcc it is essential that 'core' is mentioned before 'base'.
find_package(wxWidgets COMPONENTS ${WX_COMPONENTS} REQUIRED)

# wxWidgets include (this will do all the magic to configure everything)
include( "${wxWidgets_USE_FILE}" )

# Validate that -DCL_SRC_ROOT was passed
if ( NOT CL_SRC_ROOT) 
    message(FATAL_ERROR "**** CL_SRC_ROOT variable is not set. Please set to codelite's source folder")
else ( NOT CL_SRC_ROOT )
    message("-- CL_SRC_ROOT is set to ${CL_SRC_ROOT}")
endif( NOT CL_SRC_ROOT )

# Include paths
include_directories(
                    "${CL_SRC_ROOT}/Plugin" 
                    "${CL_SRC_ROOT}/sdk/wxsqlite3/include" 
                    "${CL_SRC_ROOT}/CodeLite" 
                    "${CL_SRC_ROOT}/PCH" 
                    "${CL_SRC_ROOT}/Interfaces" 
                    )

add_definitions(-DWXUSINGDLL_WXSQLITE3)
add_definitions(-DWXUSINGDLL_CL)
add_definitions(-DWXUSINGDLL_SDK)

if (UNIX)
    # Add RPATH
    set (LINKER_OPTIONS -Wl,-rpath,"${PLUGINS_DIR}")
endif()


FILE(GLOB UNIT_TESTS_SRC "*.cpp")
add_executable(JSParserUnitTests ${UNIT_TESTS_SRC})
target_link_libraries(JSParserUnitTests libcodelite)

if (UNIX AND NOT APPLE)
    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC" )
    set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC" )
endif()


if (UNIX)
    set(ADDITIONAL_LIBRARIES "-lutil")
endif()

